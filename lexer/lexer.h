#pragma once

#include <cstdint>
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

    ~Token() {}

    auto to_string() const -> std::string {
        return std::format("[{}] {}:{} {} ", static_cast<int>(type), line_number, column_number, (lexeme.has_value() ? lexeme.value() : ""));
    }

};

struct Lexer {
    [[nodiscard]] static auto get_token(std::string_view) -> Token;
    [[nodiscard]] static auto get_next_token(std::string_view) -> Token;
};

inline auto peekchar(const char* cur_char) -> char { return *cur_char; }

auto test_lex(std::string) -> void;
