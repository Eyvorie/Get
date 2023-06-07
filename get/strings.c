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

bool compare_strings(String *this, String *other)
{
  if (this->length != other->length)
    return false;
  return strncmp(this->characters, this->characters, this->length) == 0;
}

void free_string(String *this)
{
  tfree(this->characters);
  tfree(this);
}