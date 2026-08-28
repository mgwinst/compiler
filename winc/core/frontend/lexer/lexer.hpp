#pragma once

#include "frontend/translation_unit.hpp"
#include "frontend/lexer/token.hpp"

struct Lexer
{
    std::string_view source_;
    std::string_view::iterator cur_, src_ln_beg_, src_ln_end_;
    std::size_t line_num_, col_num_;

    explicit Lexer(TranslationUnit& translation_unit) noexcept;

    [[nodiscard]] Token get_token() noexcept;
    [[nodiscard]] Token peek_token() const noexcept;
    void find_src_line_bounds(const char* ch) noexcept;
};
