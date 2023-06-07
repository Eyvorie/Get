#include <stdbool.h>
#include "strings.h"
#include "lexer.h"
#include "value.h"

typedef struct AstNode AstNode;

typedef struct {
  LexerTokenType op;
  AstNode *rhs;
  String **responses;
} NetworkOp;

typedef struct {
  LexerTokenType op;
  AstNode *rhs;
} UnaryOp;

typedef struct {
  LexerTokenType op;
  AstNode *lhs;
  AstNode *rhs;
} BinaryOp;

typedef struct {
  StringView identifier;
} Variable;

typedef struct {
  StringView identifier;
  AstNode *rhs;
} Assignment;

typedef enum {
  Binary,
  Unary,
  NetworkRequest,
  Declaration,
  Literal,
  VariableCall,
  FieldIndex
} NodeType;

struct AstNode {
  bool diverges;
  NodeType type;
  void *expression;
};

AstNode *parse_next_line(Lexer *lexer);