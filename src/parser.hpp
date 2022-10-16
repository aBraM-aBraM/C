#pragma once

#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <stack>

#include "lexer.h"


struct ASTNode;

struct BinaryOperation {
    BinaryOperation(std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs) : lhs(std::move(lhs)),
                                                                                  rhs(std::move(rhs)) {}

    std::unique_ptr<ASTNode> rhs;
    std::unique_ptr<ASTNode> lhs;
};

struct FuncCall {
    std::vector<std::unique_ptr<ASTNode>> arg;
};

struct Block {
    std::vector<std::unique_ptr<ASTNode>> statements;
};


struct VariableDeclaration {
    Token type;
};

struct FuncDeclaration {
    Token return_type;
    std::vector<Token> args_types;
};

struct ASTNode {
    ASTNode(const Token &token) : m_token(token) {};

    ASTNode(const Token &token, FuncCall &&func_members) : m_token(token), m_members(std::move(func_members)) {};

    ASTNode(const Token &token, BinaryOperation &&operation_members) : m_token(token),
                                                                       m_members(std::move(operation_members)) {};

    ASTNode(const Token &token, VariableDeclaration &&variable_declaration) : m_token(token),
                                                                              m_members(std::move(
                                                                                      variable_declaration)) {};

    ASTNode(const Token &token, FuncDeclaration &&function_declaration) : m_token(token),
                                                                          m_members(std::move(function_declaration)) {};

    ASTNode(const Token &token, Block &&block) : m_token(token),
                                                 m_members(std::move(block)) {};

    Token m_token;
    std::variant<std::monostate, FuncCall, BinaryOperation, VariableDeclaration, FuncDeclaration, Block> m_members;
};

constexpr const char *NON_COMMA_SEPARATED_ARGS_ERROR = "unexpected two arguments in a row";
constexpr const char *NON_SEMICOLON_STATEMENT_SUFFIX = "missing an expected semicolon at the end of the statement";
constexpr const char *VARIABLE_DEFINITION_WITHOUT_TYPE = "missing a type at variable definition";
constexpr const char *VARIABLE_DEFINITION_WITHOUT_NAME = "missing a variable name at variable definition";
constexpr const char *BAD_ASSIGNMENT = "assignment without lvalue in lhs";

constexpr const char *UNEXPECTED_DANLGING_EXPRESSION = "Expected an assignment or function call when making a dangling expression";
constexpr const char *UNEXPECTED_DANGLING_DECLARATION = "Expected variable / function name declaration";
constexpr const char *BAD_DECLARATION = "Unexpected declaration";
constexpr const char *DANGLING_FUNC_DECLARATION = "Expected opening parentheses in function declaration";
constexpr const char *FUNC_DECLARATION_PARAM_MISSING_TYPE = "Expected parameter type in function declaration";

constexpr const char *UNCLOSED_SCOPE = "Expected scope close suffix";


template<typename Iterator, typename T>
Iterator get_scope_end(const Iterator &begin, const Iterator &end, const T &scope_prefix, const T &scope_suffix) {
    std::stack<Token> stack;
    for (auto it = begin; it < end; ++it) {
        if (*it == scope_prefix) {
            stack.push(*it);
        } else if (*it == scope_suffix) {
            stack.pop();
            if (stack.empty()) {
                return it;
            }
        }
    }
    throw CompilerException(UNCLOSED_SCOPE);
}


class Parser {
public:

    template<typename Iterator>
    std::unique_ptr<ASTNode> parse_func_call(Iterator &it, const Iterator &statement_end) {
        auto func_node = std::make_unique<ASTNode>(Token(TOKEN_TYPE::FUNC_CALL, it->m_value),
                                                   FuncCall({}));

        DEBUG_MSG("parsing function call: " << func_node->m_token.to_string() << "(");
        it += 2; // skip function name and left parentheses

        bool is_arg = true; // used to enforce commas between arguments

        while (it->m_type != TOKEN_TYPE::RPARENS) {
            if (it >= statement_end) {
                throw CompilerException("unclosed function call");
            }

            if (it->m_type == TOKEN_TYPE::COMMA) {
                ++it;
                is_arg = true;
                continue;
            }

            if (!is_arg) {
                throw CompilerException(NON_COMMA_SEPARATED_ARGS_ERROR);
            }

            // not parsing an expression because I don't expect assignments
            auto argument = parse_arithmetic( it, statement_end);
            DEBUG_MSG("parsed arg: " << argument->m_token.to_string());

            std::get<FuncCall>(func_node->m_members).arg.push_back(std::move(argument));
            is_arg = false;
        }
        ++it;
        DEBUG_MSG(")");

        return std::move(func_node);
    }

    template<typename Iterator>
    std::unique_ptr<ASTNode> parse_func_declaration(Iterator &it, const Iterator &statement_end) {
        Token return_type = *it++;
        auto func_node = std::make_unique<ASTNode>(*it++);

        DEBUG_MSG("parsing function declaration: " << return_type.to_string() << " " << func_node->m_token.to_string()
                                                   << "(");

        func_node->m_members = FuncDeclaration({.return_type = return_type});

        bool is_arg = true; // used to enforce commas between arguments

        if (it->m_type != TOKEN_TYPE::LPARENS) {
            throw CompilerException(DANGLING_FUNC_DECLARATION);
        }
        ++it; // skip left parentheses


        while (it->m_type != TOKEN_TYPE::RPARENS) {
            if (it >= statement_end) {
                throw CompilerException("unclosed function declaration");
            }

            if (it->m_type == TOKEN_TYPE::COMMA) {
                ++it;
                is_arg = true;
                continue;
            }

            if (!is_arg) {
                throw CompilerException(NON_COMMA_SEPARATED_ARGS_ERROR);
            }

            if (it->m_type == TOKEN_TYPE::INT || it->m_type == TOKEN_TYPE::CHAR) {
                DEBUG_MSG("parsing arg type: " << it->to_string());
                std::get<FuncDeclaration>(func_node->m_members).args_types.push_back(*it++);
                if (it->m_type == TOKEN_TYPE::IDENTIFIER) {
                    ++it; // param name isn't interesting
                }
                is_arg = false;
            } else {
                throw CompilerException(FUNC_DECLARATION_PARAM_MISSING_TYPE);
            }
        }
        ++it;
        DEBUG_MSG(")");

        return std::move(func_node);
    }

    template<typename Iterator>
    std::unique_ptr<ASTNode> parse_variable_declaration(Iterator &it, const Iterator &statement_end) {
        Token type = *it++;
        auto declaration_node = std::make_unique<ASTNode>(*it++);
        declaration_node->m_members = VariableDeclaration({type});
        DEBUG_MSG("parsing declaration: " << declaration_node->m_token.to_string()
                                          << " of type " << type.to_string());
        return std::move(declaration_node);
    }

    template<typename Iterator>
    std::unique_ptr<ASTNode> parse_block(Iterator &it, const Iterator &statement_end) {

    }

    template<typename Iterator>
    std::unique_ptr<ASTNode> parse_factor(Iterator &it, const Iterator &statement_end) {
        switch (it->m_type) {
            case TOKEN_TYPE::INTEGER:
            case TOKEN_TYPE::CHARACTER:
            case TOKEN_TYPE::STRING:
                return std::make_unique<ASTNode>(*it++);
            case TOKEN_TYPE::IDENTIFIER:
                if (std::distance(it, statement_end) > 1 && (it + 1)->m_type == TOKEN_TYPE::LPARENS) {
                    return parse_func_call(it, statement_end);
                } else {
                    // variables
                    DEBUG_MSG("parsing variable identifier: " << it->to_string());
                    return std::make_unique<ASTNode>(*it++);
                }
            default:
                throw CompilerException((std::string("unsupported factor token: ") + it->to_string()).c_str());
        }
    }

    template<typename Iterator>
    std::unique_ptr<ASTNode> parse_arithmetic(Iterator &it, const Iterator &statement_end) {
        auto lhs = parse_factor(it, statement_end);

        while (std::find(ARITHMETIC_TOKENS.begin(), ARITHMETIC_TOKENS.end(), it->m_type) != ARITHMETIC_TOKENS.end()) {
            DEBUG_MSG("parsing arithmetic: " << it->to_string());

            auto arithmetic_node = std::make_unique<ASTNode>(*it++);
            arithmetic_node->m_members = BinaryOperation(std::move(lhs), parse_factor(it, statement_end));

            lhs = std::move(arithmetic_node);
        }

        return lhs;
    }

    template<typename Iterator>
    std::unique_ptr<ASTNode> parse_expression(Iterator &it, const Iterator &statement_end) {
        auto lhs = parse_arithmetic(it, statement_end);

        while (it->m_type == TOKEN_TYPE::ASSIGN) {
            DEBUG_MSG("parsing expression " << it->to_string());

            auto assign_node = std::make_unique<ASTNode>(*it++);

            assign_node->m_members = BinaryOperation(std::move(lhs), parse_arithmetic(it, statement_end));

            lhs = std::move(assign_node);
        }

        return lhs;
    }

    template<typename Iterator>
    void validate_assignment(const Iterator &statement_start, const Iterator &it) {
        const auto &assign_it = std::find(statement_start, it, Token(TOKEN_TYPE::ASSIGN, "="));
        const auto lhs_length = std::distance(statement_start, assign_it);
        if (lhs_length > 1) {
            // definition
            if (std::find(TYPES.begin(), TYPES.end(), statement_start->m_type) == TYPES.end()) {
                throw CompilerException(VARIABLE_DEFINITION_WITHOUT_TYPE);
            }
            if ((assign_it - 1)->m_type != TOKEN_TYPE::IDENTIFIER) {
                throw CompilerException(VARIABLE_DEFINITION_WITHOUT_NAME);
            }
        } else {
            // assignment of existing variable
            if (statement_start->m_type != TOKEN_TYPE::IDENTIFIER) {
                throw CompilerException(BAD_ASSIGNMENT);
            }
        }

    }

    template<typename Iterator>
    std::unique_ptr<ASTNode> parse_declaration(Iterator &it, const Iterator &statement_end) {
        if (std::distance(it, statement_end) < 1) {
            throw CompilerException(UNEXPECTED_DANGLING_DECLARATION);
        }
        if ((it + 1)->m_type == TOKEN_TYPE::IDENTIFIER) {
            if (std::find(it, statement_end, Token(TOKEN_TYPE::LPARENS, "(")) == statement_end) {
                // might be variable declaration
                return parse_variable_declaration(it, statement_end);
            } else {
                // function declaration
                return parse_func_declaration(it, statement_end);
            }
        }
        throw CompilerException(BAD_DECLARATION);
    }

    template<typename Iterator>
    std::unique_ptr<ASTNode> parse_statement(Iterator &it, const Iterator &statement_end) {
        std::unique_ptr<ASTNode> statement;
        DEBUG_MSG("parsing statement: " << it->to_string());
        auto statement_start = it;
        switch (it->m_type) {
            case TOKEN_TYPE::INT:
            case TOKEN_TYPE::CHAR:
            case TOKEN_TYPE::VOID:
                statement = parse_declaration( it, statement_end);
                break;
            default:
                statement = parse_expression(it, statement_end);
                switch (statement->m_token.m_type) {
                    case TOKEN_TYPE::ASSIGN:
                        validate_assignment(statement_start, it);
                    case TOKEN_TYPE::FUNC_CALL:
                        break;
                    default:
                        throw CompilerException(UNEXPECTED_DANLGING_EXPRESSION);
                }
        }
        if (it->m_type != TOKEN_TYPE::SEMICOLON) {
            throw CompilerException(NON_SEMICOLON_STATEMENT_SUFFIX);
        }
        ++it;
        return std::move(statement);
    }
};


