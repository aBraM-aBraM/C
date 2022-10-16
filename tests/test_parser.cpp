#include <gtest/gtest.h>
#include "src/lexer.h"
#include "src/parser.hpp"


class ParserTestSetup : public ::testing::Test {
protected:
    void SetUp() override {
        value_to_token.insert(KEYWORDS.begin(), KEYWORDS.end());
        value_to_token.insert(COMPOUND_OPERATORS.begin(), COMPOUND_OPERATORS.end());
        value_to_token.insert(SINGLE_OPERATORS.begin(), SINGLE_OPERATORS.end());
    }

    Parser parser;
    std::map<std::string, TOKEN_TYPE> value_to_token;
};

TEST_F(ParserTestSetup, TestOperations) {
    // Test expression, for every operation parse expression returns expected lhs and rhs
    for (const auto &[value, token]: value_to_token) {
        if (std::find(ARITHMETIC_TOKENS.begin(), ARITHMETIC_TOKENS.end(), token) != ARITHMETIC_TOKENS.end()) {
            std::vector<Token> tokens({{TOKEN_TYPE::IDENTIFIER, "a"},
                                       {token,                  value},
                                       {TOKEN_TYPE::INTEGER,    5}});
            auto it = tokens.begin();
            std::unique_ptr<ASTNode> res = parser.parse_expression(it, tokens.end());
            auto &operation_members = std::get<BinaryOperation>(res->m_members);

            ASSERT_EQ(res->m_token, tokens[1]);
            ASSERT_EQ(operation_members.lhs->m_token, tokens[0]);
            ASSERT_EQ(operation_members.rhs->m_token, tokens[2]);
        }
    }
}

TEST_F(ParserTestSetup, TestAssign) {
    // Test expression, sanity assignment expression operation
    std::vector<Token> tokens({{TOKEN_TYPE::IDENTIFIER, "a"},
                               {TOKEN_TYPE::ASSIGN,     "="},
                               {TOKEN_TYPE::INTEGER,    5}});
    auto it = tokens.begin();
    std::unique_ptr<ASTNode> res = parser.parse_expression(it, tokens.end());
    auto &operation_members = std::get<BinaryOperation>(res->m_members);

    ASSERT_EQ(res->m_token, tokens[1]);
    ASSERT_EQ(operation_members.lhs->m_token, tokens[0]);
    ASSERT_EQ(operation_members.rhs->m_token, tokens[2]);
}

TEST_F(ParserTestSetup, TestFunctionLiteral) {

    // Test expression, empty function call, one parameter, two & three parameters
    for (int i = 0; i < 4; i++) {

        std::vector<Token> tokens({{TOKEN_TYPE::IDENTIFIER, "my_func_name"},
                                   {TOKEN_TYPE::LPARENS,    "("},
                                   {TOKEN_TYPE::RPARENS,    ")"}});
        // setup tokens
        for (int j = 0; j < i; ++j) {
            tokens.insert(tokens.end() - 1, Token(TOKEN_TYPE::INTEGER, j));
            if (j < i - 1) {
                tokens.insert(tokens.end() - 1, Token(SINGLE_OPERATORS.at(","), ","));
            }
        }

        auto it = tokens.begin();
        std::unique_ptr<ASTNode> res = parser.parse_expression(it, tokens.end());

        auto &func_members = std::get<FuncCall>(res->m_members);

        ASSERT_EQ(res->m_token, Token(TOKEN_TYPE::FUNC_CALL, tokens[0].m_value));
        ASSERT_EQ(func_members.arg.size(), i);

        if (!func_members.arg.empty()) {
            for (int j = 0; j < func_members.arg.size(); ++j) {
                int token_index = 2; // first argument index
                token_index += 2 * j; // argument index skipping commas
                ASSERT_TRUE(tokens[token_index] == std::move(func_members.arg[j])->m_token);
            }
        }
    }
}

TEST_F(ParserTestSetup, TestFunctionExperssion) {
    // Test expression, function call with expressions instead of literals
    std::vector<Token> add_expression({{TOKEN_TYPE::INTEGER, 1},
                                       {TOKEN_TYPE::ADD,     "+"},
                                       {TOKEN_TYPE::INTEGER, 1}});

    std::vector<Token> tokens({{TOKEN_TYPE::IDENTIFIER, "my_func_name"},
                               {TOKEN_TYPE::LPARENS,    "("},
                               {TOKEN_TYPE::RPARENS,    ")"}});

    tokens.insert(tokens.end() - 1, add_expression.begin(), add_expression.end());

    auto add_expression_it = add_expression.begin();
    std::unique_ptr<ASTNode> parsed_add_expression = parser.parse_expression(add_expression_it, add_expression.end());
    auto it = tokens.begin();
    std::unique_ptr<ASTNode> res = parser.parse_expression(it, tokens.end());
    auto &func_members = std::get<FuncCall>(res->m_members);

    ASSERT_EQ(res->m_token, Token(TOKEN_TYPE::FUNC_CALL, tokens[0].m_value));
    ASSERT_EQ(func_members.arg.size(), 1); // one parameter (addition expression)
    ASSERT_EQ(func_members.arg[0]->m_token, parsed_add_expression->m_token);
}

TEST_F(ParserTestSetup, TestFunctionSyntaxError) {
    // Test expression, bad function call with literal without commas between expect error
    std::vector<Token> tokens({{TOKEN_TYPE::IDENTIFIER, "my_func_name"},
                               {TOKEN_TYPE::LPARENS,    "("},
                               {TOKEN_TYPE::INTEGER,    1},
                               {TOKEN_TYPE::INTEGER,    1},
                               {TOKEN_TYPE::RPARENS,    ")"}});
    auto it = tokens.begin();
    try {
        parser.parse_expression(it, tokens.end());
        FAIL(); // should not reach here due to exception
    }
    catch (CompilerException &exception) {
        ASSERT_STREQ(exception.what(), NON_COMMA_SEPARATED_ARGS_ERROR);
    }
}

TEST_F(ParserTestSetup, TestDeclarationSyntaxErrorNoIdentifier) {
    // Test statement, bad declaration with no identifier on lhs
    /*
     * This falls on declaration rather than assignment with no lhs because
     * declaration is evaluated before assignment
     */
    std::vector<Token> tokens({{TOKEN_TYPE::INT,       "int"},
                               {TOKEN_TYPE::ASSIGN,    "="},
                               {TOKEN_TYPE::INTEGER,   1},
                               {TOKEN_TYPE::SEMICOLON, ";"}});
    auto it = tokens.begin();
    try {
        parser.parse_statement(it, tokens.end());
        FAIL(); // should not reach here due to exception
    }
    catch (CompilerException &exception) {
        ASSERT_STREQ(exception.what(), BAD_DECLARATION);
    }
}

TEST_F(ParserTestSetup, TestAssignmentSyntaxErrorNoLValue) {
    // Test statement, bad assignment with no identifier on lhs
    /*
     * Because there's no type prefix to the statement declaration is not evaluated
     * and instead the assignment fails because there's no value on the lhs
    */
    std::vector<Token> tokens({{TOKEN_TYPE::INTEGER,   1},
                               {TOKEN_TYPE::ASSIGN,    "="},
                               {TOKEN_TYPE::INTEGER,   1},
                               {TOKEN_TYPE::SEMICOLON, ";"}});
    auto it = tokens.begin();
    try {
        parser.parse_statement(it, tokens.end());
        FAIL(); // should not reach here due to exception
    }
    catch (CompilerException &exception) {
        ASSERT_STREQ(exception.what(), BAD_ASSIGNMENT);
    }
}

TEST_F(ParserTestSetup, TestFuncDeclaration) {
    // Test statement, function declaration with parameters identifiers
    Token return_type = {TOKEN_TYPE::INT, "int"};
    Token first_param = {TOKEN_TYPE::INT, "int"};
    Token second_param = {TOKEN_TYPE::CHAR, "char"};

    std::vector<Token> tokens({return_type,
                               {TOKEN_TYPE::IDENTIFIER, "my_func_name"},
                               {TOKEN_TYPE::LPARENS, "("},
                               first_param,
                               {TOKEN_TYPE::IDENTIFIER, "first_param_name"},
                               {TOKEN_TYPE::COMMA, ","},
                               second_param,
                               {TOKEN_TYPE::IDENTIFIER, "second_param_name"},
                               {TOKEN_TYPE::RPARENS, ")"},
                               {TOKEN_TYPE::SEMICOLON, ";"}});

    auto it = tokens.begin();
    auto statement = parser.parse_statement(it, tokens.end());
    auto &func_declaration = std::get<FuncDeclaration>(statement->m_members);

    ASSERT_EQ(func_declaration.return_type, return_type);
    ASSERT_EQ(func_declaration.args_types[0], first_param);
    ASSERT_EQ(func_declaration.args_types[1], second_param);
}

TEST_F(ParserTestSetup, TestFuncDeclarationNoArgsIdentifiers) {
    // Test statement, function declaration without parameters identifiers
    Token return_type = {TOKEN_TYPE::INT, "int"};
    Token first_param = {TOKEN_TYPE::INT, "int"};
    Token second_param = {TOKEN_TYPE::CHAR, "char"};

    std::vector<Token> tokens({return_type,
                               {TOKEN_TYPE::IDENTIFIER, "my_func_name"},
                               {TOKEN_TYPE::LPARENS, "("},
                               first_param,
                               {TOKEN_TYPE::COMMA, ","},
                               second_param,
                               {TOKEN_TYPE::RPARENS, ")"},
                               {TOKEN_TYPE::SEMICOLON, ";"}});

    auto it = tokens.begin();
    auto statement = parser.parse_statement(it, tokens.end());
    auto &func_declaration = std::get<FuncDeclaration>(statement->m_members);

    ASSERT_EQ(func_declaration.return_type, return_type);
    ASSERT_EQ(func_declaration.args_types[0], first_param);
    ASSERT_EQ(func_declaration.args_types[1], second_param);
}

TEST_F(ParserTestSetup, TestFuncDeclarationBadSyntax) {
    // Test statement, function declaration without parameter type in one parameter
    Token return_type = {TOKEN_TYPE::INT, "int"};
    Token first_param = {TOKEN_TYPE::INT, "int"};
    Token second_param = {TOKEN_TYPE::IDENTIFIER, "my_second_param"};

    std::vector<Token> tokens({return_type,
                               {TOKEN_TYPE::IDENTIFIER, "my_func_name"},
                               {TOKEN_TYPE::LPARENS, "("},
                               first_param,
                               {TOKEN_TYPE::COMMA, ","},
                               second_param,
                               {TOKEN_TYPE::RPARENS, ")"},
                               {TOKEN_TYPE::SEMICOLON, ";"}});

    auto it = tokens.begin();
    try {
        auto statement = parser.parse_statement(it, tokens.end());
    }
    catch (CompilerException &exc) {
        ASSERT_STREQ(exc.what(), FUNC_DECLARATION_PARAM_MISSING_TYPE);
    }

}

TEST_F(ParserTestSetup, TestParseStatement) {
    // General test statement parsing for every parsable supported statement type
    std::vector<Token> assignment({{TOKEN_TYPE::IDENTIFIER, "a"},
                                   {TOKEN_TYPE::ASSIGN,     "="},
                                   {TOKEN_TYPE::INTEGER,    5}});
    std::vector<Token> func_call({{TOKEN_TYPE::IDENTIFIER, "my_func_name"},
                                  {TOKEN_TYPE::LPARENS,    "("},
                                  {TOKEN_TYPE::INTEGER,    1},
                                  {TOKEN_TYPE::RPARENS,    ")"}});

    std::vector<std::reference_wrapper<std::vector<Token>>> statements = {assignment, func_call};
    const Token statement_end(TOKEN_TYPE::SEMICOLON, ";");

    std::vector<Token> program;

    for (const auto &statement: statements) {
        program.insert(program.end(), statement.get().begin(), statement.get().end());
        program.insert(program.end(), statement_end);
    }

    auto it = program.begin();

    for (int i = 0; i < statements.size(); ++i) {
        auto parsed_statement = parser.parse_statement(it, program.end());
    }

}

TEST(UnitTests, TestBracketSuffixFinder) {
    Token scope_prefix = Token(TOKEN_TYPE::LBRACE, "{");
    Token scope_suffix = Token(TOKEN_TYPE::RBRACE, "}");
    std::vector<Token> simple_tokens({scope_prefix,
                                      {TOKEN_TYPE::ASSIGN, "="},
                                      scope_suffix});
    std::vector<Token> complex_tokens({scope_prefix,
                                       {TOKEN_TYPE::ASSIGN, "="},
                                       {TOKEN_TYPE::ASSIGN, "="},
                                       scope_prefix,
                                       {TOKEN_TYPE::ASSIGN, "="},
                                       scope_suffix,
                                       {TOKEN_TYPE::ASSIGN, "="},
                                       scope_suffix,
                                       {TOKEN_TYPE::ASSIGN, "="}});

    std::vector<Token> bad_tokens({scope_prefix,
                                   scope_prefix,
                                   {TOKEN_TYPE::ASSIGN, "="},
                                   scope_suffix});

    auto ending_bracket = get_scope_end(simple_tokens.begin(), simple_tokens.end(), scope_prefix, scope_suffix);
    ASSERT_EQ(simple_tokens[simple_tokens.size() - 1], *ending_bracket);

    ending_bracket = get_scope_end(complex_tokens.begin(), complex_tokens.end(), scope_prefix, scope_suffix);
    ASSERT_EQ(complex_tokens[complex_tokens.size() - 2], *ending_bracket);

    try {
        get_scope_end(bad_tokens.begin(), bad_tokens.end(), scope_prefix, scope_suffix);
    }
    catch (CompilerException& exc)
    {
        ASSERT_STREQ(exc.what(), UNCLOSED_SCOPE);
    }
}