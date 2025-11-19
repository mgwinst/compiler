#pragma once

#include <string>
#include <fstream>
#include <filesystem>

[[nodiscard]] inline auto get_source_text(const std::string file_path) -> std::string {
    if (!std::filesystem::exists(file_path)) {
        throw std::runtime_error("File: " + file_path + " does not exist" + '\n');
    }

    std::ifstream file{ file_path };
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + file_path + '\n');
    }

    std::string source_text, line;
    while (getline(file, line)) { 
        source_text += line + '\n';
    }

    return source_text;
}

// std::visit
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };