#include "exceptions.h"

CompilerException::CompilerException(const char* msg): m_message(msg) {}

const char *CompilerException::what() {
    return m_message;
}
