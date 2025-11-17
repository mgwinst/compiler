#pragma once

#include <string>
#include <format>
#include <print>
#include <vector>
#include <variant>
#include <iostream>

#include "lexer/token.hpp"
#include "utils/utils.hpp"

struct SyntaxError
{
    const std::string msg;

    SyntaxError(const Token& token) :
        msg{ std::format("{}:{}: syntax error: unexpected symbol detected", token.line_number, token.column_number) } {}
};

using ParseError = std::variant<SyntaxError>;
using ParseWarning = std::variant<int>;


template <typename T>
[[nodiscard]] auto error_to_string(const T& error) -> std::string
{
    return std::visit(overloaded(
        [](const SyntaxError& syntax_error) {
            return std::format("{}", syntax_error.msg);
        }
    ), error);
}

template <typename T>
[[nodiscard]] auto warning_to_string(const T& warning) -> std::string
{
    return std::format("");
}

struct ParseDiagnostics
{
    std::vector<ParseError> errors;
    std::vector<ParseWarning> warnings;

    void add_error(const ParseError& error);
    void add_warning(const ParseWarning& warning);

    void dump_errors();
    void dump_warnings();
};
