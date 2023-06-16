#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "alloc.h"
#include "value.h"

void evaluate(Interpreter *this, AstNode *current, Value *ret_value);

void resolve_unary(Interpreter *this, UnaryOp *unary, Value *ret_value)
{
  Value rhs;
  evaluate(this, unary->rhs, &rhs);

  switch (unary->op) {
    case Print:
      print_value(this->out, &rhs);
      init_null_value(ret_value);
      break;
  }
}

void resolve_binary(Interpreter *this, BinaryOp *binary, Value *ret_value)
{
  Value lhs, rhs;
  evaluate(this, binary->lhs, &lhs);
  evaluate(this, binary->rhs, &rhs);

  switch (binary->op) {
    case Add: {
      switch (lhs.type) {
        case StringValue:
          init_string_value(ret_value,
            concat_strings(as_string(&lhs), as_string(coerce_to(&rhs, StringValue))));
          break;
        case IntegerValue:
          init_integer_value(ret_value,
            as_integer(&lhs) + as_integer(&rhs));
          break;
        case BooleanValue:
          init_integer_value(ret_value,
            as_integer(&lhs) + as_integer(&rhs));
          break;
      }
      break;
    }
    case Contains: {
      assert(lhs.type == StringValue && rhs.type == StringValue,
        "Interpreter error: contains can only be called for strings!");
      char *string = string_get_characters(as_string(&lhs));
      char *substring = string_get_characters(as_string(&rhs));
      init_boolean_value(ret_value,
        strstr(string, substring) != NULL);
      tfree(string);
      tfree(substring);
      break;
    }
  }
}

size_t write_callback(void *data, size_t size,
  size_t nmemb, void *userp)
{
  String *response = userp;
  size_t content_length = size * nmemb;
  size_t len = string_len(response);

  response->characters = (char *)realloc(response->characters,
    len + content_length);
  assert(response->characters != NULL,
    "Interpreter error: failed to allocate memory for curl response");

  memcpy(response->characters + len, 
    data, content_length);
  response->length += content_length;

  return content_length;
}

size_t header_callback(char *buffer, size_t size,
  size_t nitems, void *userp)
{
  return size * nitems;
}

void add_transfer(Interpreter *this, NetworkOp *network)
{
  CURL *curl_easy_handle = curl_easy_init();
  network->responses[this->line] = new_string_known_len("", 0);
  curl_easy_setopt(curl_easy_handle, CURLOPT_WRITEFUNCTION, &write_callback);
  curl_easy_setopt(curl_easy_handle, CURLOPT_HEADERFUNCTION, &header_callback);
  curl_easy_setopt(curl_easy_handle, CURLOPT_WRITEDATA, network->responses[this->line]);

  struct curl_slist *headers = NULL;
  curl_easy_setopt(curl_easy_handle, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl_easy_handle, CURLOPT_HEADERDATA, this);

  Value resolved_url;
  evaluate(this, network->rhs, &resolved_url);
  char *raw_url = string_get_characters(
    as_string(&resolved_url));
  curl_easy_setopt(curl_easy_handle, CURLOPT_URL, raw_url);
  curl_multi_add_handle(this->curl_multi_handle, curl_easy_handle);
  tfree(raw_url);
}

void resolve_request(Interpreter *this, NetworkOp *network,
  Value *ret_value)
{
  if (this->line == 0) {
    network->buffer_size = this->total_lines;
    network->responses = array_of(String *, this->total_lines);

    for (; this->line < this->concurrent_transfer_limit && this->line < this->total_lines; this->line++)
      add_transfer(this, network);

    int inflight = this->line;
    int messages_remaining = 0;
    CURLMsg *curl_message;
    do {
      int queue_has_requests = 1;
      curl_multi_perform(this->curl_multi_handle,
        &queue_has_requests);
      while ((curl_message = curl_multi_info_read(this->curl_multi_handle, &messages_remaining))) {
        if (curl_message->msg == CURLMSG_DONE) {
          CURL *curl_easy_handle = curl_message->easy_handle;
          curl_multi_remove_handle(this->curl_multi_handle,
            curl_easy_handle);
          curl_easy_cleanup(curl_easy_handle);
          inflight--;
        }
        if (this->line < this->total_lines) {
          add_transfer(this, network);
          this->line++;
          inflight++;
        }
      }
      if (inflight)
        curl_multi_wait(this->curl_multi_handle, NULL, 0, 1000, NULL);
    } while (inflight);
    this->line = 0;
  }

  init_string_value(ret_value, network->responses[this->line]);
}

// if the expression does not diverge then we collapse that
// ast node into a literal by free-ing the expression and replacing it 
// with the literal value so that it does not have to be re-processed
void try_collapse_expression(AstNode *op, Value *literal)
{
  if (op->diverges)
    return;
  
  tfree(op->expression);
  op->type = Literal;
  op->expression = shallow_copy_value(literal);
}

void evaluate(Interpreter *this, AstNode *current,
  Value *ret_value)
{
  switch(current->type) {
    case Literal:
      *ret_value = *((Value *)current->expression);
      return;
    case Unary:
      resolve_unary(this,
        (UnaryOp *)current->expression, ret_value);
      try_collapse_expression(current, ret_value);
      return;
    case Binary:
      resolve_binary(this,
        (BinaryOp *)current->expression, ret_value);
      try_collapse_expression(current, ret_value);
      return;
    case NetworkRequest:
      resolve_request(this,
        (NetworkOp *)current->expression, ret_value);
      return;
    case FieldIndex: {
      int index = as_integer(current->expression);
      char *s = this->input + this->line_table[this->line];

      int count = 0;
      for (; *s != '\0' && *s != '\r' 
        && *s != '\n' && count < index; s++) {
        if (*s == ' ')
          count++;
      }

      char *e = s;
      while (*e != ' ' && *e != '\r' && *e != '\n' && *e != '\0')
        e++;
      
      init_string_value(ret_value,
        new_string_known_len(s, e - s));
      return;
    }
    case Declaration: {
      Assignment *var = current->expression;
      Value *data = new(Value);
      evaluate(this, var->rhs, data);
      char buffer[64];
      int len = sprintf(buffer, "__%d_%.*s", this->line,
        string_len(&var->identifier), var->identifier.characters);
      String *mangled_ident = new_string_known_len(buffer, len);
      put_hashmap_entry(this->memory,
        mangled_ident, data);
      return;
    }
    case VariableCall: {
      Variable *var = current->expression;
      char buffer[64];
      int len = sprintf(buffer, "__%d_%.*s", this->line,
        string_len(&var->identifier), var->identifier.characters);
      String *mangled_ident = new_string_known_len(buffer, len);
      Value *data = get_hashmap_entry(this->memory,
        mangled_ident);
      assert(data != NULL, "Interpreter error: %.*s is not defined", 
        string_len(&var->identifier), var->identifier.characters);
      *ret_value = *data;
      free_string(mangled_ident);
      return;
    }
  }
}

bool read_file_into_memory(char *path,
  char **buffer)
{
  FILE *file = fopen(path, "rb");
  if (file == NULL)
    return false;

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);

  *buffer = array_of(char, size + 1);
  size_t read = fread(*buffer, size, 1, file);
  if (read == 0)
    return false;
  
  (*buffer)[size] = 0;
  fclose(file);
  return true;
}

int interpret(Interpreter *this,
  int argc, char *argv[])
{
  zero(this, Interpreter);

  curl_global_init(CURL_GLOBAL_ALL);
  this->curl_multi_handle = curl_multi_init();
  if (!this->curl_multi_handle) {
    printf("Interpreter error: failed to initialize curl\n");
    curl_global_cleanup();
    return EXIT_FAILURE;
  }
  curl_multi_setopt(this->curl_multi_handle, 
    CURLOPT_MAXCONNECTS, 256L);

  bool source_file = false;
  this->src = argv[1];
  this->out = stdout;
  this->memory = new_hashmap();
  this->concurrent_transfer_limit = 256;
  for (int i = 0; i < argc; i++) {
    if (*argv[i] != '-')
      continue;
    char *value = argv[i + 1];
    switch (*(argv[i] + 1)) {
      case 'o': {
        this->out = fopen(value, "ab+");
        assert(this->out != NULL,
          "Interpreter error: failed to create output file, %s", value);
        break;
      }
      case 's': {
        int success = read_file_into_memory(value,
          &this->src);
        assert(success,
          "Interpreter error: failed to read source file, %s", value);
        source_file = true;
        break;
      }
      default: {
        assert(false, "Interpreter error: unknown param");
      }
    }
  }

  int success = read_file_into_memory(argv[argc - 1],
    &this->input);
  if (!success)
    assert(false, "Interpreter error: failed to open input file, %s",
      argv[argc - 1]);
  
  int line_table_size = 1;
  this->line_table = array_of(int, line_table_size);
  this->line_table[this->total_lines++] = 0;

  char *s = this->input;
  char *n = this->input;
  while ((n = strchr(s, '\n'))) {
    n++;
    if (line_table_size <= this->total_lines) {
      line_table_size = fmax(line_table_size * 2, line_table_size + 1);
      this->line_table = (int *)realloc(this->line_table,
        line_table_size * sizeof(int));
      assert(this->line_table != NULL,
        "Interpreter error: failed to generate line table");
    }
    this->line_table[this->total_lines++] = n - this->input;
    s = n;
  }

  Lexer lexer;
  init_lexer(&lexer, this->src);

  Token token;
  peak_next_lexer_token(&lexer, &token);
  while (token.type != EndOfFile) {
    AstNode *ast = parse_next_line(&lexer);
    for (; this->line < this->total_lines; this->line++) {
      Value result;
      evaluate(this, ast, &result);
    }
    this->line = 0;
    free_ast(ast);
    peak_next_lexer_token(&lexer, &token);
  }

  if (source_file)
    tfree(this->input);
  tfree(this->line_table);
  free_hashmap(this->memory);
  free_lexer(&lexer);
  curl_multi_cleanup(this->curl_multi_handle);
  curl_global_cleanup();

  print_allocation_count();
  return EXIT_SUCCESS;
}
