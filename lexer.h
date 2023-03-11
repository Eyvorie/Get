#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include "string_view.h"

typedef enum {
  TOKEN_TYPE_IDENTIFIER,
  TOKEN_TYPE_ASSIGN,
  TOKEN_TYPE_GET,
  TOKEN_TYPE_CONTAINS,
  TOKEN_TYPE_PERIOD,
  TOKEN_TYPE_COMMA,
  TOKEN_TYPE_NEWLINE,
  TOKEN_TYPE_STRING,
  TOKEN_TYPE_NUMBER,
  TOKEN_TYPE_EOF,
  NUMBER_OF_TOKENS
} TokenType;

typedef struct {
  StringView value;
  TokenType type;
} Token;

typedef struct {
  size_t position;
  const char *src;
} Lexer;

void
init_lexer(Lexer *this, const char *src);

void
get_next_lexer_token(Lexer *this, Token *token);

#endif