#ifndef GET_STRINGS_H
#define GET_STRINGS_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
  size_t length;
  char *characters;
} String;
typedef String StringView;

void init_string_view(StringView *this, char *str, size_t length);

String *new_string(char *literal);
String *new_string_known_len(char *literal, size_t length);
String *new_string_from_view(StringView *this);

size_t string_len(String *this);
bool compare_strings(String *this, String *other);

char *string_get_characters(String *this);

void free_string(String *this);

#endif
