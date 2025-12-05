#pragma once

#include <expected>

#include "lexer/lexer.hpp"
#include "parser/ast.hpp"
#include "parser/error.hpp"

int lbp(const Token& token) noexcept;

struct Parser 
{
    SourceFile source_file_;
    Lexer lexer_;
    ParseDiagnostics diagnostics_;
    AST ast_;

    Parser(SourceFile& source_file) noexcept;

    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;
    Parser(Parser&&) = delete;
    Parser& operator=(Parser&&) = delete;
    ~Parser() = default;

    const Token& cur_token() const noexcept;
    void eat_token() noexcept;
    bool is_cur_token(const TokenType token_type) const noexcept;
    bool is_next_token(const TokenType token_type) const noexcept;
    void panic(const ParseError& error) noexcept;
    std::expected<std::string_view, ParseError> match(TokenType token_type) noexcept;
    
    auto expect(std::same_as<TokenType> auto... token_types)
    {
        return std::tuple{ match(token_types)... };
    }

    std::expected<DeclRef, ParseError> parse_compilation_unit() noexcept;
    std::expected<DeclRef, ParseError> parse_var_decl(Constness constness) noexcept;
    std::expected<DeclRef, ParseError> parse_param_decl() noexcept;
    std::expected<DeclRef, ParseError> parse_func_decl(Constness constness) noexcept;
    std::expected<DeclRef, ParseError> parse_struct_decl() noexcept;

    std::expected<ExprRef, ParseError> parse_compound_stmt() noexcept;
    std::expected<ExprRef, ParseError> nud(const Token& token);
    std::expected<ExprRef, ParseError> led(const Token& token, const ExprRef left);
    std::expected<ExprRef, ParseError> parse_expr(int min_prec) noexcept;
};
