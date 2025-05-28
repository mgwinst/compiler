#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

enum class TokenType : uint16_t {
    IDENTIFIER = 128,

    INTEGER_CONSTANT,
    FLOAT_CONSTANT,
    CHAR_CONSTANT,
    NUMERIC_LITERAL,
    STRING_LITERAL,

    KW_IF,
    KW_WHILE,
    KW_FOR,
    KW_RETURN,
    
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    MODULUS,
    INCREMENT,
    DECREMENT,

    ASSIGN,
    PLUS_ASSIGN,
    MINUS_ASSIGN,
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
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    SEMICOLON,
    COMMA,
    DOT,

    DEREFERENCE,
    ADDRESS_OF,
    ARROW,

    INVALID,   

    // bitwise later (overlap with some other tokens)
};

namespace {
    const std::unordered_map<char, TokenType> symbol_map = {
        {'+', TokenType::ADD},
        {'-', TokenType::SUBTRACT},
        {'*', TokenType::MULTIPLY},
        {'/', TokenType::DIVIDE},
        {'(', TokenType::LPAREN},
        {')', TokenType::RPAREN},
        {'{', TokenType::LBRACE},
        {'}', TokenType::RBRACE},
        {'[', TokenType::LBRACKET},
        {']', TokenType::RBRACKET},
    };
}

struct Token {
    TokenType type;
    std::string value;
};

std::vector<Token> tokenize(const std::string& source) {
    std::vector<Token> tokens;

    auto cur = source.begin();
    auto start = cur, end = cur;

    while (cur != source.end()) {
        if (std::isspace(*cur)) {
            cur++;
        } else if (std::isdigit(*cur)) {
            start = cur;
            while (std::isdigit(*cur)) {
                cur++;
            }
            end = cur;
            tokens.emplace_back(TokenType::NUMERIC_LITERAL, std::string{start, end});
            cur++;
        } else if (std::isalpha(*cur)) {
            start = cur;
            while (std::isalpha(*cur) || *cur == '_') {
                cur++;
            }
            end = cur;
            std::string value {start, end};

            TokenType type = TokenType::IDENTIFIER;
            if (value == "if")
                type = TokenType::KW_IF;
            else if (value == "while")
                type = TokenType::KW_WHILE;
            else if (value == "for")
                type = TokenType::KW_FOR;
            else if (value == "return") 
                type = TokenType::KW_RETURN;

            tokens.emplace_back(type, value);

        } else if (symbol_map.count(*cur)) {
            tokens.emplace_back(symbol_map.at(*cur), std::string{*cur});
            cur++;
        } else {
            throw std::runtime_error("Invalid token: " + std::string{*cur} + "");
        }
    }
    
    return tokens;
}

















void tokenize_test() {
    std::string s {"return     (a + b * 923 /   2) + (c + d * g + 4442)"};
    try {
        auto tokens = tokenize(s);
        for (Token tok : tokens) {
            std::cout << "Token: " << tok.value << ' ' << "Type: " << static_cast<int>(tok.type) << '\n';
        }
    } catch (const std::runtime_error &e) {
        std::cerr << e.what() << '\n';
    }
}


int main() {
    tokenize_test();
    
}


