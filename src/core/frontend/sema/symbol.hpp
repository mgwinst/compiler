#pragma once

#include <format>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../utils/alias.hpp"
#include "../../utils/enums.hpp"

namespace Sema
{
    // where will these go? size, alignment, offset (layout related info), is_const, 
    // (declaring scope useful?), linkage, storage type...

    // after sem, the symbol becomes semantic backbone

    struct Symbol
    {
        std::string identifier_;
        SymbolKind kind_;
        TypeId type_id_;
        StorageClass storage_;
        Linkage linkage_;
        // size
        // offset

        auto to_string() const 
        {
            return std::format("<Symbol: [Identifier: '{}'], [Type: {}]>", identifier_, type_id_);
        }
    };

    template <typename T>
    concept HasName = requires (T t) {
        t.name_;
    };

    struct SymbolTable
    {
        std::vector<std::unordered_map<std::string, SymbolId>> scopes_;
        std::vector<Symbol> symbol_pool_;

        SymbolId insert(const HasName auto& node, TypeId type_id) noexcept
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

            return static_cast<SymbolId>(id);
        }

        bool exists_in_scope(const std::string& ident) noexcept;
        SymbolId lookup(const std::string& ident) noexcept;
        std::unordered_map<std::string, SymbolId>& cur_scope() noexcept;
        void enter_scope() noexcept;
        void exit_scope() noexcept;
        Symbol& get_symbol(SymbolId symbol_ref) noexcept;
        void print() const noexcept;
    };

} // namespace Sema