#include "value.h"
#include "alloc.h"
#include "util.h"

Value *new_null_value()
{
  Value *this = new(Value);
  this->type = NULL_VALUE;
  return this;
}

Value *new_integer_value(int value)
{
  Value *this = new(Value);
  this->type = INTEGER_VALUE;
  this->integer = value;
  return this;
}

Value *new_boolean_value(bool value)
{
  Value *this = new(Value);
  this->type = BooleanValue;
  this->boolean = value;
  return this;
}

Value *new_string_value(String *value)
{
  Value *this = new(Value);
  this->type = StringValue;
  this->string = value;
  return this;
}

Value *new_object_value(struct HashMap *value)
{
  Value *this = new(Value);
  this->type = ObjectValue;
  this->object = value;
  return this;
}

int as_integer(Value *this)
{
  assert(this->type == INTEGER_VALUE, "invalid cast!");
  return this->integer;
}

bool as_bool(Value *this)
{
  assert(this->type == BOOLEAN_VALUE, "invalid cast!");
  return this->boolean;
}

String *as_string(Value *this)
{
  assert(this->type == STRING_VALUE, "invalid cast!");
  return this->string;
}

struct HashMap *as_object(Value *this)
{
  assert(this->type == OBJECT_VALUE, "invalid cast!");
  return this->object;
}

void free_value(Value *this)
{
  switch (this->type) {
    case STRING_VALUE: {
      free_string(this->string);
      break;
    }
    case OBJECT_VALUE: {
      free_hashmap(this->object);
      break;
    }
  }

  tfree(this);
}
