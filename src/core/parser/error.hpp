#pragma once

#include <string>
#include <format>
#include <print>
#include <vector>
#include <variant>
#include <iostream>

#include "../lexer/token.hpp"
#include "../utils/utils.hpp"

struct SyntaxError
{
    std::string msg;

    SyntaxError(const Token& token) :
        msg{ std::format("{}:{}: syntax error: unexpected symbol detected", token.line_number_, token.column_number_) } {}

    SyntaxError(const Token& token, std::string_view error_msg) :
        msg{ std::format("{}:{}: syntax error: {}", token.line_number_, token.column_number_, error_msg) } {}

    SyntaxError() = default;
    SyntaxError(const SyntaxError&) = default;
    SyntaxError& operator=(const SyntaxError&) = default;
    SyntaxError(SyntaxError&&) = default;
    SyntaxError& operator=(SyntaxError&&) = default;
    ~SyntaxError() = default;
};

// populate with more error types...
using ParseError = std::variant<SyntaxError>;
using ParseWarning = std::variant<int>;

template <typename T>
std::string error_to_string(const T& error)
{
    return std::visit(Overload(
        [](const SyntaxError& syntax_error) {
            return std::format("{}", syntax_error.msg);
        }
    ), error);
}

template <typename T>
std::string warning_to_string(const T& warning)
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
