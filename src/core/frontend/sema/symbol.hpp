#pragma once

#include <format>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../utils/alias.hpp"
#include "../../utils/enums.hpp"

struct Symbol
{
    std::string identifier_;
    SymbolKind kind_;
    TypeID type_id_;
    StorageClass storage_;
    Linkage linkage_;
    // size
    // offset

    auto to_string() const 
    {
        return std::format("<Symbol: [IDentifier: '{}'], [Type: {}]>", identifier_, type_id_);
    }
};

template <typename T>
concept HasName = requires (T t) {
    t.name_;
};

struct SymbolTable
{
    std::vector<std::unordered_map<std::string, SymbolID>> scopes_;
    std::vector<Symbol> symbol_pool_;

    SymbolID insert(const HasName auto& node, TypeID type_id) noexcept
    {
        Symbol symbol{ 
            .identifier_ = node.name_,
            .type_id_ = type_id,
            .storage_ = StorageClass::Auto,
            .linkage_ = Linkage::External
        };
        
        auto id = symbol_pool_.size();
        symbol_pool_.push_back(symbol);
        
        cur_scope().emplace(node.name_, id);

        return static_cast<SymbolID>(id);
    }

    bool exists_in_scope(const std::string& ident) noexcept;
    SymbolID lookup(const std::string& ident) noexcept;
    std::unordered_map<std::string, SymbolID>& cur_scope() noexcept;
    void enter_scope() noexcept;
    void exit_scope() noexcept;
    const Symbol& get_symbol(SymbolID symbol_ref) const noexcept;
    Symbol& get_symbol(SymbolID symbol_ref) noexcept;
};