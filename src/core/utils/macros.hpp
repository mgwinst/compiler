#pragma once

#include <source_location>
#include <cstring>
#include <print>
#include <iostream>

#define ASSERT(cond, msg) \
    { \
        auto loc = std::source_location::current(); \
        auto loc_str = std::format("{}:{}:{}: {}", loc.file_name(), loc.line(), loc.column(), loc.function_name()); \
        if (!cond) [[unlikely]] { \
            std::println(std::cerr, "{} | {} | error: {} [{}]", loc_str, msg, std::strerror(errno), errno); \
            exit(EXIT_FAILURE); \
        } \
    }

#define error_exit(msg) \
    { \
        auto loc = std::source_location::current(); \
        auto loc_str = std::format("{}:{}:{}: {}", loc.file_name(), loc.line(), loc.column(), loc.function_name()); \
        std::println(std::cerr, "{} | {} | error: {} [{}]", loc_str, msg, std::strerror(errno), errno); \
        std::exit(EXIT_FAILURE); \
    }

// ************* Parsing Helpers *************

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