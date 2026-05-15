#include <gtest/gtest.h>
#include <fstream>

#include "lexer/lexer.hpp"
#include "utils/utils.hpp"

TokenType token_test_table[] = {
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
    TokenType::KEYWORD_CONST,
    TokenType::KEYWORD_TYPEDEF,
    TokenType::KEYWORD_STATIC,
    TokenType::KEYWORD_TRUE,
    TokenType::KEYWORD_FALSE,
    TokenType::KEYWORD_SIZEOF,
    TokenType::KEYWORD_ALIGNOF,
    TokenType::KEYWORD_LAMBDA,
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
    TokenType::TYPE
};

TEST(TestLexer, Tokens) {
    Module module = get_module("lexer/sample_tokens.txt");
    Lexer lexer{module.data};

    int i = 0;
    while (1) {
        Token tok = lexer.get_token();
        if (tok.type_ == TokenType::END_OF_FILE) 
            break;

        EXPECT_EQ(tok.type_, token_test_table[i]) << "Mismatch at token: " << i << \
        " [" << static_cast<int>(tok.type_) << ", " << static_cast<int>(token_test_table[i]) << ']' << \
        " [error token: " << tok.lexeme_ << "]";

        i++;
    }
}

TEST(TestLexer, GetAndPeekToken)
{
    Module module = get_module("lexer/sample_tokens.txt");
    Lexer lexer{module.data};

    EXPECT_EQ(lexer.get_token().type_, token_test_table[0]);
    EXPECT_EQ(lexer.peek_token().type_, token_test_table[1]);
}