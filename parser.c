#include "lexer.h"
#include "parser.h"

void
parse_expression(Lexer *lexer)
{

}

void
parse_assignment_statement(Lexer *lexer)
{

}

void
parse(Lexer *lexer)
{
  Token token;
  get_next_lexer_token(lexer, &token);
  while (token.type == TOKEN_TYPE_EOF) {
  }
}