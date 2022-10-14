#include <exception>
#include <string_view>

class CompilerException : std::exception {
public:
    explicit CompilerException(const char* msg);
    const char* what();
private:
    const char* m_message;
};