#pragma once

#include <ranges>

template <typename T>
auto static_cast_view(auto&& range)
{
    return std::forward<decltype(range)>(range) | std::views::transform([](auto* p) { return static_cast<T*>(p); });
}