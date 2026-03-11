#pragma once

#include <string>
#include <format>
#include <variant>

#include "source_location.hpp"
#include "../utils/utils.hpp"

inline std::string format_source(const SourceLoc src_loc)
{
    const auto d = (size_t)std::distance(src_loc.source_line_.data(), src_loc.item_.data());
    const auto underline = std::string(d, ' ') + std::string(src_loc.item_.length(), '^');
    return std::format("    {}\n    {}", src_loc.source_line_, underline);
}

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

    RedefinitionError(std::string_view err_msg, SourceLoc source_loc) :
        msg_{ std::format("{}:{}: error: {}\n{}\n",
            source_loc.line_number_,
            source_loc.column_number_,
            err_msg,
            format_source(source_loc)) } {}
};


struct UndeclaredIdentiferError
{
    std::string msg_;

    UndeclaredIdentiferError(std::string_view err_msg, SourceLoc source_loc) :
        msg_{ std::format("{}:{}: error: {}\n{}\n", 
            source_loc.line_number_,
            source_loc.column_number_,
            err_msg, 
            format_source(source_loc)) } {}

};

struct TypeError
{
    std::string msg_;

    TypeError(std::string_view err_msg) : 
        msg_{ err_msg } {}

    TypeError(std::string_view err_msg, SourceLoc source_loc) : 
        msg_{ std::format("{}:{}: error: {}\n{}\n", 
            source_loc.line_number_,
            source_loc.column_number_,
            err_msg, 
            format_source(source_loc)) } {}
};


using Error = std::variant<SyntaxError, RedefinitionError, UndeclaredIdentiferError, TypeError>;

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

        [](const UndeclaredIdentiferError& undecl_ident_error) {
            return std::format("{}", undecl_ident_error.msg_);
        },

        [](const TypeError& type_error) {
            return std::format("{}", type_error.msg_);
        }

    ), error);
}
