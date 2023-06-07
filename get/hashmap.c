#include <string.h>
#include "hashmap.h"
#include "alloc.h"

// TODO: dynamically increase/decrease buckets based on load factor
#define BUCKETS 100

// fnv 1a hash
// http://www.isthe.com/chongo/tech/comp/fnv/#public_domain
size_t hash(String *key)
{
  unsigned char *p = key->characters;
  unsigned long long h = 0xcbf29ce484222325ULL;
  int i;

  for (i = 0; i < key->length; i++)
    h = (h ^ p[i]) * 0x100000001b3ULL;

  return h;
}

HashEntry *new_entry(String *key, Value *data)
{
  HashEntry *new = new(HashEntry);
  new->key = key;
  new->data = data;
  return new;
}

HashMap *new_hashmap()
{
  HashMap *map = new(HashMap);
  map->buckets = BUCKETS;
  map->entries = array_of(HashEntry *, BUCKETS);
  memset(map->entries, 0, BUCKETS * sizeof(HashEntry *));
  return map;
}

void put_hashmap_entry(HashMap *this, String *key, Value *data)
{
  HashEntry *new = new_entry(key, data);
  size_t bucket = hash(key) % this->buckets;
  new->next = this->entries[bucket];
  this->entries[bucket] = new;
}

HashEntry *get(HashMap *this, String *key)
{
  HashEntry *entry = this->entries[hash(key) % this->buckets];
  if (!entry)
    return NULL;

  while (entry->next) {
    if (compare_strings(key, entry->key))
      break;
    entry = entry->next;
  }

  return entry;
}

Value *get_hashmap_entry(HashMap *this, String *key)
{
  HashEntry *entry = get(this, key);
  if (entry)
    return entry->data;

  return NULL;
}

void free_hashentry(HashEntry *entry)
{
  free_value(entry->data);
  free_string(entry->key);
  tfree(entry);
}

void free_hashmap(HashMap *this)
{
  for (int i = 0; i < BUCKETS; i++) {
    HashEntry *entry = this->entries[i];
    while (entry) {
      HashEntry *temp = entry;
      entry = entry->next;
      free_hashentry(temp);
    }
  }

  tfree(this->entries);
  tfree(this);
}