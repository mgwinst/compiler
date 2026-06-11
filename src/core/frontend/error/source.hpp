#pragma once

#include <string_view>
#include <cstdint>

#include "frontend/lexer/token.hpp"

struct Source
{
    uint64_t line_number_, column_number_;
    std::string_view item_, source_line_;

    Source() = default;

    Source(const Token& start, const Token& end) :
        line_number_{ start.line_number_ },
        column_number_{ start.column_number_ },
        item_{start.lexeme_.data(), end.lexeme_.data() + end.length_},
        source_line_{ start.source_line_ } {}

    Source(const Token& tok) :
        line_number_{ tok.line_number_ },
        column_number_{ tok.column_number_ },
        item_{ tok.lexeme_ },
        source_line_{ tok.source_line_ } {}
};

inline std::string format_source(const Source source)
{
    const auto d = (size_t)std::distance(source.source_line_.data(), source.item_.data());
    const auto underline = std::string(d, ' ') + std::string(source.item_.length(), '^');
    return std::format("    {}\n    {}", source.source_line_, underline);
}