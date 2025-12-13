#pragma once

#include <expected>
#include <tuple>

#include "lexer/lexer.hpp"
#include "ast/ast.hpp"
#include "parser/error.hpp"

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

    std::expected<NodeRef, ParseError> parse_compilation_unit() noexcept;
    std::expected<NodeRef, ParseError> parse_var_decl(Constness constness) noexcept;
    std::expected<NodeRef, ParseError> parse_param_decl() noexcept;
    std::expected<NodeRef, ParseError> parse_func_decl(Constness constness) noexcept;
    std::expected<NodeRef, ParseError> parse_compound_stmt() noexcept;
    std::expected<NodeRef, ParseError> parse_struct(Constness constness) noexcept;
    std::expected<NodeRef, ParseError> parse_struct_decl(Constness constness) noexcept;
    std::expected<NodeRef, ParseError> parse_struct_def() noexcept;
    std::expected<NodeRef, ParseError> parse_decl() noexcept;
    std::expected<NodeRef, ParseError> parse_field() noexcept;
    std::expected<NodeRef, ParseError> nud(const Token token) noexcept;
    std::expected<NodeRef, ParseError> led(const Token token, NodeRef left) noexcept;
    std::expected<NodeRef, ParseError> parse_expr(int min_prec = 0) noexcept;
    std::expected<NodeRef, ParseError> parse_init_list_expr() noexcept;
};
