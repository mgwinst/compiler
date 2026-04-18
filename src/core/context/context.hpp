#pragma once

#include "../frontend/sema/types/type_pool.hpp"
#include "../frontend/sema/symbol.hpp"
#include "../frontend/error/diagnostics.hpp"
#include "../utils/utils.hpp"

struct CompilerContext
{
    std::vector<SourceFile> source_files_;
    // std::vector<ModuleContext> modules_;

    CompilerContext(std::vector<std::string> files)
    {
        for (const auto& file : files)
            source_files_.push_back(get_source_file(file));
    }
};

template <typename T>
concept ContainsSymbol = requires(T t){
    t.symbol_id_;
};

struct ModuleContext
{
    TypePool type_pool_;
    SymbolTable symbol_table_;
    Diagnostics diagnostics_;

    const Symbol& get_symbol(ContainsSymbol auto& node) const
    {
        return symbol_table_.get_symbol(node.symbol_id_);
    }

    Symbol& get_symbol(ContainsSymbol auto& node) 
    {
        return symbol_table_.get_symbol(node.symbol_id_);
    }
};