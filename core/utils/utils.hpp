#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <iterator>
#include <print>
#include <ranges>

[[nodiscard]] inline std::string get_source_text(const std::string& file_path)
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

    return std::string {
        std::istreambuf_iterator<char>{ file },
        std::istreambuf_iterator<char>{}
    };
}

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };