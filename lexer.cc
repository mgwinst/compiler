#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <chrono>

enum class TokenType : uint16_t {
    IDENTIFIER = 128,

    INTEGER_CONSTANT,
    FLOAT_CONSTANT,
    CHAR_CONSTANT,
    NUMERIC_LITERAL,
    STRING_LITERAL,

    KW_IF,
    KW_ELSE,
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
    COLON,
    COMMA,
    DOT,

    DEREFERENCE,
    ADDRESS_OF,
    ARROW,

    END_OF_FILE,
    INVALID,   

    // bitwise later (overlap with some other tokens)
};

namespace {
    const std::unordered_map<char, TokenType> symbol_map = {
        {'=', TokenType::ASSIGN},
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
        {'>', TokenType::GREATER_THAN},
        {'<', TokenType::LESS_THAN},
        {']', TokenType::RBRACKET},
        {']', TokenType::RBRACKET},
        {';', TokenType::SEMICOLON},
        {':', TokenType::COLON},
        {',', TokenType::COMMA},
        {'.', TokenType::DOT},
        {'!', TokenType::LOGICAL_NOT},
    };
}

struct Token {
    TokenType type;
    std::string lexeme;

    std::string to_string() const {
        return std::to_string(static_cast<int>(type)) + " " + lexeme;
    }
};

[[nodiscard]] std::vector<Token> tokenize(std::string_view source) {
    std::vector<Token> tokens;

    auto cur = source.begin();
    auto start = cur, end = cur;

    while (cur != source.end()) {
        if (std::isspace(*cur)) cur++;
        else if (std::isdigit(*cur)) {
            start = cur;
            while (std::isdigit(*cur)) cur++;
            end = cur;
            tokens.emplace_back(TokenType::NUMERIC_LITERAL, std::string{start, end});
            cur++;
        } else if (std::isalpha(*cur)) {
            start = cur;
            while (std::isalpha(*cur) || *cur == '_') cur++;
            end = cur;

            std::string value {start, end};
            TokenType type = TokenType::IDENTIFIER;

            if (value == "if")
                type = TokenType::KW_IF;
            if (value == "else")
                type = TokenType::KW_ELSE;
            else if (value == "while")
                type = TokenType::KW_WHILE;
            else if (value == "for")
                type = TokenType::KW_FOR;
            else if (value == "return") 
                type = TokenType::KW_RETURN;

            tokens.emplace_back(type, std::string{value});

        } else if (symbol_map.count(*cur)) {
            tokens.emplace_back(symbol_map.at(*cur), std::string{*cur});
            cur++;
        } else {
            throw std::runtime_error("Invalid token: " + std::string{*cur} + "");
        }
    }
    
    return tokens;
}

void tokenize_test(std::string path) {
    std::string source_text;
    
    for (int i = 0; i < 100000; i++) {
        std::ifstream file{path};
        if (!file.is_open())
            throw std::runtime_error("can't open file" + path);

        std::string line;

        while (getline(file, line)) {
            source_text += line;
        }
    }

    try {
        auto start = std::chrono::high_resolution_clock::now();
        auto tokens = tokenize(source_text);
        auto end = std::chrono::high_resolution_clock::now();

        auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << duration_ms.count() << "ms\n";
        
        /*
        for (Token tok : tokens) {
            std::cout << tok.to_string() << '\n';
        }
        */

    } catch (const std::runtime_error &e) {
        std::cerr << e.what() << '\n';
    }
}

int main() {

    tokenize_test("lex_example.txt");
    
}


