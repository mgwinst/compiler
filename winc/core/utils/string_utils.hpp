#pragma once

#include <optional>
#include <string_view>
#include <charconv>

template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

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

inline std::string_view trim(const char* start, const char* end)
{
    std::string_view sv{start, end};
    constexpr auto ws = " \t\n\r\f\v";

    auto b = sv.find_first_not_of(ws);
    if (b == std::string_view::npos)
        return {};

    auto e = sv.find_last_not_of(ws);
    return sv.substr(b, e - b + 1);
}

inline std::string_view trim(std::string_view sv)
{
    constexpr auto ws = " \t\n\r\f\v";

    auto b = sv.find_first_not_of(ws);
    if (b == std::string_view::npos)
        return {};

    auto e = sv.find_last_not_of(ws);
    return sv.substr(b, e - b + 1);
}
