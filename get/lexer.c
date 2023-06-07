#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lexer.h"
#include "value.h"
#include "util.h"

void init_lexer(Lexer *this, char *src)
{
  this->src = src;
  // map keywords
  HashMap *keywords = new_hashmap();
  put_hashmap_entry(keywords,
    new_string("if"), new_integer_value(If));
  put_hashmap_entry(keywords,
    new_string("else"), new_integer_value(Else));
  put_hashmap_entry(keywords,
    new_string("for"), new_integer_value(For));
  put_hashmap_entry(keywords,
    new_string("while"), new_integer_value(While));
  put_hashmap_entry(keywords,
    new_string("get"), new_integer_value(Get));
  put_hashmap_entry(keywords,
    new_string("post"), new_integer_value(Post));
  put_hashmap_entry(keywords,
    new_string("put"), new_integer_value(Put));
  put_hashmap_entry(keywords,
    new_string("delete"), new_integer_value(Delete));
  put_hashmap_entry(keywords,
    new_string("print"), new_integer_value(Print));
  this->keywords = keywords;

  assert(keywords != NULL, "Lexer error: failed to initialize the lexer");
}

inline void init_token(Lexer *this, LexerToken *token, LexerTokenType type, int value_length)
{
  token->type = type;
  init_string_view(&token->value, this->src, value_length);
}

bool is_number(char c)
{
  return (c >= '0' && c <= '9') || c == '-';
}

bool is_alphanumeric(char c)
{
  return (c >= 0 && c <= 9) || (c >= 'a' && c <= 'z') ||
    (c >= 'A' && c <= 'Z') || c == '_';
}

void peak_next_lexer_token(Lexer *this, 
  LexerToken *token)
{
  while (*this->src == ' ')
    this->src++;
  char c = *this->src;

  switch (c) {
    case '+':
      init_token(this, token, Add, 1);
      return;
    case '-':
      init_token(this, token, Subtract, 1);
      return;
    case '*':
      init_token(this, token, Multiply, 1);
      return;
    case '/':
      init_token(this, token, Divide, 1);
      return;
    case '=':
      init_token(this, token, Assign, 1);
      return;
    case ',':
      init_token(this, token, Comma, 1);
      return;
    case '(':
      init_token(this, token, LeftParen, 1);
      return;
    case ')':
      init_token(this, token, RightParen, 1);
      return;
    case '<':
      init_token(this, token, LessThan, 1);
      return;
    case '>':
      init_token(this, token, GreaterThan, 1);
      return;
    case '\0':
      init_token(this, token, EndOfFile, 1);
      return;
    case '\n':
      init_token(this, token, Newline, 1);
      return;
    case '\r':
      init_token(this, token, Newline, 2);
      return;
    case '"': {
      char *end_quote = strchr(this->src + 1, '"');
      assert(end_quote != NULL, "Lexer error: missing end quote");
      init_token(this, token, StringLiteral, end_quote - this->src + 1);
      return;
    }
  }

  if (is_number(c)) {
    char *number = this->src;
    while (is_number(*number))
      number++;
    init_token(this, token, IntegerLiteral, number - this->src);
    return;
  }

  if (is_alphanumeric(c)) {
    char *ident = this->src;
    while (is_alphanumeric(*ident))
      ident++;
    size_t len = ident - this->src;
    String *key = new_string_known_len(this->src, len);
    Value *keyword = get_hashmap_entry(this->keywords, key);
    if (keyword != NULL) {
      init_token(this, token, as_integer(keyword), string_len(key));
      free_string(key);
      return;
    }
    init_token(this, token, Identifier, len);
    free_string(key);
    return;
  }

  assert(false, "Lexer error: illegal token");
}

void consume_lexer_token(Lexer *this, LexerToken *token)
{
  this->src += string_len(&token->value);
}

void get_next_lexer_token(Lexer *this, LexerToken *token)
{
  peak_next_lexer_token(this, token);
  consume_lexer_token(this, token);
}

void cleanup_lexer(Lexer *this)
{
  free_hashmap(this->keywords);
}