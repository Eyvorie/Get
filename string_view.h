#ifndef STRING_VIEW_H
#define STRING_VIEW_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
  size_t length;
  const char *string;
} StringView;

void
string_view_from_literal(
  StringView *this, const char *string, size_t length);

bool
compare_string_views(StringView *this, StringView *other);

inline bool
compare_literal(StringView *this, const char *literal);

#endif