#pragma once

#include <expected>
#include <tuple>

#include "error.hpp"
#include "../lexer/lexer.hpp"
#include "../ast/ast.hpp"

#define CONST (true)
#define NON_CONST (false)

struct Parser 
{
    SourceFile source_file_;
    ParseDiagnostics diagnostics_;
    Lexer lexer_;
    Token prev_token_, cur_token_;
    AST ast_;

    Parser(SourceFile& source_file) noexcept;

    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;
    Parser(Parser&&) = delete;
    Parser& operator=(Parser&&) = delete;
    ~Parser() = default;

    const Token next_token() const noexcept;
    void eat_token() noexcept;
    bool is_cur_token(const TokenType token_type) const noexcept;
    bool is_next_token(const TokenType token_type) const noexcept;
    void panic(const ParseError& error) noexcept;
    
    std::expected<std::string_view, ParseError> match(TokenType token_type) noexcept;

    auto expect(std::same_as<TokenType> auto... token_types) noexcept
    {
        return std::tuple{ match(token_types)... };
    }

    std::expected<ASTNodeRef, ParseError> parse_compilation_unit() noexcept;
    std::expected<ASTNodeRef, ParseError> parse_decl() noexcept;
    std::expected<ASTNodeRef, ParseError> parse_var_decl(bool is_const) noexcept;
    std::expected<ASTNodeRef, ParseError> parse_param_decl() noexcept;
    std::expected<ASTNodeRef, ParseError> parse_func_decl() noexcept;
    std::expected<ASTNodeRef, ParseError> parse_struct(bool is_const) noexcept;
    std::expected<ASTNodeRef, ParseError> parse_struct_decl(bool is_const) noexcept;
    std::expected<ASTNodeRef, ParseError> parse_struct_def() noexcept;
    std::expected<ASTNodeRef, ParseError> parse_field() noexcept;
    std::expected<ASTNodeRef, ParseError> parse_compound_stmt() noexcept;
    std::expected<ASTNodeRef, ParseError> parse_while_loop() noexcept;
    std::expected<ASTNodeRef, ParseError> parse_for_loop() noexcept;
    std::expected<ASTNodeRef, ParseError> parse_if_stmt() noexcept;
    std::expected<ASTNodeRef, ParseError> nud(const Token token) noexcept;
    std::expected<ASTNodeRef, ParseError> led(const Token token, ASTNodeRef left) noexcept;
    std::expected<ASTNodeRef, ParseError> parse_expr(int min_prec = 0) noexcept;
    std::expected<ASTNodeRef, ParseError> parse_init_list_expr() noexcept;
};
