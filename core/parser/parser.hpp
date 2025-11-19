#pragma once

#include <expected>

#include "parser/ast.hpp"
#include "parser/error.hpp"

enum class ParseContext { TopLevel, Function, Statement, Expression };

struct Parser 
{
    Lexer lexer_;
    Token cur_token_, next_token_;
    AST ast_;
    ParseContext ctx_;
    ParseDiagnostics diagnostics_;

    Parser(const std::string& source_text) :
        lexer_{ source_text }, ast_{ AST{} }, ctx_{ ParseContext::TopLevel } {}

    void eat_token();
    bool is_cur_token(TokenType token_type);
    bool is_next_token(TokenType token_type);
    void panic(const ParseError& error);

    void main_parse();

    std::expected<DeclRef, ParseError> parse_var_decl();
    std::expected<DeclRef, ParseError> parse_const_var_decl();
    std::expected<DeclRef, ParseError> parse_param_decl();
    std::expected<DeclRef, ParseError> parse_func_decl();
    std::expected<DeclRef, ParseError> parse_struct_decl();
    
    std::expected<ExprRef, ParseError> parse_primary_expr();
    std::expected<ExprRef, ParseError> parse_expr(int min_prec);
};
