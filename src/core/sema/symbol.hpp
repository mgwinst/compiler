#pragma once

#include <boost/container/options.hpp>
#include <ranges>
#include <string>

#include "sema_context.hpp"
#include "types.hpp"

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

    using SymbolRef = int64_t;

    // symbol table manages scope state, it is up to the SemaTree walk to determine when to enter/exit scope.
    class SymbolTable
    {
    public:
        SymbolRef insert(const SyntaxTree::VarDecl& var, TypeRef type) noexcept
        {
            auto cur_scope = scopes.back();      
            
            Symbol symbol{ 
                .identifier_ = var.name_,
                .type_ = type,
                .storage_ = StorageClass::Auto,
                .linkage_ = Linkage::External
            };
            
            auto idx = symbol_pool.size();
            symbol_pool.push_back(symbol);
            cur_scope[var.name_] = idx;

            return idx;
        }

        SymbolRef search_current_scope(const std::string& ident)
        {
            auto cur_scope = scopes.back();

            if (auto symbol = cur_scope.find(ident); symbol != cur_scope.end()) {
                if (symbol->second)
            }
        }       

        SymbolRef lookup(const std::string& ident) noexcept {
            for (auto& scope : scopes | std::views::reverse) {
                if (scope.contains(ident)) {
                    return scope[ident];
                }
            }

            return -1;
        }

        void enter_scope() noexcept
        {
            scopes.push_back({ });
        }

        void exit_scope() noexcept
        {
            scopes.pop_back();
        }

    private:   
        std::vector<std::unordered_map<std::string, SymbolRef>> scopes;
        std::vector<Symbol> symbol_pool;
    };

    // symbol table manages scope state, it is up to the SemaTree walk to determine when to enter/exit scope.
    inline SymbolRef resolve_var_decl_symbol(SemaContext& sema_ctx, const Syntax::VarDecl& var)
    {
        auto& symbol_table = sema_ctx.symbol_table_;       

        SymbolRef symbol = symbol_table.lookup(var.name_);
    
        return symbol == -1 ? symbol_table.insert(var) : symbol;
    }

} // namespace Sema