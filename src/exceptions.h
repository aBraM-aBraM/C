#include <exception>
#include <string>


class CompilerException : std::exception {
public:
    explicit CompilerException(const char* msg);
    const char* what();
private:
    const std::string m_message;
};