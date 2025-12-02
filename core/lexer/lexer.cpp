#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <cassert>

#include "lexer/lexer.hpp"

[[nodiscard]] Token Lexer::get_token() noexcept
{
    auto start = cur_, end = cur_;

    while (cur_ != source_.end()) {
        // whitespace
        if (std::isspace(*cur_)) {
            if (*cur_ == '\n') {
                line_num_++;
                col_num_ = 1;
            } else {
                col_num_++;
            }
            cur_++;
        
        // digits
        } else if (std::isdigit(*cur_)) {
            start = cur_;
            auto start_col_num = col_num_;
            while (std::isdigit(*cur_)) {
                cur_++; 
                col_num_++;
            }
            end = cur_;
            return Token{TokenType::NUMERIC_LITERAL, std::string_view{start, end}, line_num_, start_col_num, static_cast<std::size_t>(end - start)};
        
        // names, types and keywords
        } else if (std::isalpha(*cur_)) {
            start = cur_;
            while (std::isalnum(*cur_) || *cur_ == '_') {
                cur_++; 
                col_num_++;
            }
            end = cur_;

            std::string_view value{start, end};

            if (auto kw = keywords.find(value); kw != keywords.end()) {
                return Token{kw->second, std::string_view{start, end}, line_num_, col_num_ - (end-start), static_cast<std::size_t>(end-start)};
            } else if (auto t = lang_types.find(value); t != lang_types.end()) {
                return Token{TokenType::TYPE, std::string_view{start, end}, line_num_, col_num_ - (end-start), static_cast<std::size_t>(end-start)};
            } else {
                return Token{TokenType::IDENTIFIER, std::string_view{start, end}, line_num_, col_num_ - (end-start), static_cast<std::size_t>(end-start)};
            }

        // symbols
        } else if (auto double_symbol = double_symbol_map.find(std::string_view{cur_, 2}); double_symbol != double_symbol_map.end()) {
            Token token{double_symbol->second, std::string_view{cur_, 2}, line_num_, col_num_, 2};
            cur_ += 2; 
            col_num_ += 2;
            return token;
        } else if (auto symbol = single_symbol_map.find(*cur_); symbol != single_symbol_map.end()) {
            return Token{symbol->second, std::string_view{cur_++, 1}, line_num_, col_num_++, 1};
        } else {
            return Token{TokenType::INVALID, std::string_view{cur_, 1}, line_num_, col_num_, 1};
        }
    }

    return Token{TokenType::END_OF_FILE, {}, line_num_, col_num_, 1};
}

[[nodiscard]] Token Lexer::peek_token() noexcept 
{
    auto peek_cur = cur_;
    auto start = peek_cur, end = peek_cur;

    while (peek_cur != source_.end()) {
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
        
        // names, types and keywords
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

    return Token{TokenType::END_OF_FILE, " "};
}









