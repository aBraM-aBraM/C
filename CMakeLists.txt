CMAKE_MINIMUM_REQUIRED(VERSION 3.16.3)
project(c_compiler CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-std=c++2a)

set(SRC
        src/exceptions.cpp
        src/token.cpp
        src/lexer.cpp
        )

add_library(c_compiler_lib ${SRC})

add_subdirectory(libs)
add_subdirectory(tests)

add_executable(c_compiler src/main.cpp)
target_link_libraries(c_compiler PUBLIC c_compiler_lib)