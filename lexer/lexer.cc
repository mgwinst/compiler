#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <cassert>

#include "lexer.h"

[[nodiscard]] auto Lexer::get_token() -> Token {
    auto start = cur, end = cur;

    while (cur != source.end()) {
        // whitespace
        if (std::isspace(*cur)) {
            if (*cur == '\n') {
                line_num++;
                col_num = 1;
            } else {
                col_num++;
            }
            cur++;
        
        // digits
        } else if (std::isdigit(*cur)) {
            start = cur;
            auto start_col_num = col_num;
            while (std::isdigit(*cur)) {
                cur++; col_num++;
            }
            end = cur;
            return Token{TokenType::NUMERIC_LITERAL, std::string_view{start, end}, line_num, start_col_num, static_cast<std::size_t>(end-start)};
        
        // identifiers, types and keywords
        } else if (std::isalpha(*cur)) {
            start = cur;
            while (std::isalnum(*cur) || *cur == '_') {
                cur++; col_num++;
            }
            end = cur;

            std::string_view value{start, end};

            if (auto kw = keywords.find(value); kw != keywords.end()) {
                return Token{kw->second, std::string_view{start, end}, line_num, col_num - (end-start), static_cast<std::size_t>(end-start)};
            } else if (auto t = lang_types.find(value); t != lang_types.end()) {
                return Token{TokenType::TYPE, std::string_view{start, end}, line_num, col_num - (end-start), static_cast<std::size_t>(end-start)};
            } else {
                return Token{TokenType::IDENTIFIER, std::string_view{start, end}, line_num, col_num - (end-start), static_cast<std::size_t>(end-start)};
            }

        // symbols
        } else if (auto double_symbol = double_symbol_map.find(std::string_view{cur, 2}); double_symbol != double_symbol_map.end()) {
            Token token{double_symbol->second, std::string_view{cur, 2}, line_num, col_num, 2};
            cur += 2; col_num += 2;
            return token;
        } else if (auto symbol = single_symbol_map.find(*cur); symbol != single_symbol_map.end()) {
            return Token{symbol->second, std::string_view{cur++, 1}, line_num, col_num++, 1};
        } else {
            return Token{TokenType::INVALID, std::string_view{cur, 1}, line_num, col_num, 1};
        }
    }

    return Token{TokenType::END_OF_FILE, {}, line_num, col_num, 1};
}

[[nodiscard]] auto Lexer::peek_token() -> Token {
    auto peek_cur = cur;
    auto start = peek_cur, end = peek_cur;

    while (peek_cur != source.end()) {
        // whitespace
        if (std::isspace(*peek_cur)) {
            peek_cur++;
        
        // digits
        } else if (std::isdigit(*peek_cur)) {
            start = peek_cur;
            while (std::isdigit(*peek_cur)) {
                peek_cur++;
            }
            end = peek_cur;
            return Token{TokenType::NUMERIC_LITERAL, std::string_view{start, end}};
        
        // identifiers, types and keywords
        } else if (std::isalpha(*peek_cur)) {
            start = peek_cur;
            while (std::isalnum(*peek_cur) || *peek_cur == '_') {
                peek_cur++;
            }
            end = peek_cur;

            std::string_view value{start, end};

            if (auto kw = keywords.find(value); kw != keywords.end()) {
                return Token{kw->second, std::string_view{start, end}};
            } else if (auto t = lang_types.find(value); t != lang_types.end()) {
                return Token{TokenType::TYPE, std::string_view{start, end}};
            } else {
                return Token{TokenType::IDENTIFIER, std::string_view{start, end}};
            }

        // symbols
        } else if (auto double_symbol = double_symbol_map.find(std::string_view{peek_cur, 2}); double_symbol != double_symbol_map.end()) {
            return Token{double_symbol->second, std::string_view{peek_cur, 2}};
        } else if (auto symbol = single_symbol_map.find(*peek_cur); symbol != single_symbol_map.end()) {
            return Token{symbol->second, std::string_view{peek_cur, 1}};
        } else {
            return Token{TokenType::INVALID, std::string_view{peek_cur, 1}};
        }
    }

    return Token{TokenType::END_OF_FILE, std::nullopt};
}

auto test_lex(std::string path) -> void {
    std::ifstream file{path};
    assert(file.is_open());

    std::string source_text;
    std::string line;

    while (getline(file, line)) {
        source_text += line + '\n';
    }

    Lexer lexer{source_text};

    while (1) {
        lexer.cur_token = lexer.get_token();

        std::cout << lexer.cur_token.to_string() << '\n';
        if (lexer.cur_token.type == TokenType::END_OF_FILE) break;
    }

}


int main() {
    // test_lex("../test/sample_program.c");
    test_lex("../test/sample_tokens.txt");

}







