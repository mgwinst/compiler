#include <ranges>

#include "symbol.hpp"

bool SymbolTable::exists_in_scope(const std::string& identifier) noexcept
{
    return cur_scope().contains(identifier);           
}       

SymbolID SymbolTable::lookup(const std::string& identifier) noexcept {
    for (auto& scope : scopes_ | std::views::reverse) {
        if (scope.contains(identifier)) {
            return scope[identifier];
        }
    }

    return static_cast<SymbolID>(-1);
}

std::unordered_map<std::string, SymbolID>& SymbolTable::cur_scope() noexcept
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

const Symbol& SymbolTable::get_symbol(SymbolID id) const noexcept
{
    return symbol_pool_[id];
}

Symbol& SymbolTable::get_symbol(SymbolID id) noexcept
{
    return symbol_pool_[id];
}