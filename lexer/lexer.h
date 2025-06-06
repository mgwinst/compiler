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

    ADD,
    SUBTRACT,
    MULTIPLY,
    DEREFERENCE = MULTIPLY,
    DIVIDE,
    MODULUS,
    INCREMENT,
    DECREMENT,

    ASSIGN,
    ADD_ASSIGN,
    SUBTRACT_ASSIGN,
    MULTIPLY_ASSIGN,
    DIVIDE_ASSIGN,

    EQUALS,
    NOT_EQUAL,
    LESS_THAN,
    GREATER_THAN,
    LESS_EQUAL,
    GREATER_EQUAL,

    LOGICAL_AND,
    LOGICAL_OR,
    LOGICAL_NOT,

    LPAREN,
    RPAREN,
    LBRACKET,
    RBRACKET,
    LBRACE,
    RBRACE,
    SINGLE_QUOTE,
    DOUBLE_QUOTE,
    SEMICOLON,
    COLON,
    COMMA,
    DOT,
    PIPE,

    ADDRESS_OF,
    ARROW,

    COMMENT,
    END_OF_FILE,
    INVALID,   

    // handle bitwise later
};

struct Token {
    TokenType type;
    std::optional<std::string_view> lexeme;
    std::size_t line_number;
    std::size_t column_number;
    std::size_t length;

    Token(TokenType t, std::optional<std::string_view> l, std::size_t ln, std::size_t cn, std::size_t len) :
        type{t}, lexeme{l}, line_number{ln}, column_number{cn}, length{len} {}

    ~Token() {}

    std::string to_string() const {
        return std::format("[{}] {}:{} {} ", static_cast<int>(type), line_number, column_number, (lexeme.has_value() ? lexeme.value() : ""));
    }

    std::string to_string_extra() const {
        return std::format("[{}] {} ", static_cast<int>(type), line_number, column_number, (lexeme.has_value() ? lexeme.value() : ""));
    }
};

inline char peekchar(const char*);
[[nodiscard]] std::vector<Token> lex(std::string_view);

void test_lex(std::string);
