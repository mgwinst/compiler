#pragma once

#include <thread>

#include "../frontend/sema/types/type_pool.hpp"
#include "../frontend/sema/symbol.hpp"
#include "../frontend/error/diagnostics.hpp"
#include "../utils/utils.hpp"
#include "../utils/print/print.hpp"

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