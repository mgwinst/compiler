#include <fstream>
#include <gtest/gtest.h>
#include <cassert>
#include "../lexer/lexer.h"

TEST(LEXER_TEST, lexer) {
    std::ifstream file{"sample_tokens.txt"};
    assert(file.is_open());
        
    std::string source_text;
    std::string line;

    while (getline(file, line)) {
        source_text += line;
    }

    auto tokens = lex(source_text);
    
    ASSERT_EQ(tokens[0].type, TokenType::IDENTIFIER);
    ASSERT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    ASSERT_EQ(tokens[2].type, TokenType::NUMERIC_LITERAL);
    ASSERT_EQ(tokens[3].type, TokenType::NUMERIC_LITERAL);
    ASSERT_EQ(tokens[4].type, TokenType::STRING_LITERAL);
    ASSERT_EQ(tokens[5].type, TokenType::KEYWORD_IF);
    ASSERT_EQ(tokens[6].type, TokenType::KEYWORD_ELSE);
    ASSERT_EQ(tokens[7].type, TokenType::KEYWORD_WHILE);
    ASSERT_EQ(tokens[8].type, TokenType::KEYWORD_FOR);
    ASSERT_EQ(tokens[9].type, TokenType::KEYWORD_RETURN);
    ASSERT_EQ(tokens[10].type, TokenType::KEYWORD_CONST);
    ASSERT_EQ(tokens[11].type, TokenType::ADD);
    ASSERT_EQ(tokens[12].type, TokenType::SUBTRACT);
    ASSERT_EQ(tokens[13].type, TokenType::MULTIPLY);
    ASSERT_EQ(tokens[14].type, TokenType::DIVIDE);
    ASSERT_EQ(tokens[15].type, TokenType::MODULUS);
    ASSERT_EQ(tokens[16].type, TokenType::INCREMENT);
    ASSERT_EQ(tokens[17].type, TokenType::DECREMENT);
    ASSERT_EQ(tokens[18].type, TokenType::ASSIGN);
    ASSERT_EQ(tokens[19].type, TokenType::ADD_ASSIGN);
    ASSERT_EQ(tokens[20].type, TokenType::SUBTRACT_ASSIGN);
    ASSERT_EQ(tokens[21].type, TokenType::MULTIPLY_ASSIGN);
    ASSERT_EQ(tokens[22].type, TokenType::DIVIDE_ASSIGN);
    ASSERT_EQ(tokens[23].type, TokenType::EQUALS);
    ASSERT_EQ(tokens[24].type, TokenType::NOT_EQUAL);
    ASSERT_EQ(tokens[25].type, TokenType::LESS_EQUAL);
    ASSERT_EQ(tokens[26].type, TokenType::GREATER_EQUAL);
    ASSERT_EQ(tokens[27].type, TokenType::LESS_THAN);
    ASSERT_EQ(tokens[28].type, TokenType::GREATER_THAN);
    ASSERT_EQ(tokens[29].type, TokenType::LOGICAL_AND);
    ASSERT_EQ(tokens[30].type, TokenType::LOGICAL_NOT);
    ASSERT_EQ(tokens[31].type, TokenType::LPAREN);
    ASSERT_EQ(tokens[32].type, TokenType::RPAREN);
    ASSERT_EQ(tokens[33].type, TokenType::LBRACE);
    ASSERT_EQ(tokens[34].type, TokenType::RBRACE);
    ASSERT_EQ(tokens[35].type, TokenType::LBRACKET);
    ASSERT_EQ(tokens[36].type, TokenType::RBRACKET);
    ASSERT_EQ(tokens[37].type, TokenType::SEMICOLON);
    ASSERT_EQ(tokens[38].type, TokenType::COLON);
    ASSERT_EQ(tokens[39].type, TokenType::COMMA);
    ASSERT_EQ(tokens[40].type, TokenType::DOT);
    ASSERT_EQ(tokens[41].type, TokenType::ADDRESS_OF);
    ASSERT_EQ(tokens[42].type, TokenType::ARROW);
    ASSERT_EQ(tokens[43].type, TokenType::COMMENT);
    ASSERT_EQ(tokens[44].type, TokenType::KEYWORD_TYPEDEF);

    /*
    ASSERT_EQ(tokens[43].type, TokenType::TYPE_INT);
    ASSERT_EQ(tokens[44].type, TokenType::TYPE_INT8);
    ASSERT_EQ(tokens[45].type, TokenType::TYPE_INT16);
    ASSERT_EQ(tokens[46].type, TokenType::TYPE_INT32);
    ASSERT_EQ(tokens[47].type, TokenType::TYPE_INT64);
    ASSERT_EQ(tokens[48].type, TokenType::TYPE_FLOAT);
    ASSERT_EQ(tokens[49].type, TokenType::TYPE_FLOAT16);
    ASSERT_EQ(tokens[50].type, TokenType::TYPE_FLOAT32);
    ASSERT_EQ(tokens[51].type, TokenType::TYPE_FLOAT64);
    ASSERT_EQ(tokens[52].type, TokenType::TYPE_CHAR);
    ASSERT_EQ(tokens[53].type, TokenType::TYPE_STRING);
    ASSERT_EQ(tokens[54].type, TokenType::TYPE_STRUCT);
    ASSERT_EQ(tokens[55].type, TokenType::TYPE_UNION);
    */
}