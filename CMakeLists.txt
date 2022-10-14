CMAKE_MINIMUM_REQUIRED(VERSION 3.16.3)
project(c_compiler CXX)

set(SRC
        src/main.cpp
        src/lexer.cpp)

add_executable(c_compiler ${SRC})