#include <gtest/gtest.h>
#include <fstream>

#include "lexer/lexer.h"
#include "utils/utils.h"


TEST(TestLexer, lexer) {
    TokenType test_table[60] = {
        TokenType::IDENTIFIER,
        TokenType::IDENTIFIER,
        TokenType::NUMERIC_LITERAL,
        TokenType::NUMERIC_LITERAL,
        // TokenType::STRING_LITERAL,
        // TokenType::CHAR_LITERAL,
        TokenType::KEYWORD_IF,
        TokenType::KEYWORD_ELSE,
        TokenType::KEYWORD_WHILE,
        TokenType::KEYWORD_FOR,
        TokenType::KEYWORD_RETURN,
        TokenType::KEYWORD_CONST,
        TokenType::KEYWORD_TYPEDEF,
        TokenType::PLUS,
        TokenType::MINUS,
        TokenType::STAR,
        TokenType::SLASH,
        TokenType::PERCENT,
        TokenType::PLUS_PLUS,
        TokenType::MINUS_MINUS,
        TokenType::EQUAL,
        TokenType::PLUS_EQUAL,
        TokenType::MINUS_EQUAL,
        TokenType::STAR_EQUAL,
        TokenType::SLASH_EQUAL,
        TokenType::EQUAL_EQUAL,
        TokenType::BANG_EQUAL,
        TokenType::LESS_EQUAL,
        TokenType::GREATER_EQUAL,
        TokenType::LESS,
        TokenType::GREATER,
        TokenType::AMPERSAND_AMPERSAND,
        TokenType::BANG,
        TokenType::LPAREN,
        TokenType::RPAREN,
        TokenType::LBRACE,
        TokenType::RBRACE,
        TokenType::LBRACKET,
        TokenType::RBRACKET,
        TokenType::SEMICOLON,
        TokenType::COLON,
        TokenType::COMMA,
        TokenType::DOT,
        TokenType::AMPERSAND,
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
        TokenType::TYPE
    };

    std::string source_text = get_source_text("../samples/sample_program");
    Lexer lexer{source_text};

    int i = 0;
    while (1) {
        lexer.cur_token = lexer.get_token();
        if (lexer.cur_token.type == TokenType::END_OF_FILE) break;

        EXPECT_EQ(lexer.cur_token.type, test_table[i]) << "Mismatch at token: " << i << \
        " [" << static_cast<int>(lexer.cur_token.type) << ", " << static_cast<int>(test_table[i]) << ']' << \
        " [error token: " << lexer.cur_token.lexeme.value() << "]";

        i++;
    }
}
