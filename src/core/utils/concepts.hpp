#pragma once

#include <concepts>
#include <string>

template <typename T>
concept StringLike = std::constructible_from<T, std::string>;

template <typename T>
concept Contiguous = std::ranges::contiguous_range<T>;