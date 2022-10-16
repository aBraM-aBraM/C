#include "token.h"


Token::Token(const TOKEN_TYPE &token_type, const std::variant<int, std::string, char>& value) :
        m_type(token_type),
        m_value(value) {

}

std::string Token::to_string() const {
    switch(m_value.index()){
        case 0: // int
            return std::to_string(std::get<int>(m_value));
        case 1: // string
            return std::get<std::string>(m_value);
        default: // char
            return std::string(1, std::get<char>(m_value)); // NOLINT(modernize-return-braced-init-list)
    }
}

bool Token::operator==(const Token &other) const {
    return m_value == other.m_value && m_type == other.m_type;
}
