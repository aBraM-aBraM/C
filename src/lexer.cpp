
#include <vector>
#include <iosfwd>
#include <memory>
#include <iostream>
#include <span>
#include "lexer.h"
#include "token.h"


std::unique_ptr<std::vector<Token>> Lexer::lex(std::istream &file_to_lex) {
    m_tokens = std::make_unique<std::vector<Token>>();

    std::string line;
    while (std::getline(file_to_lex, line)) {
        lex_line(line);
    }

    return std::move(m_tokens);
}

void Lexer::lex_line(std::string_view statement) {

    auto it = statement.begin();
    while (it < statement.end()) {

        if(scan_line_comment(statement, it)) return;

        size_t stride;
        stride = scan_compound_operator(statement, it);
        stride = stride == 0 ? scan_single_operator(it): stride;
        stride = stride == 0 ? scan_whitespace(it): stride;
        stride = stride == 0 ? scan_literal_string(statement, it): stride;
        stride = stride == 0 ? scan_literal_char(statement, it): stride;
        stride = stride == 0 ? scan_literal_int(statement, it): stride;
        stride = stride == 0 ? scan_keyword_identifier(statement, it): stride;

        stride = stride == 0 ? 1: stride;
        it += stride;
    }
}

bool Lexer::scan_token(std::string_view possible_token, const std::map<std::string, TOKEN_TYPE> &search_tokens) {
    for (const auto &searched_token: search_tokens) {
        if (!possible_token.compare(searched_token.first)) {
            DEBUG_MSG("token: " << possible_token);
            m_tokens->push_back({searched_token.second, std::string(possible_token)});
            return true;
        }
    }
    return false;

}

