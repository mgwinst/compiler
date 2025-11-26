#pragma once

#include <concepts>
#include <string>

template <typename T>
concept StringConvertible = std::convertible_to<T, std::string>;