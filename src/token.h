#pragma once
#include <variant>
#include <map>
#include <vector>
#include <string>

enum class TOKEN_TYPE {
    IF,
    EQ,
    OR,
    FOR,
    INT,
    GEQ,
    LEQ,
    AND,
    AMP,
    MOD,
    BREAK,
    CONTINUE,
    ADD,
    SUB,
    DIV,
    LOR,
    LAND,
    DOT,
    NEQ,
    LESS,
    PIPE,
    BANG,
    STAR,
    ELSE,
    FUNC_CALL,
    CHAR,
    EMPTY,
    WHILE,
    GREAT,
    COMMA,
    COLON,
    INPUT,
    PRINT,
    ASSIGN,
    ADDRESSOF,
    DEREF,
    RBRACE,
    LBRACE,
    STRING,
    RETURN,
    INTEGER,
    LPARENS,
    RPARENS,
    LBRACKET,
    RBRACKET,
    SEMICOLON,
    CHARACTER,
    IDENTIFIER,
    VOID
};

const std::map<std::string, TOKEN_TYPE> KEYWORDS = {

        {"if",       TOKEN_TYPE::IF},
        {"else",     TOKEN_TYPE::ELSE},
        {"input",    TOKEN_TYPE::INPUT},
        {"while",    TOKEN_TYPE::WHILE},
        {"print",    TOKEN_TYPE::PRINT},
        {"return",   TOKEN_TYPE::RETURN},
        {"break",    TOKEN_TYPE::BREAK},
        {"continue", TOKEN_TYPE::CONTINUE},

        // Types
        {"int",      TOKEN_TYPE::INT},
        {"char",     TOKEN_TYPE::CHAR},
        {"void",     TOKEN_TYPE::VOID},
};

const std::vector<TOKEN_TYPE> TYPES = {
        TOKEN_TYPE::INT,
        TOKEN_TYPE::CHAR,
        TOKEN_TYPE::VOID,
};

constexpr int COMPOUND_OPERATOR_SIZE = 2;
const std::map<std::string, TOKEN_TYPE> COMPOUND_OPERATORS = {

        // Dual Character Operators
        {"==", TOKEN_TYPE::EQ},
        {"!=", TOKEN_TYPE::NEQ},
        {"<=", TOKEN_TYPE::LEQ},
        {">=", TOKEN_TYPE::GEQ},
        {"&&", TOKEN_TYPE::LAND},
        {"||", TOKEN_TYPE::LOR},
};

constexpr int SINGLE_OPERATOR_SIZE = 1;
const std::map<std::string, TOKEN_TYPE> SINGLE_OPERATORS = {
        // Single Character Operators @NOTE : We don't insert '/' here as it needs to be check for start-of-comment use before being able to assert if its an operator.
        {"+", TOKEN_TYPE::ADD},
        {"-", TOKEN_TYPE::SUB},
        {"*", TOKEN_TYPE::STAR},
        {"/", TOKEN_TYPE::DIV},
        {"%", TOKEN_TYPE::MOD},
        {"=", TOKEN_TYPE::ASSIGN},
        {"!", TOKEN_TYPE::BANG},
        {"&", TOKEN_TYPE::AMP},
        {"<", TOKEN_TYPE::LESS},
        {">", TOKEN_TYPE::GREAT},
        {"|", TOKEN_TYPE::PIPE},
//        {".", TOKEN_TYPE::DOT},
        {",", TOKEN_TYPE::COMMA},
//        {":", TOKEN_TYPE::COLON},
        {"(", TOKEN_TYPE::LPARENS},
        {")", TOKEN_TYPE::RPARENS},
        {"[", TOKEN_TYPE::LBRACKET},
        {"]", TOKEN_TYPE::RBRACKET},
        {"{", TOKEN_TYPE::LBRACE},
        {"}", TOKEN_TYPE::RBRACE},
        {";", TOKEN_TYPE::SEMICOLON},
};

constexpr std::array<TOKEN_TYPE, 15> ARITHMETIC_TOKENS = {
        TOKEN_TYPE::LOR,    // ||
        TOKEN_TYPE::LAND,   // &&
        TOKEN_TYPE::LESS,   // <
        TOKEN_TYPE::GREAT,  // >
        TOKEN_TYPE::LEQ,    // <=
        TOKEN_TYPE::GEQ,    // >=
        TOKEN_TYPE::EQ,     // ==
        TOKEN_TYPE::NEQ,    // !=
        TOKEN_TYPE::ADD,    // +
        TOKEN_TYPE::SUB,    // -
        TOKEN_TYPE::STAR,   // *
        TOKEN_TYPE::DIV,    // /
        TOKEN_TYPE::MOD,    // %
        TOKEN_TYPE::PIPE,   // |
        TOKEN_TYPE::AMP,    // &
};

struct Token {

    Token(const TOKEN_TYPE& token_type, const std::variant<int, std::string, char>& value);
    bool operator==(const Token& other) const;

    TOKEN_TYPE m_type;
    std::variant<int, std::string, char> m_value;

    std::string to_string() const;
};
