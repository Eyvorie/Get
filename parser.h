#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef union Expression Expression;

typedef struct {
  const char* identifier;
  Expression *lhs;
} AssignmentExpression;

union Expression {
  AssignmentExpression assignment;
};

void parse(Lexer *lexer);

#endif