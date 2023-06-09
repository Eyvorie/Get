#include "interpreter.h"

int main(int argc, char *argv[])
{
  Interpreter interpreter;
  return interpret(&interpreter, argc, argv);
}