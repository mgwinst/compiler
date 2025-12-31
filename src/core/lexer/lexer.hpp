#pragma once

#include <print>

#include "token.hpp"

struct Lexer {
    std::string_view source_;
    std::string_view::iterator cur_;
    std::size_t line_num_, col_num_;

    Lexer(std::string_view source) noexcept: 
        source_{ source }, 
        cur_{ source.begin() }, 
        line_num_{ 1 }, 
        col_num_{ 1 } {}

    [[nodiscard]] Token get_token() noexcept;
    [[nodiscard]] Token peek_token() const noexcept;
};