#pragma once

#include <ranges>
#include <string>
#include <unordered_map>

#include "types/types.hpp"

using SymbolRef = int64_t;

namespace Sema
{
    enum class SymbolKind
    {

    };

    enum class StorageClass
    {
        Auto,
        Static,
    };

    enum class Linkage
    {
        Internal,
        External,
        Weak,
        None
    };

    // where will these go? size, alignment, offset (layout related info), is_const, 
    // (declaring scope useful?), linkage, storage type...

    // after sem, the symbol becomes semantic backbone

    struct Symbol
    {
        std::string identifier_;
        SymbolKind kind_;
        TypeRef type_;
        StorageClass storage_;
        Linkage linkage_;
    };

    template <typename T>
    concept HasName = requires (T t) {
        t.name_;
    };

    struct SymbolTable
    {
        SymbolRef insert(const HasName auto& node, TypeRef type) noexcept
        {
            Symbol symbol{ 
                .identifier_ = node.name_,
                .type_ = type,
                .storage_ = StorageClass::Auto,
                .linkage_ = Linkage::External
            };
            
            auto idx = symbol_pool_.size();
            symbol_pool_.push_back(symbol);
            
            cur_scope().emplace(node.name_, idx);

            return idx;
        }

        bool exists_in_scope(const std::string& ident)
        {
            return cur_scope().contains(ident);           
        }       

        SymbolRef lookup(const std::string& ident) noexcept {
            for (auto& scope : scopes_ | std::views::reverse) {
                if (scope.contains(ident)) {
                    return scope[ident];
                }
            }

            return -1;
        }

        void enter_scope() noexcept
        {
            scopes_.push_back({ });
        }

        void exit_scope() noexcept
        {
            scopes_.pop_back();
        }

        std::vector<std::unordered_map<std::string, SymbolRef>> scopes_;
        std::vector<Symbol> symbol_pool_;

        std::unordered_map<std::string, SymbolRef>& cur_scope() 
        { 
            return scopes_.back();
        }
    };

} // namespace Sema