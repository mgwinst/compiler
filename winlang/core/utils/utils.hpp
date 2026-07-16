#pragma once

#include <array>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <iterator>
#include <print>

inline void error_exit(std::string msg)
{
    std::println("{}", msg);
    exit(EXIT_FAILURE);
};

struct Module
{
    std::string file_path;
    std::string data;
};

[[nodiscard]] inline Module get_module(const std::string& file_path)
{
    if (!std::filesystem::exists(file_path)) {
        error_exit(std::format("File: {} does not exist", file_path));
    }

    std::ifstream file{file_path, std::ios::binary};
    if (!file.is_open()) {
        error_exit(std::format("Failed to open file {}", file_path));
    }

    // mmap instead?
    auto data = std::string{std::istreambuf_iterator<char>{ file }, std::istreambuf_iterator<char>{ }};

    return {file_path, data};
}

template<typename... Lambdas>
struct Overload : Lambdas... 
{ 
    using Lambdas::operator()...; 
};

template <typename T, typename... Ts>
constexpr auto combine(T first, Ts... rest)
{
    return std::array<T, 1 + sizeof...(Ts)>{first, rest...};
}

template <typename T>
auto prepend(const T& first, std::vector<T> rest)
{
    std::vector<T> result;
    result.reserve(1 + rest.size());
    result.push_back(first);
    result.append_range(std::move(rest));
    return result;
}

template <typename T>
inline constexpr bool always_false_v = false;

void swap_pop(std::ranges::contiguous_range auto& container, auto it)
{
    if (it >= container.begin() && it < container.end()) {
        std::swap(*it, container.back());
        container.pop_back();
    }
}

void swap_pop(std::ranges::contiguous_range auto& container, std::size_t index)
{
    if (index >= 0 && index < container.size()) {
        std::swap(container[index], container.back());
        container.pop_back();
    }
}