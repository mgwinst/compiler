#pragma once

#include <string_view>

#include "../lexer/token.hpp"

struct SourceSpan
{
    std::string_view str;
    std::size_t line_number_, column_number_;

    SourceSpan(Token start, Token end) :
        str{start.lexeme_.data(), end.lexeme_.data()},
        line_number_{ start.line_number_ },
        column_number_{ start.column_number_ } {}
};