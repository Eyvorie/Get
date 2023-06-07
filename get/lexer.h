#ifndef GET_LEXER_H
#define GET_LEXER_H

#include "strings.h"
#include "hashmap.h"

typedef enum {

  // variable
  LexerTokenIdentifier,
  LexerTokenFunction,

  // keywords
  LexerTokenIf,
  LexerTokenElse,
  LexerTokenFor,
  LexerTokenWhile,
  LexerTokenGet,
  LexerTokenPost,
  LexerTokenPut,
  LexerTokenDelete,
  LexerTokenPrint,
  
  // operators
  LexerTokenAdd,
  LexerTokenSubtract,
  LexerTokenMultiply,
  LexerTokenDivide,
  LexerTokenAssign,
  LexerTokenEquals,
  LexerTokenPeriod,
  LexerTokenComma,
  LexerTokenLParen,
  LexerTokenRParen,
  LexerTokenLessThan,
  LexerTokenGreaterThan,

  // literals
  LexerTokenTrue,
  LexerTokenFalse,
  LexerTokenString,
  LexerTokenInt,

  // control characters
  LexerTokenNewline,
  LexerTokenEOF

} LexerTokenType;

typedef struct {
  LexerTokenType type;
  StringView value;
} LexerToken;

typedef struct {
  char *src;
  HashMap *keywords;
} Lexer;

void init_lexer(Lexer *this, char *src);
void peak_next_lexer_token(Lexer *this, LexerToken *token);
void get_next_lexer_token(Lexer *this, LexerToken *token);
void consume_lexer_token(Lexer *this, LexerToken *token);
void cleanup_lexer(Lexer *this);

#endif