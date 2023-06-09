#include "value.h"
#include "alloc.h"
#include "util.h"

Value *shallow_copy_value(Value *source)
{
  Value *this = new(Value);
  this->type = source->type;
  switch (this->type) {
    case NullValue:
      return this;
    case IntegerValue:
      this->integer = source->integer;
      return this;
    case BooleanValue:
      this->boolean = source->boolean;
      return this;
    case StringValue:
      this->string = source->string;
      return this;
    case ObjectValue:
      this->object = source->object;
      return this;
  }
  assert(false, "Failed to copy value");
  return NULL;
}

Value *new_null_value()
{
  Value *this = new(Value);
  this->type = NullValue;
  return this;
}

Value *new_integer_value(int value)
{
  Value *this = new(Value);
  this->type = IntegerValue;
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

void init_null_value(Value *this)
{
  this->type = NullValue;
}

void init_integer_value(Value *this, int value)
{
  this->type = IntegerValue;
  this->integer = value;
}

void init_boolean_value(Value *this, bool value)
{
  this->type = BooleanValue;
  this->boolean = value;
}

void init_string_value(Value *this, String *value)
{
  this->type = StringValue;
  this->string = value;
}

void init_object_value(Value *this, struct HashMap *value)
{
  this->type = ObjectValue;
  this->object = value;
}

int as_integer(Value *this)
{
  assert(this->type == IntegerValue, "invalid cast!");
  return this->integer;
}

bool as_bool(Value *this)
{
  assert(this->type == BooleanValue, "invalid cast!");
  return this->boolean;
}

String *as_string(Value *this)
{
  assert(this->type == StringValue, "invalid cast!");
  return this->string;
}

struct HashMap *as_object(Value *this)
{
  assert(this->type == ObjectValue, "invalid cast!");
  return this->object;
}

void free_value(Value *this)
{
  switch (this->type) {
    case StringValue: {
      free_string(this->string);
      break;
    }
    case ObjectValue: {
      free_hashmap(this->object);
      break;
    }
  }

  tfree(this);
}
