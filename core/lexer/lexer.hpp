#pragma once

#include <cstdint>
#include <utility>
#include <string>
#include <print>

#include "lexer/token.hpp"

struct Lexer {
    std::string_view source;
    std::string_view::iterator cur;
    std::size_t line_num, col_num;
    Token cur_token;

    Lexer(std::string_view src) : source{src}, cur{src.begin()}, line_num{1}, col_num{1}, cur_token{} {}

    [[nodiscard]] auto get_token() -> Token;
    [[nodiscard]] auto peek_token() -> Token;
};