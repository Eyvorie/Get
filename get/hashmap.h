#ifndef GET_HASHMAP_H
#define GET_HASHMAP_H

#include <stdlib.h>
#include <stdbool.h>
#include "strings.h"
#include "value.h"

typedef struct HashEntry HashEntry;
struct HashEntry {
  String *key;
  Value *data;
  HashEntry *next;
};

typedef struct {
  size_t buckets;
  HashEntry **entries;
} HashMap;

HashMap *new_hashmap();
void put_hashmap_entry(HashMap *this, String *key, Value *data);
void remove_hashmap_entry(HashMap *this, String *key);
Value *get_hashmap_entry(HashMap *this, String *key);
void free_hashmap(HashMap *this);

#endif