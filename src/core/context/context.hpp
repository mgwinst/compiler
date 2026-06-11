#pragma once

#include "frontend/error/diagnostics.hpp"
#include "frontend/sema/types/type_table.hpp"
#include "frontend/sema/symbol.hpp"
#include "utils/utils.hpp"

struct ModuleContext
{
    TypeTable type_table_;
    SymbolTable symbol_table_;
    Diagnostics diagnostics_;
};

class CompilerContext
{
public:
    CompilerContext(std::vector<std::string> modules, std::vector<std::string> flags)
    {
        for (const auto& module : modules)
            modules_.push_back(get_module(module));

        for (const auto& flag : flags) {
            flags_.insert(flag);
        }
    }

    const auto& modules() const
    {
        return modules_;
    }

    const auto& flags() const
    {
        return flags_;
    }

    uint64_t module_count() const
    {
        return modules_.size();
    }

private:
    // std::vector<ModuleContext> modules_;

    std::vector<Module> modules_;
    std::unordered_set<std::string> flags_;
};