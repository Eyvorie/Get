#ifndef GET_LEXER_H
#define GET_LEXER_H

#include "strings.h"
#include "hashmap.h"

typedef enum {

  // variable
  Identifier,
  Function,

  // keywords
  If,
  Else,
  For,
  While,
  Get,
  Post,
  Put,
  Delete,
  Print,
  Contains,
  
  // operators
  Add,
  Subtract,
  Multiply,
  Divide,
  Assign,
  Equals,
  Period,
  Comma,
  LeftParen,
  RightParen,
  LessThan,
  GreaterThan,
  DollarSign,

  // literals
  TrueLiteral,
  FalseLiteral,
  StringLiteral,
  IntegerLiteral,

  // control characters
  Newline,
  EndOfFile

} LexerTokenType;

typedef struct {
  LexerTokenType type;
  StringView value;
} Token;

typedef struct {
  char *src;
  HashMap *keywords;
} Lexer;

void init_lexer(Lexer *this, char *src);
void peak_next_lexer_token(Lexer *this, Token *token);
void get_next_lexer_token(Lexer *this, Token *token);
void consume_lexer_token(Lexer *this, Token *token);
void free_lexer(Lexer *this);

#endif