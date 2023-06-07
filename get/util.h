#ifndef GET_UTIL_H
#define GET_UTIL_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

inline void assert(int condition, const char *format, ...)
{
  if (condition) return;
  va_list argp;
  va_start(argp, format);
  vprintf(format, argp);
  exit(EXIT_FAILURE);
}

#endif