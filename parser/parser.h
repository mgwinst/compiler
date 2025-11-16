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

enum class ParseContext { TopLevel, Function, Statement, Expression };

struct Parser 
{
    Lexer lexer;
    Token cur_token, next_token;
    AST ast;
    ParseContext ctx;

    Parser(std::string source_text) : lexer{ source_text }, ast{ AST{} }, ctx{ ParseContext::TopLevel } {}

    void eat_token()
    {
        cur_token = lexer.get_token();
        next_token = lexer.peek_token();
    }

    void main_parse();

    Decl parse_var_decl();
    Decl parse_func_decl();
    Expr parse_compound_stmt();
    Expr parse_expr();
    
    /*
    Decl parse_struct_decl();

    Expr parse_return_statement();
    Expr parse_if_statement();
    Expr parse_while_statement();
    Expr parse_for_statement();
    Expr parse_paren_expr();
    Expr parse_literal_expr();

    */
};
