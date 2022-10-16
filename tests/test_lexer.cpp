#include <gtest/gtest.h>
#include <string>
#include <array>
#include "src/lexer.h"

constexpr auto CODE_FILE = "../../tests/hello_world.c";

bool contains_tokens(const std::vector<Token> &tokens, const std::map<std::string, TOKEN_TYPE> &expected_tokens) {
    for (auto &[expected_token, expected_token_type]: expected_tokens) {
        bool contains_type = false;
        for (auto &[token_type, token_value]: tokens) {
            if (token_type == expected_token_type) {
                contains_type = true;
            }
        }
        if (!contains_type) {
            return false;
        }
    }
    return true;
}

bool contains_token(const std::vector<Token> &tokens, const Token &expected_token) {
    for (auto &[token_type, token_value]: tokens) {
        if (token_value == expected_token.m_value && token_type == expected_token.m_type) {
            return true;
        }
    }
    return false;
}

TEST(UnitTests, TestHelloWorld) {
    std::ifstream input_file(CODE_FILE);

    Lexer lexer;
    try {
        auto tokens = lexer.lex(input_file);

        ASSERT_TRUE(contains_tokens(*tokens, COMPOUND_OPERATORS));
        ASSERT_TRUE(contains_tokens(*tokens, SINGLE_OPERATORS));
        ASSERT_TRUE(contains_tokens(*tokens, KEYWORDS));
        ASSERT_TRUE(contains_token(*tokens, {TOKEN_TYPE::STRING, "expected_string"}));
        ASSERT_TRUE(contains_token(*tokens, {TOKEN_TYPE::CHARACTER, 'f'}));
        ASSERT_TRUE(contains_token(*tokens, {TOKEN_TYPE::INTEGER, 1355}));
        ASSERT_TRUE(contains_token(*tokens, {TOKEN_TYPE::IDENTIFIER, "my_str"}));
        // TODO: figure out how to test comments
    }
    catch (CompilerException &exc) {
        GTEST_FATAL_FAILURE_(exc.what());
    }

}
