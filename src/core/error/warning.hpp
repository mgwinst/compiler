#pragma once

#include <variant>
#include <string>
#include <format>

struct ImplicitConversionWarning
{

};

using Warning = std::variant<ImplicitConversionWarning>;

template <typename T>
std::string warning_to_string(const T& warning)
{
    return std::format("");
}