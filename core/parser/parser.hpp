#pragma once

#include "parser/ast.hpp"
#include "parser/error.hpp"



enum class ParseContext { TopLevel, Function, Statement, Expression };

struct Parser 
{
    Lexer lexer;
    Token cur_token, next_token;
    AST ast;
    ParseContext ctx;
    ParseDiagnostics diagnostics;

    Parser(std::string source_text) : 
        lexer{ source_text }, ast{ AST{} }, ctx{ ParseContext::TopLevel } {}

    void eat_token();
    void expect(TokenType token_type);
    void panic_mode();

    void main_parse();
    std::optional<Decl> parse_var_decl();
    std::optional<Decl> parse_func_decl();
    std::optional<Decl> parse_struct_decl();
    std::optional<Decl> parse_param_decl();
    std::optional<Expr> parse_expr();
    std::optional<Expr> parse_compound_stmt();
    std::optional<Expr> parse_return_statement();
    std::optional<Expr> parse_if_statement();
    std::optional<Expr> parse_while_statement();
    std::optional<Expr> parse_for_statement();
    std::optional<Expr> parse_paren_expr();
    std::optional<Expr> parse_literal_expr();
};
