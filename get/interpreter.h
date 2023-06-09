#ifndef GET_INTERPRETER_H
#define GET_INTERPRETER_H

#define CURL_STATICLIB
#include <stdio.h>
#include "curl/curl.h"
#include "hashmap.h"

typedef struct {
  int line;
  int total_lines;
  int concurrent_transfer_limit;
  char *src;
  char *input;
  int *line_table;
  FILE *out;
  CURLM *curl_multi_handle;
  HashMap *memory;
} Interpreter;

int interpret(Interpreter *this, int argc, char *argv[]);

#endif