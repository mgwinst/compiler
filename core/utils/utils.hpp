#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <iterator>
#include <print>
#include <ranges>

struct SourceFile
{
    std::string file_path;
    std::string data;
};

[[nodiscard]] inline SourceFile get_source_file(const std::string& file_path)
{
    if (!std::filesystem::exists(file_path)) {
        std::println(std::cerr, "File: {} does not exist", file_path);
        exit(EXIT_FAILURE);
    }

    std::ifstream file{file_path, std::ios::binary};
    if (!file.is_open()) {
        std::println(std::cerr, "Failed to open file {}", file_path);
        exit(EXIT_FAILURE);
    }

    auto data = std::string{std::istreambuf_iterator<char>{ file }, std::istreambuf_iterator<char>{}};

    return {file_path, data};
}

enum class Constness
{
    CONST,
    MUTABLE,
    NON_CONST = MUTABLE
};

template<typename... Lambdas>
struct Overload : Lambdas... 
{ 
    using Lambdas::operator()...; 
};
