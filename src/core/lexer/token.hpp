#pragma once

#include <unordered_map>
#include <unordered_set>
#include <format>
#include <cstdint>
#include <string_view>

enum class TokenType : uint16_t
{
    IDENTIFIER = 128,
    NUMERIC_LITERAL,
    CHAR_LITERAL,
    STRING_LITERAL,
    TYPE,
    KEYWORD_FUNCTION,
    KEYWORD_STRUCT,
    KEYWORD_VAR,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,
    KEYWORD_FOR,
    KEYWORD_RETURN,
    KEYWORD_BREAK,
    KEYWORD_CONTINUE,
    KEYWORD_CONST,
    KEYWORD_TYPEDEF,
    KEYWORD_STATIC,
    KEYWORD_INLINE,
    KEYWORD_TRUE,
    KEYWORD_FALSE,
    KEYWORD_LAMBDA,
    KEYWORD_NULL,
    KEYWORD_SIZEOF,
    KEYWORD_ALIGNOF,
    LPAREN,
    RPAREN,
    LBRACKET,
    RBRACKET,
    LBRACE,
    RBRACE,
    EQUAL,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,
    LESS,
    GREATER,
    SINGLE_QUOTE,
    DOUBLE_QUOTE,
    AMPERSAND,
    BANG,
    CARROT,
    SEMICOLON,
    COLON,
    COMMA,
    DOT,
    PIPE,
    ARROW,
    TILDE,
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
    STAR_STAR,
    AMPERSAND_AMPERSAND,
    PIPE_PIPE,
    SLASH_SLASH,
    END_OF_FILE,
    INVALID,
};

inline const std::unordered_set<std::string_view> built_in_types {
    "int", "int8", "int16", "int32", "int64",
    "uint", "uint8", "uint16", "uint32", "uint64",
    "float", "float16", "float32", "float64",
    "byte", "bool", "void", "union", "enum",
};

inline const std::unordered_map<std::string_view, TokenType> keywords {
    {"fn",       TokenType::KEYWORD_FUNCTION},
    {"struct",   TokenType::KEYWORD_STRUCT},
    {"var",      TokenType::KEYWORD_VAR},
    {"if",       TokenType::KEYWORD_IF},
    {"else",     TokenType::KEYWORD_ELSE},
    {"while",    TokenType::KEYWORD_WHILE},
    {"for",      TokenType::KEYWORD_FOR},
    {"return",   TokenType::KEYWORD_RETURN},
    {"break",    TokenType::KEYWORD_BREAK},
    {"continue", TokenType::KEYWORD_CONTINUE},
    {"const",    TokenType::KEYWORD_CONST},
    {"typedef",  TokenType::KEYWORD_TYPEDEF},
    {"static",   TokenType::KEYWORD_STATIC},
    {"inline",   TokenType::KEYWORD_INLINE},
    {"true",     TokenType::KEYWORD_TRUE},
    {"false",    TokenType::KEYWORD_FALSE},
    {"lambda",   TokenType::KEYWORD_LAMBDA},
    {"null",     TokenType::KEYWORD_NULL},
    {"sizeof",   TokenType::KEYWORD_SIZEOF},
    {"alignof",  TokenType::KEYWORD_ALIGNOF},
};

inline const std::unordered_map<char, TokenType> single_symbol_map {
    {'=', TokenType::EQUAL},
    {'+', TokenType::PLUS},
    {'-', TokenType::MINUS},
    {'*', TokenType::STAR},
    {'/', TokenType::SLASH},
    {'%', TokenType::PERCENT},
    {'<', TokenType::LESS},
    {'>', TokenType::GREATER},
    {'(', TokenType::LPAREN},
    {')', TokenType::RPAREN},
    {'{', TokenType::LBRACE},
    {'}', TokenType::RBRACE},
    {'[', TokenType::LBRACKET},
    {']', TokenType::RBRACKET},
    {'&', TokenType::AMPERSAND},
    {'"', TokenType::DOUBLE_QUOTE},
    {'\'', TokenType::SINGLE_QUOTE},
    {';', TokenType::SEMICOLON},
    {':', TokenType::COLON},
    {',', TokenType::COMMA},
    {'.', TokenType::DOT},
    {'!', TokenType::BANG},
    {'|', TokenType::PIPE},
    {'^', TokenType::CARROT},
    {'~', TokenType::TILDE}
};

inline const std::unordered_map<std::string_view, TokenType> double_symbol_map {
    {"==", TokenType::EQUAL_EQUAL},
    {"!=", TokenType::BANG_EQUAL},
    {"+=", TokenType::PLUS_EQUAL},
    {"-=", TokenType::MINUS_EQUAL},
    {"*=", TokenType::STAR_EQUAL},
    {"/=", TokenType::SLASH_EQUAL},
    {"%=", TokenType::PERCENT_EQUAL},
    {"<=", TokenType::LESS_EQUAL},
    {">=", TokenType::GREATER_EQUAL},
    {"++", TokenType::PLUS_PLUS},
    {"--", TokenType::MINUS_MINUS},
    {"<<", TokenType::LESS_LESS},
    {">>", TokenType::GREATER_GREATER},
    {"**", TokenType::STAR_STAR},
    {"&&", TokenType::AMPERSAND_AMPERSAND},
    {"||", TokenType::PIPE_PIPE},
    {"->", TokenType::ARROW},
    {"//", TokenType::SLASH_SLASH},
};

struct Token {
    TokenType type_;
    std::string_view lexeme_, source_line_;
    std::size_t line_number_, column_number_, length_;

    Token(TokenType type, std::string_view lexeme, std::string_view source_line, std::size_t line_number, std::size_t column_number, std::size_t length) noexcept:
        type_{ type }, 
        lexeme_{ lexeme }, 
        source_line_{ source_line }, 
        line_number_{ line_number }, 
        column_number_{ column_number },
        length_{ length } {}

    Token(TokenType type, std::string_view lexeme) noexcept:
        type_{ type },
        lexeme_{ lexeme },
        line_number_{ 0 }, 
        column_number_{ 0 },
        length_{ 0 } {}

    Token() = default;
    ~Token() = default;

    std::string to_string() const noexcept
    {
        return std::format("[{}] {}:{} {} ", static_cast<int>(type_), line_number_, column_number_, lexeme_);
    }
};