#include <ranges>

#include "symbol.hpp"

Symbol* SymbolTable::insert(Syntax::Decl* node, Type* type)
{
    auto* symbol = arena_.emplace<Symbol>(node->name_, type);

    cur_scope().emplace(node->name_, symbol);

    return symbol;
}

bool SymbolTable::exists_in_scope(const std::string& identifier)
{
    return cur_scope().contains(identifier);
}       

Symbol* SymbolTable::lookup(const std::string& identifier) {
    for (auto& scope : std::views::reverse(scopes_)) {
        if (scope.contains(identifier)) {
            return scope[identifier];
        }
    }

    return nullptr;
}

std::unordered_map<std::string, Symbol*>& SymbolTable::cur_scope()
{ 
    assert(!scopes_.empty());
    return scopes_.back();
}

void SymbolTable::enter_scope()
{
    scopes_.emplace_back();
}

void SymbolTable::exit_scope()
{
    assert(!scopes_.empty());
    scopes_.pop_back();
}