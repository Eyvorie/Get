cmake_minimum_required(VERSION 3.23.0)
project(CanvasSearchVisualScript)

add_executable(${PROJECT_NAME} engine.c 
  lexer.c lexer.h
  string_view.c string_view.h
  parser.c parser.h)