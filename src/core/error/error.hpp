#pragma once

#include <string>
#include <format>
#include <variant>

#include "source_span.hpp"
#include "../utils/utils.hpp"

struct SyntaxError
{
    std::string msg_;

    SyntaxError(const Token& token) :
        msg_{ std::format("{}:{}: syntax error: unexpected symbol detected", 
            token.line_number_, 
            token.column_number_) } {}

    SyntaxError(const Token& token, std::string_view err_msg) :
        msg_{ std::format("{}:{}: syntax error: {}", 
            token.line_number_, 
            token.column_number_, 
            err_msg) } {}
};

struct RedefinitionError
{
    std::string msg_;

    RedefinitionError(std::string_view err_msg, SourceSpan source_span) :
        msg_{ std::format("{}:{} error: {}\n    '{}'\n", 
            source_span.line_number_,
            source_span.column_number_,
            err_msg, 
            source_span.str) } {}
};

struct TypeError
{
    std::string msg_;

    TypeError(std::string_view err_msg) : 
        msg_{} {}

    TypeError(std::string_view err_msg, SourceSpan source_span) : 
        msg_{ std::format("{}:{} error: {}\n    '{}'\n", 
            source_span.line_number_,
            source_span.column_number_,
            err_msg, 
            source_span.str) } {}
};


using Error = std::variant<SyntaxError, RedefinitionError, TypeError>;

template <typename T>
std::string error_to_string(const T& error)
{
    return std::visit(Overload(
        [](const SyntaxError& syntax_error) {
            return std::format("{}", syntax_error.msg_);
        },

        [](const RedefinitionError& redef_error) {
            return std::format("{}", redef_error.msg_);
        },

        [](const TypeError& type_error) {
            return std::format("{}", type_error.msg_);
        }

    ), error);
}
