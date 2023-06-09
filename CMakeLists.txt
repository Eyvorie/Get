cmake_minimum_required(VERSION 3.23.0)
project(Get)

add_executable(${PROJECT_NAME} main.c
 get/interpreter.c get/interpreter.h
 get/parser.c get/parser.h
 get/lexer.c get/lexer.h
 get/strings.c get/strings.h
 get/hashmap.c get/hashmap.h
 get/value.c get/value.h
 get/alloc.c get/alloc.h
 get/util.h)
  
target_include_directories(${PROJECT_NAME} PRIVATE get)
target_include_directories(${PROJECT_NAME} PRIVATE external/curl/include)
target_link_directories(${PROJECT_NAME} PUBLIC external/curl/lib)

if (MSVC) 
  target_link_libraries(${PROJECT_NAME}
    Normaliz Crypt32 Ws2_32 Wldap32 libcurl_a)
else()
  find_package(CURL_REQUIRED)
  target_link_libraries(${PROJECT_NAME} CURL::libcurl)
endif()
