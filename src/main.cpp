#include <iostream>
#include <memory>
#include "lexer.h"
#include "macros.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Unsupported syntax!" << std::endl << "Supported Syntax: ./c_compiler <code.c>" << std::endl;
        return 1;
    }

    DEBUG_MSG("Compiling " << argv[1]);
    std::ifstream input_file(argv[1]);

    Lexer lexer;
    auto tokens = lexer.lex(input_file);

    return 0;
}