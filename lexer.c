#include <string.h>
#include <stdbool.h>
#include "lexer.h"
#include "string_view.h"

const char *alphanumeric_underscore =
  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_";

void
init_lexer(Lexer *this, const char *src)
{
  this->position = 0;
  this->src = src;
}

bool
token_from_match(Token *token, const char *token_str, const char *src)
{
  if (strstr(src, token_str) != src)
    return false;

  StringView *view = &token->value;
  view->string = src;
  view->length = strlen(token_str);
  return true;
}

bool
is_static_token(Lexer *this, Token *token)
{
  const char *src = this->src + this->position;
  if (token_from_match(token, "=", src)) {
    token->type = TOKEN_TYPE_ASSIGN;
    this->position += token->value.length;
    return true;
  }
  if (token_from_match(token, "get", src)) {
    token->type = TOKEN_TYPE_GET;
    this->position += token->value.length;
    return true;
  }
  if (token_from_match(token, "contains", src)) {
    token->type = TOKEN_TYPE_CONTAINS;
    this->position += token->value.length;
    return true;
  }
  if (token_from_match(token, ".", src)) {
    token->type = TOKEN_TYPE_PERIOD;
    this->position += token->value.length;
    return true;
  } 
  if (token_from_match(token, ",", src)) {
    token->type = TOKEN_TYPE_COMMA;
    this->position += token->value.length;
    return true;
  } 
  if (token_from_match(token, "\n", src)) {
    token->type = TOKEN_TYPE_NEWLINE;
    this->position += token->value.length;
    return true;
  } 
  if (token_from_match(token, "\r", src)) {
    token->type = TOKEN_TYPE_NEWLINE;
    this->position += token->value.length;
    return true;
  }

  return false;
}

bool
try_consume_character(Lexer *this, char c)
{
  bool matches = *(this->src + this->position) == c;
  this->position += matches;
  return matches;
}

bool
is_string(Lexer *this, Token *token)
{
  if (!try_consume_character(this, '"'))
    return false;

  size_t start_position = this->position;
  while (!try_consume_character(this, '"'))
    this->position++;

  token->type = TOKEN_TYPE_STRING;
  token->value.string = this->src + start_position;
  token->value.length = this->position - start_position;

  return true;
}

void
consume_whitespace(Lexer *this)
{
  while (try_consume_character(this, ' '));
}

bool
is_end_of_file(Lexer *this, Token *token)
{
  if (*(this->src + this->position) != '\0')
    return false;

  token->type = TOKEN_TYPE_EOF;
  return true;
}

void
is_identifier(Lexer *this, Token *token)
{
  const char *src = this->src + this->position;

  StringView identifier;
  size_t position = strspn(src, alphanumeric_underscore);
  string_view_from_literal(&identifier, src, position);
  token->type = TOKEN_TYPE_IDENTIFIER;
  token->value = identifier;
  this->position += position;
}

void
get_next_lexer_token(Lexer *this, Token *token)
{

  if (is_end_of_file(this, token))
    return;

  consume_whitespace(this);
  if (is_string(this, token))
    return;
  if (is_static_token(this, token))
    return;

  is_identifier(this, token);
}