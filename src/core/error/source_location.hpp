#pragma once

#include <string_view>

#include "../lexer/token.hpp"

struct SourceLoc
{
    uint64_t line_number_, column_number_;
    std::string_view item_, source_line_;

    SourceLoc() = default;

    SourceLoc(const Token& start, const Token& end) :
        line_number_{ start.line_number_ },
        column_number_{ start.column_number_ },
        item_{start.lexeme_.data(), end.lexeme_.data() + end.length_},
        source_line_{ start.source_line_ } {}

    SourceLoc(const Token& tok) :
        line_number_{ tok.line_number_ },
        column_number_{ tok.column_number_ },
        item_{ tok.lexeme_ },
        source_line_{ tok.source_line_ } {}
};