#pragma once

#include <string>
#include <format>
#include <variant>

#include "source_location.hpp"
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

struct TypeMismatchError
{
    std::string msg_;

    TypeMismatchError(std::string_view msg) : 
        msg_{ msg } {}

    TypeMismatchError(std::string_view err_msg, SourceLoc source_loc) : 
        msg_{ std::format("{}:{}: error: {}\n{}\n", 
            source_loc.line_number_,
            source_loc.column_number_,
            err_msg, 
            format_source(source_loc)) } {}
};

struct InvalidArguments
{
    std::string msg_;

    InvalidArguments(std::string_view msg) : 
        msg_{ msg } {}

    InvalidArguments(std::string_view err_msg, SourceLoc source_loc) : 
        msg_{ std::format("{}:{}: error: {}\n{}\n", 
            source_loc.line_number_,
            source_loc.column_number_,
            err_msg, 
            format_source(source_loc)) } {}
};


using Error = std::variant<
    SyntaxError, 
    RedefinitionError, 
    UndeclaredIdentiferError, 
    TypeMismatchError, 
    InvalidArguments
>;

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

        [](const TypeMismatchError& type_error) {
            return std::format("{}", type_error.msg_);
        },

        [](const InvalidArguments& type_error) {
            return std::format("{}", type_error.msg_);
        }

    ), error);
}
