#include <print>
#include <ranges>

#include "symbol.hpp"

namespace Sema 
{
    bool SymbolTable::exists_in_scope(const std::string& ident) noexcept
    {
        return cur_scope().contains(ident);           
    }       

    SymbolRef SymbolTable::lookup(const std::string& ident) noexcept {
        for (auto& scope : scopes_ | std::views::reverse) {
            if (scope.contains(ident)) {
                return scope[ident];
            }
        }

        return -1;
    }

    std::unordered_map<std::string, SymbolRef>& SymbolTable::cur_scope() noexcept
    { 
        return scopes_.back();
    }

    void SymbolTable::enter_scope() noexcept
    {
        scopes_.push_back({ });
    }

    void SymbolTable::exit_scope() noexcept
    {
        scopes_.pop_back();
    }

    Symbol& SymbolTable::get_symbol(SymbolRef symbol_ref) noexcept
    {
        return symbol_pool_[symbol_ref];
    }

    void SymbolTable::print() const noexcept
    {
        for (const auto& symbol : symbol_pool_) {
            std::println("{}", symbol.to_string());
        }
    }

} // namespace Sema