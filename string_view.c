#include <string.h>
#include "string_view.h"

void
string_view_from_literal(
  StringView *this, const char *string, size_t length)
{
  this->string = string;
  this->length = length;
}

bool
compare_string_views(StringView *this, StringView *other)
{
  if (this->length != other->length)
    return false;
  return strncmp(this->string, other->string, this->length) == 0;
}

inline bool
compare_literal(StringView *this, const char *literal)
{
  return strncmp(this->string, literal, this->length) == 0;
}