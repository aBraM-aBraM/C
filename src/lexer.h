#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <string_view>
#include <map>
#include <span>
#include <variant>

#include "macros.h"
#include "exceptions.h"
#include "token.h"

constexpr std::string_view LINE_COMMENT = "//";
constexpr char STRING_DELIMITER = '"';
constexpr char CHAR_DELIMITER = '\'';
constexpr int CHAR_EXPRESSION_LENGTH = 3; // <delimiter><char><delimiter>




class Lexer {
public:
    std::unique_ptr<std::vector<Token>> lex(std::istream &file_to_lex);

private:
    void lex_line(std::string_view statement);

    bool scan_token(std::string_view possible_token, const std::map<std::string, TOKEN_TYPE> &search_tokens);

    template<typename Iterator>
    bool scan_line_comment(std::string_view statement, const Iterator &it) {
        if (std::distance(it, statement.end()) + 1 >= LINE_COMMENT.size()) {

            std::string_view token_value(it, LINE_COMMENT.size());

            if (!token_value.compare(LINE_COMMENT)) {
                DEBUG_MSG("Skipping line comment: " << it);
                return true;
            }
        }
        return false;
    }

    template<typename Iterator>
    size_t scan_compound_operator(std::string_view statement, const Iterator &it) {
        if (std::distance(it, statement.end()) + 1 >= COMPOUND_OPERATOR_SIZE) {

            std::string_view token_value(it, COMPOUND_OPERATOR_SIZE);

            if (scan_token(token_value, COMPOUND_OPERATORS)) {
                return COMPOUND_OPERATOR_SIZE;
            }
        }
        return 0;

    }

    template<typename Iterator>
    size_t scan_single_operator(const Iterator &it) {

        std::string_view token_value(it, SINGLE_OPERATOR_SIZE);

        if (scan_token(token_value, SINGLE_OPERATORS)) {
            return SINGLE_OPERATOR_SIZE;
        }
        return 0;
    }

    template<class Iterator>
    size_t scan_whitespace(Iterator &it) {

        // TODO: update column and rows when available
        return 0;
    }

    template<typename Iterator>
    size_t scan_literal_string(std::string_view statement, const Iterator &it) {

        if (*it == STRING_DELIMITER) {

            auto string_end_it = std::find(it + 1, // skip current delimiter
                                           statement.end(),
                                           STRING_DELIMITER);
            if (string_end_it == statement.end()) {
                throw CompilerException("unclosed string literal");
            }
            std::string string_token(it + 1, string_end_it);
            DEBUG_MSG("string token: \"" << string_token << "\"");
            m_tokens->push_back({TOKEN_TYPE::STRING, string_token});
            return std::distance(it, string_end_it) + 1; // add one to skip last delimiter
        }

        return 0;
    }

    template<typename Iterator>
    size_t scan_literal_char(std::string_view statement, const Iterator &it) {
        if (std::distance(it, statement.end()) + 1 >= CHAR_EXPRESSION_LENGTH) {
            if (*it == CHAR_DELIMITER) {
                if (*(it + 2) != CHAR_DELIMITER) // expected char delimiter as expression suffix
                {
                    throw CompilerException("unclosed char literal");
                }
                char char_token = *(it + 1);
                DEBUG_MSG("char token: '" << std::string(1, char_token) << "'");
                m_tokens->push_back({TOKEN_TYPE::CHARACTER, char_token});
                return CHAR_EXPRESSION_LENGTH;
            }
        }
        return 0;
    }

    template<typename Iterator>
    size_t scan_literal_int(std::string_view statement, const Iterator &it) {
        if(std::isdigit(*it)){
            for (auto int_it = it; int_it < statement.end(); ++int_it){
                if(!std::isdigit(*int_it)){
                    std::string string_int(it, int_it);
                    DEBUG_MSG("int literal token: " << string_int);
                    m_tokens->push_back({TOKEN_TYPE::INTEGER, std::stoi(string_int)});
                    return std::distance(it, int_it);
                }
            }
        }
        return 0;
    }

    template<typename Iterator>
    size_t scan_keyword_identifier(std::string_view statement, const Iterator&it){
        // first letter of all keywords / identifiers is alphabetical
        if(std::isalpha(*it)){
            for (auto word_it = it; word_it < statement.end(); ++word_it){
                // non first letter in an identifier can be alphanumeric
                if(!std::isalnum(*word_it) && *word_it != '_'){
                    std::string word_string(it, word_it);
                    if(KEYWORDS.find(word_string) != KEYWORDS.end())
                    {
                        DEBUG_MSG("keyword token: " << word_string);
                        m_tokens->push_back({KEYWORDS.at(word_string), word_string});
                    }
                    else{
                        DEBUG_MSG("identifier token: " << word_string);
                        m_tokens->push_back({TOKEN_TYPE::IDENTIFIER, word_string});
                    }
                    return std::distance(it, word_it);
                }
            }
        }
        return 0;
    }

    std::unique_ptr<std::vector<Token>> m_tokens;
};

