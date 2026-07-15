#include <doctest/doctest.h>

#include <fstream>
#include <array>

#include "frontend/lexer/lexer.hpp"
#include "utils/utils.hpp"

std::array token_table = {
    TokenType::IDENTIFIER,
    TokenType::IDENTIFIER,
    TokenType::NUMERIC_LITERAL,
    TokenType::NUMERIC_LITERAL,
    // TokenType::STRING_LITERAL,
    // TokenType::CHAR_LITERAL,
    TokenType::KEYWORD_FUNCTION,
    TokenType::KEYWORD_STRUCT,
    TokenType::KEYWORD_IF,
    TokenType::KEYWORD_ELSE,
    TokenType::KEYWORD_WHILE,
    TokenType::KEYWORD_FOR,
    TokenType::KEYWORD_RETURN,
    TokenType::KEYWORD_BREAK,
    TokenType::KEYWORD_CONTINUE,
    TokenType::KEYWORD_CONST,
    TokenType::KEYWORD_TRUE,
    TokenType::KEYWORD_FALSE,
    TokenType::KEYWORD_INLINE,
    TokenType::EQUAL,
    TokenType::PLUS,
    TokenType::MINUS,
    TokenType::STAR,
    TokenType::SLASH,
    TokenType::PERCENT,
    TokenType::LESS,
    TokenType::GREATER,
    TokenType::LPAREN,
    TokenType::RPAREN,
    TokenType::LBRACE,
    TokenType::RBRACE,
    TokenType::LBRACKET,
    TokenType::RBRACKET,
    TokenType::AMPERSAND,
    TokenType::DOUBLE_QUOTE,
    TokenType::SINGLE_QUOTE,
    TokenType::SEMICOLON,
    TokenType::COLON,
    TokenType::COMMA,
    TokenType::DOT,
    TokenType::BANG,
    TokenType::PIPE,
    TokenType::CARROT,
    TokenType::EQUAL_EQUAL,
    TokenType::BANG_EQUAL,
    TokenType::PLUS_EQUAL,
    TokenType::MINUS_EQUAL,
    TokenType::STAR_EQUAL,
    TokenType::SLASH_EQUAL,
    TokenType::PERCENT_EQUAL,
    TokenType::LESS_EQUAL,
    TokenType::GREATER_EQUAL,
    TokenType::PLUS_PLUS,
    TokenType::MINUS_MINUS,
    TokenType::LESS_LESS,
    TokenType::GREATER_GREATER,
    TokenType::AMPERSAND_AMPERSAND,
    TokenType::PIPE_PIPE,
    TokenType::ARROW,
    TokenType::SLASH_SLASH,
    TokenType::TYPE,
    TokenType::TYPE,
    TokenType::TYPE,
    TokenType::TYPE,
    TokenType::TYPE,
    TokenType::TYPE,
    TokenType::TYPE,
    TokenType::TYPE,
    TokenType::TYPE,
    TokenType::TYPE,
    TokenType::TYPE,
    TokenType::TYPE,
    TokenType::TYPE,
    TokenType::TYPE,
    TokenType::TYPE,
    TokenType::TYPE,
    TokenType::TYPE,
    TokenType::TYPE,
};

constexpr std::string_view tokens = R"(
var1 b 123 35
fn struct if else while for return break continue const true false inline
= + - * / % < > ( ) { } [ ] & " ' ; : , . ! | ^
== != += -= *= /= %= <= >= ++ -- << >> && || -> //
int int8 int16 int32 int64
uint uint8 uint16 uint32 uint64
float float16 float32 float64
char bool void union)";

TEST_CASE("tokens") {
    Lexer lexer{tokens};

    int i = 0;
    while (true) {
        Token tok = lexer.get_token();
        if (tok.type_ == TokenType::END_OF_FILE) 
            break;

        INFO(std::format("{} != {}", token_type_to_string(tok.type_), token_type_to_string(token_table[i])));
        
        CHECK(tok.type_ == token_table[i]);

        ++i;
    }
}

TEST_CASE("get_token() / peek_token()")
{
    Lexer lexer{tokens};

    CHECK(lexer.get_token().type_ == token_table[0]);
    CHECK(lexer.peek_token().type_ == token_table[1]);
}