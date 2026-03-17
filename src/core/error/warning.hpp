#pragma once

#include <variant>
#include <string>
#include <format>

struct ImplicitConversionWarning
{

};

struct NarrowingConversionWarning
{

};

using Warning = std::variant<NarrowingConversionWarning>;

template <typename T>
std::string warning_to_string(const T& warning)
{
    return std::format("");
}