#ifndef GET_LEXER_H
#define GET_LEXER_H
#include "get/strings.h"

typedef enum {

  LexerTokenIllegal,

  // variable
  LexerTokenLet,
  LexerTokenIdentifier,
  LexerTokenFunction,

  LexerTokenIf,
  LexerTokenElse,
  
  // operators
  LexerTokenAdd,
  LexerTokenSubtract,
  LexerTokenMultiply,
  LexerTokenDivide,
  LexerTokenAssign,
  LexerTokenEquals,
  LexerTokenComma,
  LexerTokenLParen,
  LexerTokenRParen,
  LexerTokenLessThan,
  LexerTokenGreaterThan,

  // literals
  LexerTokenTrue,
  LexerTokenFalse,
  LexerTokenString,

  LexerTokenEOF

} LexerTokenType;

typedef struct {
  LexerTokenType type;
  StringView value;
} LexerToken;

typedef struct {
  int row;
  int column;
  const char *src;
} Lexer;

void init_lexer(Lexer *this, const char *src);
void get_next_lexer_token(Lexer *this, LexerToken *token);

#endif