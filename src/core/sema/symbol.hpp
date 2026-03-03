#pragma once

#include <format>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

#include "../utils/alias.hpp"
#include "../utils/enums.hpp"

using SymbolRef = int64_t;

namespace Sema
{
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

        auto to_string() const 
        {
            return std::format("Symbol: [Identifier: '{}'], [Type: {}]]", identifier_, type_);
        }
    };

    template <typename T>
    concept HasName = requires (T t) {
        t.name_;
    };

    struct SymbolTable
    {
        std::vector<std::unordered_map<std::string, SymbolRef>> scopes_;
        std::vector<Symbol> symbol_pool_;

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

            return static_cast<SymbolRef>(idx);
        }

        bool exists_in_scope(const std::string& ident) noexcept;
        SymbolRef lookup(const std::string& ident) noexcept;
        std::unordered_map<std::string, SymbolRef>& cur_scope() noexcept;
        void enter_scope() noexcept;
        void exit_scope() noexcept;

        void print() const noexcept;
    };

} // namespace Sema