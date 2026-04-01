#pragma once

#include <variant>
#include <string>
#include <format>

#include "source_location.hpp"
#include "../../utils/utils.hpp"

struct ImplicitConversionWarning
{
    std::string msg_;

    ImplicitConversionWarning(std::string_view err_msg, SourceLoc source_loc) :
        msg_{ std::format("{}:{}: warning: {}\n{}\n",
            source_loc.line_number_,
            source_loc.column_number_,
            err_msg,
            format_source(source_loc)) } {}
};

struct NarrowingConversionWarning
{
    std::string msg_;

    NarrowingConversionWarning(std::string_view err_msg, SourceLoc source_loc) :
        msg_{ std::format("{}:{}: warning: {}\n{}\n",
            source_loc.line_number_,
            source_loc.column_number_,
            err_msg,
            format_source(source_loc)) } {}
};

using Warning = std::variant<NarrowingConversionWarning>;

template <typename T>
std::string warning_to_string(const T& error)
{
    return std::visit(Overload(
        [](const ImplicitConversionWarning& warning) {
            return std::format("{}", warning.msg_);
        },

        [](const NarrowingConversionWarning& warning) {
            return std::format("{}", warning.msg_);
        }

    ), error);
}