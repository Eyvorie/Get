#include <string.h>
#include "strings.h"
#include "alloc.h"

size_t string_len(String *this)
{
  return this->length;
}

void init_string_view(StringView *this, char *str, size_t length)
{
  this->length = length;
  this->characters = str;
}

String *new_string_known_len(char *literal, size_t length)
{
  String *string = new(String);
  string->length = length;
  string->characters = array_of(char, length);
  memcpy(string->characters, literal, length);
  return string;
}

String *new_string(char *literal)
{
  return new_string_known_len(literal, strlen(literal));
}

String *new_string_from_view(StringView *this)
{
  return new_string_known_len(this->characters, this->length);
}

bool compare_strings(String *this, String *other)
{
  if (this->length != other->length)
    return false;
  return strncmp(this->characters, other->characters, this->length) == 0;
}

char *string_get_characters(String *this)
{
  char *string = array_of(char, this->length + 1);
  memcpy(string, this->characters, this->length);
  string[this->length] = 0;
  return string;
}

String *concat_strings(String *this, String *other)
{
  String *string = new(String);
  string->length = this->length + other->length;
  string->characters = array_of(char, string->length);
  memcpy(string->characters, this->characters, this->length);
  memcpy(string->characters + this->length, other->characters, other->length);
  return string;
}

void free_string(String *this)
{
  tfree(this->characters);
  tfree(this);
}
