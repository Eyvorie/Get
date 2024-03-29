#ifndef GET_VALUE_H
#define GET_VALUE_H

#include <stdio.h>
#include <stdbool.h>
#include "strings.h"

typedef enum {
  NullValue,
  IntegerValue,
  BooleanValue,
  StringValue,
  ObjectValue
} ValueType;

typedef struct {
  ValueType type;
  union {
    int integer;
    bool boolean;
    String *string;
    struct HashMap *object;
  };
} Value;

Value *shallow_copy_value(Value *source);
Value *new_null_value();
Value *new_integer_value(int value);
Value *new_boolean_value(bool value);
Value *new_string_value(String *value);
Value *new_object_value(struct HashMap *value);

void init_null_value(Value *this);
void init_integer_value(Value *this, int value);
void init_boolean_value(Value *this, bool value);
void init_string_value(Value *this, String *value);
void init_object_value(Value *this, struct HashMap *value);

int as_integer(Value *this);
bool as_bool(Value *this);
String *as_string(Value *this);
struct HashMap *as_object(Value *this);

Value *null_to_string(Value *this);
Value *integer_to_string(Value *this);
Value *boolean_to_string(Value *this);
Value *coerce_to(Value *this, ValueType type);

void print_value(FILE *outstream, Value *data);
void free_value(Value *this);

#endif
