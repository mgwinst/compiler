#pragma once

#include <expected>

#include "lexer/lexer.hpp"
#include "parser/ast.hpp"
#include "parser/error.hpp"

// enum class ParseContext { Global, Function, Statement, Expression };

int lbp(const Token& token) noexcept;

struct Parser 
{
    SourceFile source_file_;
    Lexer lexer_;
    Token cur_token_, next_token_;
    ParseDiagnostics diagnostics_;
    AST ast_;

    Parser(SourceFile& source_file) noexcept;

    void eat_token() noexcept;
    bool is_cur_token(TokenType token_type) const noexcept;
    bool is_next_token(TokenType token_type) const noexcept;
    // bool is_kth_token(TokenType token_type, int k) const noexcept;
    void panic(const ParseError& error) noexcept;

    DeclRef parse_compilation_unit() noexcept;

    
    std::expected<DeclRef, ParseError> parse_const_decl() noexcept; // have a dispatcher to var vs func
    std::expected<DeclRef, ParseError> parse_var_decl() noexcept;
    std::expected<DeclRef, ParseError> parse_const_var_decl() noexcept;
    std::expected<DeclRef, ParseError> parse_param_decl() noexcept;
    std::expected<DeclRef, ParseError> parse_func_decl(bool is_const) noexcept;
    std::expected<DeclRef, ParseError> parse_struct_decl() noexcept;
    
    std::expected<ExprRef, ParseError> nud(const Token& token);
    std::expected<ExprRef, ParseError> led(const Token& token, const ExprRef left);

    std::expected<ExprRef, ParseError> parse_expr(int min_prec) noexcept;
    
};
