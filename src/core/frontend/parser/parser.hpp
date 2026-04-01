#pragma once

#include <expected>
#include <tuple>

#include "../lexer/lexer.hpp"
#include "../ast/ast.hpp"
#include "../error/diagnostics.hpp"

struct Parser 
{
    SourceFile& source_file_;
    Lexer lexer_;
    Token prev_token_, cur_token_;
    AST ast_;
    Diagnostics diagnostics_;

    Parser(SourceFile& source_file) noexcept;

    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;
    Parser(Parser&&) = delete;
    Parser& operator=(Parser&&) = delete;
    ~Parser() = default;

    AST run()
    {
        parse_compilation_unit();
        return std::move(ast_);
    }

    const Token next_token() const noexcept;
    void eat_token() noexcept;
    bool is_cur_token(const TokenType token_type) const noexcept;
    bool is_next_token(const TokenType token_type) const noexcept;
    void panic(const Error& error) noexcept;
    SourceLoc get_source_loc() noexcept;

    std::expected<std::string_view, Error> match(TokenType token_type) noexcept;

    auto expect(std::same_as<TokenType> auto... token_types) noexcept
    {
        return std::tuple{ match(token_types)... };
    }

    void parse_compilation_unit() noexcept;

    std::expected<ASTNodeID, Error> parse_decl() noexcept;
    std::expected<ASTNodeID, Error> parse_var_decl() noexcept;
    std::expected<ASTNodeID, Error> parse_param_decl() noexcept;
    std::expected<ASTNodeID, Error> parse_func_decl() noexcept;
    std::expected<ASTNodeID, Error> parse_struct_def() noexcept;
    std::expected<ASTNodeID, Error> parse_field() noexcept;
    std::expected<ASTNodeID, Error> parse_return_stmt() noexcept;
    std::expected<ASTNodeID, Error> parse_compound_stmt() noexcept;
    std::expected<ASTNodeID, Error> parse_while_loop() noexcept;
    std::expected<ASTNodeID, Error> parse_for_loop() noexcept;
    std::expected<ASTNodeID, Error> parse_if_stmt() noexcept;
    std::expected<ASTNodeID, Error> parse_expr(int min_prec = 0) noexcept;
    std::expected<ASTNodeID, Error> parse_init_list_expr() noexcept;
    std::expected<ASTNodeID, Error> parse_type() noexcept;
    std::expected<ASTNodeID, Error> nud(const Token token) noexcept;
    std::expected<ASTNodeID, Error> led(const Token token, ASTNodeID left) noexcept;
};