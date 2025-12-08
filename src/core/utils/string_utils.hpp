#pragma once

#include <cstdint>
#include <optional>
#include <string_view>
#include <charconv>
#include <print>

#include "utils/concepts.hpp"

template <Numeric T>
std::optional<T> sv_to_numeric(std::string_view str)
{
    if (str.empty()) 
        return std::nullopt;   

    T value = 0;
    auto [ptr, ec] = std::from_chars(str.begin(), str.end(), value);
 
    if (ec == std::errc{} && ptr == str.end())
        return value;

    return std::nullopt;
}