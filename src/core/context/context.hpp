#pragma once

#include "../frontend/sema/types/type_pool.hpp"
#include "../frontend/sema/symbol.hpp"
#include "../frontend/error/diagnostics.hpp"
#include "../utils/utils.hpp"

struct CompilerContext
{
    std::vector<SourceFile> source_files_;
    // std::vector<ModuleContext> modules_;
    // set<flags> ; if set then activate in pipeline

    CompilerContext(std::vector<std::string> files, std::vector<std::string> flags)
    {
        for (const auto& file : files)
            source_files_.push_back(get_source_file(file));
    }
};

template <typename T>
concept ContainsSymbol = requires(T t){
    t.symbol_id_;
};

template <typename T>
concept ContainsTypeID = requires(T t){
    t.type_id_;
};

struct ModuleContext
{
    TypePool type_pool_;
    SymbolTable symbol_table_;
    Diagnostics diagnostics_;

    const Symbol& get_symbol(ContainsSymbol auto& node) const { return symbol_table_.get_symbol(node.symbol_id_); }
    Symbol& get_symbol(ContainsSymbol auto& node) { return symbol_table_.get_symbol(node.symbol_id_); }
    const Symbol& get_symbol(SymbolID symbol_id) const { return symbol_table_.get_symbol(symbol_id); }
    Symbol& get_symbol(SymbolID symbol_id) { return symbol_table_.get_symbol(symbol_id); }
    
    const Type& get_type(ContainsTypeID auto& node) const { return type_pool_.get_type(node.type_id_); }
    Type& get_type(ContainsTypeID auto& node) { return type_pool_.get_type(node.type_id_); }
    const Type& get_type(TypeID type_id) const { return type_pool_.get_type(type_id); }
    Type& get_type(TypeID type_id) { return type_pool_.get_type(type_id); }
};