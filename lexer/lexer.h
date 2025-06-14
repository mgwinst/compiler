#pragma once

#include <cstdint>
#include <utility>
#include <string>
#include <print>

enum class TokenType : uint16_t {
    IDENTIFIER = 128,

    NUMERIC_LITERAL,
    CHAR_LITERAL,
    STRING_LITERAL,

    TYPE,

    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,
    KEYWORD_FOR,
    KEYWORD_RETURN,
    KEYWORD_CONST,
    KEYWORD_TYPEDEF,
    
    EQUAL,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,
    LESS,
    GREATER,
    LPAREN,
    RPAREN,
    LBRACKET,
    RBRACKET,
    LBRACE,
    RBRACE,
    SINGLE_QUOTE,
    DOUBLE_QUOTE,
    AMPERSAND,
    BANG,
    SEMICOLON,
    COLON,
    COMMA,
    DOT,
    PIPE,
    ARROW,

    EQUAL_EQUAL,
    BANG_EQUAL,
    PLUS_EQUAL,
    MINUS_EQUAL,
    STAR_EQUAL,
    SLASH_EQUAL,
    PERCENT_EQUAL,
    LESS_EQUAL,
    GREATER_EQUAL,
    PLUS_PLUS,
    MINUS_MINUS,
    LESS_LESS,
    GREATER_GREATER,
    AMPERSAND_AMPERSAND,
    PIPE_PIPE,
    SLASH_SLASH,

    END_OF_FILE,
    INVALID,   

    // handle bitwise later
};

struct Token {
    TokenType type;
    std::optional<std::string_view> lexeme;
    std::size_t line_number, column_number, length;

    Token(TokenType t, std::optional<std::string_view> l, std::size_t ln, std::size_t cn, std::size_t len) :
        type{t}, lexeme{l}, line_number{ln}, column_number{cn}, length{len} {}
    Token(TokenType t, std::optional<std::string_view> l) : type{t}, lexeme{l}, line_number{0}, column_number{0}, length{0} {}
    Token(TokenType t) : type{t}, lexeme{std::nullopt}, line_number{0}, column_number{0}, length{0} {}

    ~Token() {}

    auto to_string() const -> std::string {
        return std::format("[{}] {}:{} {} ", static_cast<int>(type), line_number, column_number, (lexeme.has_value() ? lexeme.value() : ""));
    }

    auto to_string_less() const -> std::string {
        return std::format("{}", (lexeme.has_value() ? lexeme.value() : ""));
    }

};

struct Lexer {
    std::string_view source;
    std::string_view::iterator cur;
    std::size_t line_num, col_num;

    Lexer(std::string_view src) : source{src}, cur{src.begin()}, line_num{1}, col_num{1} {}

    [[nodiscard]] auto get_token() -> Token;
    [[nodiscard]] auto peek_token() -> Token;
};

auto test_lex(std::string) -> void;
