#ifndef GET_ALLOC_H
#define GET_ALLOC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "util.h"

#define zero(dest, type) ((memset(dest, 0, sizeof(type))))
#define array_of(type, length) ((type *)err_or_malloc(sizeof(type) * length, #type))
#define new(type) ((type *)err_or_calloc(1, sizeof(type), #type))

extern int allocs;

inline void *err_or_malloc(size_t size, const char *type)
{
  void *block = malloc(size);
  assert(block != NULL, "out of memory error, could not create new array of %s\n", type);
#ifdef ALLOC_VERBOSE
  printf("allocating new %s at address 0x%p\n", type, block);
#endif
  allocs++;
  return block;
}

inline void *err_or_calloc(size_t elements, size_t size, const char *type)
{
  void *block = calloc(elements, size);
  assert(block != NULL, "out of memory error, could not create new %s\n", type);
#ifdef ALLOC_VERBOSE
  printf("allocating new %s at address 0x%p\n", type, block);
#endif
  allocs++;
  return block;
}

inline void tfree(void *block)
{
  allocs--;
#ifdef ALLOC_VERBOSE
  printf("freeing 0x%p\n", block);
#endif
  free(block);
}

inline void print_allocation_count()
{
#ifdef ALLOC_VERBOSE
  printf("unfree-d allocations: %d\n", allocs);
#endif
}

#endif