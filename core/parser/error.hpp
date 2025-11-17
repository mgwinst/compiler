#pragma once

#include <string>
#include <format>
#include <print>
#include <vector>
#include <variant>
#include <iostream>

#include "lexer/token.hpp"

struct SyntaxError
{
    std::string error;

    SyntaxError(const Token& token) :
        error{ std::format("{}:{}: syntax error: unexpected symbol detected", token.line_number, token.column_number) } {}
};

using ParseError = std::variant<SyntaxError>;
using ParseWarning = std::variant<int>;

struct ParseDiagnostics
{
    std::vector<ParseError> errors;
    std::vector<ParseWarning> warnings;

    void add_error(const ParseError& error);
    void add_warning(const ParseWarning& warning);

    void dump_errors();
    void dump_warnings();
};
