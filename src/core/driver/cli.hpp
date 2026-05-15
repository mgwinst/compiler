#pragma once

// winc main.w file1.w file2.w ...
// winc -ir -ast main.w

#include <span>
#include <string_view>
#include <cstdint>
#include <filesystem>
#include <ranges>
#include <set>
#include <vector>
#include <algorithm>

#include "../context/context.hpp"

namespace fs = std::filesystem;

inline std::unordered_set<std::string> winc_flags {
    "-ast",
    "-ir"
};

inline void print_winc_help()
{
    std::println("winc: the winlang compiler\n");
    std::println("usage: winc [-options] [file1.w file2.w ...]\n");
    std::println("options:");
    std::println("  [-ast] -> dump AST");
    std::println("  [-ir] -> dump IR");
    std::println("  [-h] -> help");
}

inline CompilerContext parse_command(int argc, const char** argv)
{
    auto args = std::span<const char*>{argv, static_cast<size_t>(argc)} 
              | std::views::drop(1) 
              | std::ranges::to<std::unordered_set<std::string>>();

    if (args.empty()) {
        std::println("winc: error -> no input files");
        std::exit(1);
    }

    if (args.contains("-h")) {
        print_winc_help();
        std::exit(1);
    }

    std::vector<std::string> files;
    std::vector<std::string> flags;

    uint32_t errors = 0;
    for (auto arg : args) {
        if (arg.ends_with(".w")) {
            if (fs::exists(arg)) {
                files.push_back(arg);
            } else {
                std::println(std::cerr, "winc: error -> '{}' file not found ", arg);
                ++errors;
            }
        } else if (winc_flags.contains(arg)) {
            flags.push_back(arg);
        } else {
            std::println(std::cerr, "winc: error -> '{}' unsupported argument", arg);
            ++errors;
        }
    }

    if (errors) {
        std::exit(1);
    }

    if (files.empty()) {
        std::println("winc: error -> no input files");
        std::exit(1);
    }

    return CompilerContext{std::move(files), std::move(flags)};
}
