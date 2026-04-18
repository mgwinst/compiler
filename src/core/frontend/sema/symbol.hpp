#pragma once

#include <format>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../utils/alias.hpp"
#include "../../utils/enums.hpp"

// should the symbol or symbol table handle type info queries?

struct Symbol
{
    std::string identifier_;
    SymbolKind kind_;
    TypeID type_id_;
    StorageClass storage_;
    Linkage linkage_;
    
    // these should be part of the type instead
    // uint64_t size_;
    // uint64_t alignment_;

    auto to_string() const 
    {
        return std::format("<Symbol: [IDentifier: '{}'], [Type: {}]>", identifier_, type_id_);
    }
};

template <typename T>
concept ContainsName = requires (T t) {
    t.name_;
};

struct SymbolTable
{
    SymbolID insert(ContainsName auto& node, TypeID type_id) noexcept
    {
        Symbol symbol{ 
            .identifier_ = node.name_,
            .type_id_ = type_id,
            .storage_ = StorageClass::Auto,
            .linkage_ = Linkage::External,
        };
        
        auto id = symbol_pool_.size();
        symbol_pool_.push_back(symbol);
        
        cur_scope().emplace(node.name_, id);

        return static_cast<SymbolID>(id);
    }

    bool exists_in_scope(const std::string& ident) noexcept;
    SymbolID lookup(const std::string& ident) noexcept;
    void enter_scope() noexcept;
    void exit_scope() noexcept;
    const Symbol& get_symbol(SymbolID symbol_ref) const noexcept;
    Symbol& get_symbol(SymbolID symbol_ref) noexcept;

    std::unordered_map<std::string, SymbolID>& cur_scope() noexcept;

    std::vector<std::unordered_map<std::string, SymbolID>> scopes_;
    std::vector<Symbol> symbol_pool_;
};