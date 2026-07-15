#pragma once

#include <format>
#include <string>
#include <unordered_map>
#include <vector>

#include "frontend/ast/ast.hpp"
#include "frontend/sema/types/types.hpp"
#include "utils/enums.hpp"

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

struct Symbol
{
    std::string identifier_;
    Type* type_;
    StorageClass storage_;
    Linkage linkage_;

    Symbol(std::string_view identifier, Type* type, StorageClass storage = StorageClass::Auto, Linkage linkage = Linkage::External) :
        identifier_{ identifier },
        type_{ type },
        storage_{ storage },
        linkage_{ linkage } {}
};

struct SymbolTable
{
    SymbolTable() = default;
    SymbolTable(const SymbolTable&) = delete;
    SymbolTable& operator=(const SymbolTable&) = delete;
    SymbolTable(SymbolTable&&) = default;
    SymbolTable& operator=(SymbolTable&&) = default;
    ~SymbolTable() = default;

    Symbol* insert(Syntax::Decl* node, Type* type);
    Symbol* lookup(const std::string& ident);
    bool exists_in_scope(const std::string& ident);
    void enter_scope();
    void exit_scope();
    std::unordered_map<std::string, Symbol*>& cur_scope();

    std::vector<std::unordered_map<std::string, Symbol*>> scopes_;
    Arena arena_;
};