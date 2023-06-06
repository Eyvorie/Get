#include "get/lexer.h"

void init_lexer(Lexer *this, const char *src)
{
  this->row = 0;
  this->column = 0;
  this->src = src;
}

void consume_whitespace(Lexer *this)
{
  while (this->src == ' ')
    this->src++;
}

void init_token(LexerToken *token, LexerTokenType type)
{
  token->type = type;
}

void get_next_lexer_token(Lexer *this, 
  LexerToken *token)
{
  consume_whitespace(this);

  switch (*this->src) {
    case '+':
      init_token(token, LexerTokenAdd);
      break;
    case '-':
      init_token(token, LexerTokenSubtract);
      break;
    case '*':
      init_token(token, LexerTokenMultiply);
      break;
    case '/':
      init_token(token, LexerTokenDivide);
      break;
    case '=':
      init_token(token, LexerTokenAssign);
      break;
    case ',':
      init_token(token, LexerTokenComma);
      break;
    case '(':
      init_token(token, LexerTokenLParen);
      break;
    case ')':
      init_token(token, LexerTokenRParen);
      break;
    case '<':
      init_token(token, LexerTokenLessThan);
      break;
    case '>':
      init_token(token, LexerTokenGreaterThan);
      break;
  }

  init_token(token, LexerTokenIllegal);
}