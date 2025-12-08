#pragma once

#define EXPECT_SEMICOLON() \
    do { \
        auto [semi_colon] = expect(TokenType::SEMICOLON); \
        if (!semi_colon) return std::unexpected{ SyntaxError{prev_token_, "missing ';'"} }; \
    } while (0)

#define EXPECT_LBRACE() \
    do { \
        auto [lbrace] = expect(TokenType::LBRACE); \
        if (!lbrace) return std::unexpected{ SyntaxError{prev_token_, "missing '{'"} }; \
    } while (0)

#define EXPECT_RBRACE() \
    do { \
        auto [rbrace] = expect(TokenType::RBRACE); \
        if (!rbrace) return std::unexpected{ SyntaxError{prev_token_, "missing '}'"} }; \
    } while (0)

#define EXPECT_LPAREN() \
    do { \
        auto [lparen] = expect(TokenType::LPAREN); \
        if (!lparen) return std::unexpected{ SyntaxError{prev_token_, "missing '('"} }; \
    } while (0)

#define EXPECT_RPAREN() \
    do { \
        auto [rparen] = expect(TokenType::RPAREN); \
        if (!rparen) return std::unexpected{ SyntaxError{prev_token_, "missing ')'"} }; \
    } while (0)

#define EXPECT_LBRACKET() \
    do { \
        auto [lbracket] = expect(TokenType::LBRACKET); \
        if (!lbracket) return std::unexpected{ SyntaxError{prev_token_, "missing '['"} }; \
    } while (0)

#define EXPECT_RBRACKET() \
    do { \
        auto [rbracket] = expect(TokenType::RBRACKET); \
        if (!rbracket) return std::unexpected{ SyntaxError{prev_token_, "missing ']'"} }; \
    } while (0)