#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <cctype>
#include <variant>

enum class TokenType : uint16_t {
    IDENTIFIER = 128,

    INTEGER_CONSTANT,
    FLOAT_CONSTANT,
    CHAR_CONSTANT,
    NUMERIC_LITERAL,
    STRING_LITERAL,

    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,
    KEYWORD_FOR,
    KEYWORD_RETURN,
    
    ADD,
    SUBTRACT,
    MULTIPLY,
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
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    SINGLE_QUOTE,
    DOUBLE_QUOTE,
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
        {'%', TokenType::MODULUS},
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
        {'"', TokenType::DOUBLE_QUOTE},
        {';', TokenType::SEMICOLON},
        {':', TokenType::COLON},
        {',', TokenType::COMMA},
        {'.', TokenType::DOT},
        {'!', TokenType::LOGICAL_NOT},
    };
}

struct Token {
    TokenType type;
    std::string value;

    std::string to_string() const {
        return std::to_string(static_cast<int>(type)) + " " + value;
    }
};

[[nodiscard]] std::vector<Token> lex(std::string_view source) {
    std::vector<Token> tokens;

    auto cur = source.begin();
    auto start = cur, end = cur;

    while (cur != source.end()) {
        // white space
        if (std::isspace(*cur)) {
            cur++;

        // digits
        } else if (std::isdigit(*cur)) {
            start = cur;
            while (std::isdigit(*cur)) cur++;
            end = cur;
            tokens.emplace_back(TokenType::NUMERIC_LITERAL, std::string{start, end});

        // identifiers and keywords
        } else if (std::isalpha(*cur)) {
            start = cur;
            while (std::isalpha(*cur) || *cur == '_') cur++;
            end = cur;

            std::string_view value {start, end};
            TokenType type = TokenType::IDENTIFIER;

            if (value == "if")
                type = TokenType::KEYWORD_IF;
            if (value == "else")
                type = TokenType::KEYWORD_ELSE;
            else if (value == "while")
                type = TokenType::KEYWORD_WHILE;
            else if (value == "for")
                type = TokenType::KEYWORD_FOR;
            else if (value == "return") 
                type = TokenType::KEYWORD_RETURN;

            tokens.emplace_back(type, std::string{value});
        
        } else {
            switch (*cur) {
                case '=':
                    tokens.emplace_back(TokenType::ASSIGN, std::string{*cur});
                    cur++;
                    break;
                case '+':
                    if (*(cur+1) == '=') {
                        tokens.emplace_back(TokenType::ADD_ASSIGN, std::string{cur, cur+2});
                        cur += 2;
                    } else {
                        tokens.emplace_back(TokenType::ADD, std::string{*cur});
                        cur++;
                    }
                    break;
                case '-':
                    if (*(cur+1) == '=') {
                        tokens.emplace_back(TokenType::SUBTRACT_ASSIGN, std::string{cur, cur+2});
                        cur += 2;
                    } else {
                        tokens.emplace_back(TokenType::SUBTRACT, std::string{*cur});
                        cur++;
                    }
                    break;
                case '*':
                    if (*(cur+1) == '=') {
                        tokens.emplace_back(TokenType::MULTIPLY_ASSIGN, std::string{cur, cur+2});
                        cur += 2;
                    } else {
                        tokens.emplace_back(TokenType::MULTIPLY, std::string{*cur});
                        cur++;
                    }
                    break;
                case '/':
                    if (*(cur+1) == '=') {
                        tokens.emplace_back(TokenType::DIVIDE_ASSIGN, std::string{cur, cur+2});
                        cur += 2;
                    } else {
                        tokens.emplace_back(TokenType::DIVIDE, std::string{*cur});
                        cur++;
                    }
                    break;
                case '%':
                    tokens.emplace_back(TokenType::MODULUS, std::string{*cur});
                    cur++;
                    break;
                case '(':
                    tokens.emplace_back(TokenType::LPAREN, std::string{*cur});
                    cur++;
                    break;
                case ')':
                    tokens.emplace_back(TokenType::RPAREN, std::string{*cur});
                    cur++;
                    break;
                case '{':
                    tokens.emplace_back(TokenType::LBRACE, std::string{*cur});
                    cur++;
                    break;
                case '}':
                    tokens.emplace_back(TokenType::RBRACE, std::string{*cur});
                    cur++;
                    break;
                case '[':
                    tokens.emplace_back(TokenType::LBRACKET, std::string{*cur});
                    cur++;
                    break;
                case ']':
                    tokens.emplace_back(TokenType::RBRACKET, std::string{*cur});
                    cur++;
                    break;
                case '>':
                    if (*(cur+1) == '=') {
                        tokens.emplace_back(TokenType::GREATER_EQUAL, std::string{cur, cur+2});
                        cur += 2;
                    } else {
                        tokens.emplace_back(TokenType::GREATER_THAN, std::string{*cur});
                        cur++;
                    }
                    break;
                case '<':
                    if (*(cur+1) == '=') {
                        tokens.emplace_back(TokenType::LESS_EQUAL, std::string{cur, cur+2});
                        cur += 2;
                    } else {
                        tokens.emplace_back(TokenType::LESS_THAN, std::string{*cur});
                        cur++;
                    }
                    break;
                case '"':
                    start = cur;
                    while (*cur != '"') cur++;
                    end = cur;
                    tokens.emplace_back(TokenType::STRING_LITERAL, std::string{start, end+1});
                    cur++;
                    break;
                case ';':
                    tokens.emplace_back(TokenType::SEMICOLON, std::string{*cur});
                    cur++;
                    break;
                case ':':
                    tokens.emplace_back(TokenType::COLON, std::string{*cur});
                    cur++;
                    break;
                case ',':
                    tokens.emplace_back(TokenType::COMMA, std::string{*cur});
                    cur++;
                    break;
                case '.':
                    tokens.emplace_back(TokenType::DOT, std::string{*cur});
                    cur++;
                    break;
                case '!':
                    tokens.emplace_back(TokenType::LOGICAL_NOT, std::string{*cur});
                    cur++;
                    break;
                default:
                    throw std::runtime_error("Invalid token: " + std::string{*cur});
            }
        }
    }
    return tokens;
}

void lex_test(std::string path) {
    std::ifstream file{path};
    if (!file.is_open())
        throw std::runtime_error("can't open file" + path);

    std::string source_text;
    std::string line;

    while (getline(file, line)) {
        source_text += line;
    }

    try {
        auto tokens = lex(source_text);
        for (Token tok : tokens) {
            std::cout << tok.to_string() << '\n';
        }
    } catch (const std::runtime_error &e) {
        std::cerr << e.what() << '\n';
    }
}

int main() {
    lex_test("lex_example.txt");
}




