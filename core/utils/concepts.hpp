#pragma once

#include <concepts>
#include <string>

template <typename T>
concept StringLike = std::convertible_to<T, std::string>;

template <typename T>
concept Contiguous = std::ranges::contiguous_range<T>;