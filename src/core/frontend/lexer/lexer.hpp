#pragma once

#include "token.hpp"

struct Lexer {
    std::string_view source_;
    std::string_view::iterator cur_, src_ln_beg_, src_ln_end_;
    std::size_t line_num_, col_num_;

    Lexer(std::string_view source) noexcept: 
        source_{ source }, 
        cur_{ source.begin() }, 
        line_num_{ 1 }, 
        col_num_{ 1 } 
    {
        find_src_line_bounds(cur_ - 1);
    }

    [[nodiscard]] Token get_token() noexcept;
    [[nodiscard]] Token peek_token() const noexcept;
    void find_src_line_bounds(const char* ch) noexcept;
};