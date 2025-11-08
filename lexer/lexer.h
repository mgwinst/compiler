#pragma once

#include <cstdint>
#include <utility>
#include <string>
#include <print>

#include "token.h"

struct Token {
    TokenType type;
    std::optional<std::string_view> lexeme;
    std::size_t line_number, column_number, length;

    Token() = default;
    Token(TokenType t, std::optional<std::string_view> l, std::size_t ln, std::size_t cn, std::size_t len) :
        type{t}, lexeme{l}, line_number{ln}, column_number{cn}, length{len} {}
    Token(TokenType t, std::optional<std::string_view> l) : type{t}, lexeme{l}, line_number{0}, column_number{0}, length{0} {}
    Token(TokenType t) : type{t}, lexeme{std::nullopt}, line_number{0}, column_number{0}, length{0} {}
    ~Token() = default;

    auto to_string() const -> std::string 
    {
        return std::format("[{}] {}:{} {} ", static_cast<int>(type), line_number, column_number, (lexeme.has_value() ? lexeme.value() : ""));
    }

    auto to_string_less() const -> std::string 
    {
        return std::format("{}", (lexeme.has_value() ? lexeme.value() : ""));
    }
};

struct Lexer {
    std::string_view source;
    std::string_view::iterator cur;
    std::size_t line_num, col_num;
    Token cur_token;

    Lexer(std::string_view src) : source{src}, cur{src.begin()}, line_num{1}, col_num{1}, cur_token{} {}

    [[nodiscard]] auto get_token() -> Token;
    [[nodiscard]] auto peek_token() -> Token;
};