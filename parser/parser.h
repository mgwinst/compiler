#pragma once

#include "ast.h"

/*
    struct CompilationUnit
    {
        std::string file_name;
        AST ast;

        (global symbol table)
    };

*/

// enum class ParseContext { TopLevel, Function, Statement, Expression }; // ctx

struct Parser 
{
    Lexer lexer;
    Token cur_token, next_token;
    AST ast;

    Parser(std::string source_text) : lexer{ source_text }, ast{ AST{} } {}

    void eat_token()
    {
        cur_token = lexer.get_token();
        next_token = lexer.peek_token();
    }

    void main_parse();

    DeclRef parse_var_decl();
    DeclRef parse_func_decl();
    DeclRef parse_struct_decl();

    StmtRef parse_return_statement();
    StmtRef parse_if_statement();
    StmtRef parse_while_statement();
    StmtRef parse_for_statement();

    ExprRef parse_expr();
    ExprRef parse_paren_expr();
    ExprRef parse_literal_expr();
};
