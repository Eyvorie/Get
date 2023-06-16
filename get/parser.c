#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "alloc.h"
#include "lexer.h"
#include "parser.h"

AstNode *new_expression(Lexer *lexer);

inline bool is_divergent(AstNode *node)
{
  return node->diverges || node->type == FieldIndex;
}

AstNode *new_node(void *expression, NodeType type, bool diverges)
{
  AstNode *ast_node = new(AstNode);
  ast_node->type = type;
  ast_node->expression = expression;
  ast_node->diverges = diverges;
  return ast_node;
}

UnaryOp *new_unary(AstNode *rhs, Token *op)
{
  UnaryOp *unary = new(UnaryOp);
  unary->op = op->type;
  unary->rhs = rhs;
  return unary;
}

BinaryOp *new_binary(AstNode *lhs, AstNode *rhs, Token *op)
{
  BinaryOp *binary = new(BinaryOp);
  binary->op = op->type;
  binary->lhs = lhs;
  binary->rhs = rhs;
  return binary;
}

Value *parse_int(StringView *raw)
{
  #define MAX_DIGITS 21
  char buffer[MAX_DIGITS];
  sprintf(buffer, "%.*s",
    min(raw->length, MAX_DIGITS - 1), raw->characters);
  return new_integer_value(atoi(buffer));
}

Value *new_literal(Token *literal)
{
  StringView *raw = &literal->value;
  switch (literal->type) {
    case FalseLiteral:
      return new_boolean_value(false);
    case TrueLiteral:
      return new_boolean_value(true);
    case IntegerLiteral:
      return parse_int(raw);
    case StringLiteral: {
      // adjust the raw string value to trim
      // the quotation marks in-front and behind
      // the string value
      StringView trimmed = *raw;
      trimmed.characters += 1, trimmed.length -= 2;
      return new_string_value(
        new_string_from_view(&trimmed));
    }
  }
  assert(false, "Parser error: unsupported literal :(");
  return NULL; // make compiler happy
}

Assignment *new_assignment(AstNode *rhs, Token *identifier)
{
  Assignment *assignment = new(Assignment);
  assignment->identifier = identifier->value;
  assignment->rhs = rhs;
  return assignment;
}

Variable *new_variable(Token *identifier)
{
  Variable *variable = new(Variable);
  variable->identifier = identifier->value;
  return variable;
}

NetworkOp *new_network_request(AstNode *rhs, Token *op)
{
  NetworkOp *network_request = new(NetworkOp);
  network_request->op = op->type;
  network_request->rhs = rhs;
  return network_request;
}

AstNode *new_factor(Lexer *lexer)
{
  Token token;
  peak_next_lexer_token(lexer, &token);
  switch (token.type) {
    case Get:
    case Post:
    case Put:
    case Delete: {
      consume_lexer_token(lexer, &token);
      AstNode *factor = new_factor(lexer);
      return new_node(new_network_request(factor, &token),
        NetworkRequest, is_divergent(factor));
    }
    case IntegerLiteral:
    case TrueLiteral:
    case FalseLiteral:
    case StringLiteral: {
      consume_lexer_token(lexer, &token);
      return new_node(new_literal(&token),
        Literal, false);
    }
    case Print: {
      consume_lexer_token(lexer, &token);
      AstNode *factor = new_factor(lexer);
      return new_node(new_unary(factor, &token),
        Unary, is_divergent(factor));
    }
    case Identifier: {
      consume_lexer_token(lexer, &token);
      return new_node(new_variable(&token),
        VariableCall, true);
    }
    case DollarSign: {
      // adjust the raw string value to trim
      // the dollar sign in-front of the index
      // e.g. $0 -> 0
      consume_lexer_token(lexer, &token);
      token.value.characters += 1, token.value.length -= 1;
      return new_node(parse_int(&token.value),
        FieldIndex, true);
    }
    case LeftParen: {
      consume_lexer_token(lexer, &token);
      AstNode *contents = new_expression(lexer);
      peak_next_lexer_token(lexer, &token);
      assert(token.type == RightParen, "Parser error: missing closing bracket");
      consume_lexer_token(lexer, &token);
      return contents;
    }
  }
  assert(false, "Parser error: invalid factor :(");
  return NULL; // make compiler happy
}

AstNode *new_term(Lexer *lexer)
{
  AstNode *node = new_factor(lexer);

  Token token;
  peak_next_lexer_token(lexer, &token);
  while (token.type == Multiply || token.type == Divide) {
    consume_lexer_token(lexer, &token);

    AstNode *lhs = node;
    AstNode *rhs = new_factor(lexer);
    node = new_node(new_binary(lhs, rhs, &token),
      Binary, is_divergent(lhs) || is_divergent(rhs));

    peak_next_lexer_token(lexer, &token);
  }

  return node;
}

AstNode *new_expression(Lexer *lexer)
{
  AstNode *node = new_term(lexer);

  Token token;
  peak_next_lexer_token(lexer, &token);
  while (token.type == Add || token.type == Contains || token.type == Period) {
    consume_lexer_token(lexer, &token);

    AstNode *lhs = node;
    AstNode *rhs = new_term(lexer);
    node = new_node(new_binary(lhs, rhs, &token),
      Binary, is_divergent(lhs) || is_divergent(rhs));

    peak_next_lexer_token(lexer, &token);
  }

  return node;
}

AstNode *new_statement(Lexer *lexer)
{
  Token token;
  peak_next_lexer_token(lexer, &token);
  while (token.type == Newline) {
    consume_lexer_token(lexer, &token);
    peak_next_lexer_token(lexer, &token);
  }

  switch (token.type) {
    case Identifier: {
      Token ident = token;
      consume_lexer_token(lexer, &token);
      peak_next_lexer_token(lexer, &token);
      if (token.type == Assign) {
        consume_lexer_token(lexer, &token);
        AstNode *rhs = new_expression(lexer);
        return new_node(new_assignment(rhs, &ident),
          Declaration, is_divergent(rhs));
      }
    }
  }

  return new_expression(lexer);
}

AstNode *parse_next_line(Lexer *lexer)
{
  return new_statement(lexer);
}

void free_ast(AstNode *head)
{
  void *expression = head->expression;
  switch (head->type) {
    case Binary: {
      BinaryOp *binary = expression;
      free_ast(binary->lhs);
      free_ast(binary->rhs);
      tfree(binary);
      break;
    }
    case Unary: {
      UnaryOp *unary = expression;
      free_ast(unary->rhs);
      tfree(unary);
      break;
    }
    case NetworkRequest: {
      NetworkOp *network = expression;
      //for (int i = 0; i < network->buffer_size; i++)
        //free_string(network->responses[i]);
      free_ast(network->rhs);
      tfree(network->responses);
      tfree(network);
      break;
    }
    case Declaration: {
      Assignment *assign = expression;
      free_ast(assign->rhs);
      tfree(assign);
      break;
    }
    case Literal:
    case FieldIndex:
      free_value(expression);
      break;
  }

  tfree(head);
}
