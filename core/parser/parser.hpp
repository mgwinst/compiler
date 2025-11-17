#pragma once

#include <expected>

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

    Parser(const std::string& source_text) :
        lexer{ source_text }, ast{ AST{} }, ctx{ ParseContext::TopLevel } {}

    void eat_token();
    bool is_cur_token(TokenType token_type);
    bool is_next_token(TokenType token_type);
    void panic(const ParseError& error);

    void main_parse();
    std::expected<Decl, ParseError> parse_var_decl();
    std::expected<Decl, ParseError> parse_const_var_decl();
    std::expected<Decl, ParseError> parse_func_decl();
    std::expected<Decl, ParseError> parse_struct_decl();
    std::expected<Decl, ParseError> parse_param_decl();
    std::expected<Expr, ParseError> parse_expr();
    std::expected<Expr, ParseError> parse_compound_stmt();
    std::expected<Expr, ParseError> parse_return_statement();
    std::expected<Expr, ParseError> parse_if_statement();
    std::expected<Expr, ParseError> parse_while_statement();
    std::expected<Expr, ParseError> parse_for_statement();
    std::expected<Expr, ParseError> parse_paren_expr();
    std::expected<Expr, ParseError> parse_literal_expr();
};
