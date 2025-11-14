#pragma once

#include "ast.h"

struct Parser {
    Lexer lexer;
    
    Parser(std::string source_text) : lexer{source_text} {}
};

struct CompilationUnit
{
    std::vector<AST::FuncDecl> functions;
    std::vector<AST::StructDecl> structs;
    std::vector<AST::GlobalVariableDecl> global_vars;

    // global symbol table for compilation unit
    std::unordered_map<std::string, uint32_t> func_idxs;
    std::unordered_map<std::string, uint32_t> struct_idxs;
    std::unordered_map<std::string, uint32_t> global_var_idxs;
};

void build_symbol_table(CompilationUnit& unit)
{
    for (size_t i = 0; i != unit.functions.size(); i++) {
        auto ident = unit.functions.at(i).ident;
        unit.func_idxs[ident] = i;
    }

    for (size_t i = 0; i != unit.structs.size(); i++) {
        auto ident = unit.structs.at(i).ident;
        unit.struct_idxs[ident] = i;
    }

    for (size_t i = 0; i != unit.global_vars.size(); i++) {
        auto ident = unit.global_vars.at(i).ident;
        unit.global_var_idxs[ident] = i;
    }
}
