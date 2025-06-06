#include <string>
#include <vector>
#include <unordered_set>
#include <cctype>
#include <fstream>
#include <iostream>
#include <print>

#include "lexer.h"

namespace {
    const std::unordered_set<std::string_view> type_keywords = {
        "int8", "int16", "int32", "int64",
        "uint8", "uint16", "uint32", "uint64",
        "float16", "float32", "float64",
        "char", "string", "struct", "bool",
        "void", "union"
    };
}

[[nodiscard]] std::vector<Token> lex(std::string_view source) {
    std::vector<Token> tokens;

    auto cur = source.begin();
    auto start = cur, end = cur;

    std::size_t line_num = 1, col_num = 1, token_len = 1;

    while (cur != source.end()) {
        if (std::isspace(*cur)) {
            cur++;
            
        } else if (std::isdigit(*cur)) {
            start = cur;
            while (std::isdigit(*cur)) cur++;
            end = cur;
            tokens.emplace_back(TokenType::NUMERIC_LITERAL, std::string_view{start, end});

        } else if (std::isalpha(*cur)) {
            start = cur;
            while (std::isalnum(*cur) || *cur == '_') cur++;
            end = cur;

            std::string_view value{start, end};
            TokenType type = TokenType::IDENTIFIER;

            if (value == "if")
                type = TokenType::KEYWORD_IF;
            else if (value == "else")
                type = TokenType::KEYWORD_ELSE;
            else if (value == "while")
                type = TokenType::KEYWORD_WHILE;
            else if (value == "for")
                type = TokenType::KEYWORD_FOR;
            else if (value == "return") 
                type = TokenType::KEYWORD_RETURN;
            else if (value == "const")
                type = TokenType::KEYWORD_CONST;
            else if (value == "typedef")
                type = TokenType::KEYWORD_TYPEDEF;
            else if (type_keywords.contains(value))
                type = TokenType::TYPE;

            tokens.emplace_back(type, std::string_view{value});
        
        } else {
            switch (*cur) {
                case '=':
                    if (*(cur+1) == '=') {
                        tokens.emplace_back(TokenType::EQUALS, std::string_view{cur, 2});
                        cur += 2;
                    } else {
                        tokens.emplace_back(TokenType::ASSIGN, std::string_view{cur, 1});
                        cur++;
                    }
                    break;
                case '+':
                    if (*(cur+1) == '=') {
                        tokens.emplace_back(TokenType::ADD_ASSIGN, std::string_view{cur, 2});
                        cur += 2;
                    } else if (*(cur+1) == '+') {
                        tokens.emplace_back(TokenType::INCREMENT, std::string_view{cur, 2});
                        cur += 2;
                    } else {
                        tokens.emplace_back(TokenType::ADD, std::string_view{cur, 1});
                        cur++;
                    }
                    break;
                case '-':
                    if (*(cur+1) == '=') {
                        tokens.emplace_back(TokenType::SUBTRACT_ASSIGN, std::string_view{cur, 2});
                        cur += 2;
                    }else if (*(cur+1) == '-') {
                        tokens.emplace_back(TokenType::DECREMENT, std::string_view{cur, 2});
                        cur += 2;
                    } else if (*(cur+1) == '>') {
                        tokens.emplace_back(TokenType::ARROW, std::string_view{cur, 2});
                        cur += 2;
                    } else {
                        tokens.emplace_back(TokenType::SUBTRACT, std::string_view{cur, 1});
                        cur++;
                    }
                    break;
                case '*':
                    if (*(cur+1) == '=') {
                        tokens.emplace_back(TokenType::MULTIPLY_ASSIGN, std::string_view{cur, 2});
                        cur += 2;
                    } else {
                        tokens.emplace_back(TokenType::MULTIPLY, std::string_view{cur, 1});
                        cur++;
                    }
                    break;
                case '/':
                    if (*(cur+1) == '=') {
                        tokens.emplace_back(TokenType::DIVIDE_ASSIGN, std::string_view{cur, 2});
                        cur += 2; 
                    } else if (*(cur+1) == '/') {
                        tokens.emplace_back(TokenType::COMMENT, std::string_view{cur, 2});
                        cur += 2;
                    } else {
                        tokens.emplace_back(TokenType::DIVIDE, std::string_view{cur, 1});
                        cur++;
                    }
                    break;
                case '%':
                    tokens.emplace_back(TokenType::MODULUS, std::string_view{cur, 1});
                    cur++;
                    break;
                case '(':
                    tokens.emplace_back(TokenType::LPAREN, std::string_view{cur, 1});
                    cur++;
                    break;
                case ')':
                    tokens.emplace_back(TokenType::RPAREN, std::string_view{cur, 1});
                    cur++;
                    break;
                case '{':
                    tokens.emplace_back(TokenType::LBRACE, std::string_view{cur, 1});
                    cur++;
                    break;
                case '}':
                    tokens.emplace_back(TokenType::RBRACE, std::string_view{cur, 1});
                    cur++;
                    break;
                case '[':
                    tokens.emplace_back(TokenType::LBRACKET, std::string_view{cur, 1});
                    cur++;
                    break;
                case ']':
                    tokens.emplace_back(TokenType::RBRACKET, std::string_view{cur, 1});
                    cur++;
                    break;
                case '>':
                    if (*(cur+1) == '=') {
                        tokens.emplace_back(TokenType::GREATER_EQUAL, std::string_view{cur, 2});
                        cur += 2;
                    } else {
                        tokens.emplace_back(TokenType::GREATER_THAN, std::string_view{cur, 1});
                        cur++;
                    }
                    break;
                case '<':
                    if (*(cur+1) == '=') {
                        tokens.emplace_back(TokenType::LESS_EQUAL, std::string_view{cur, 2});
                        cur += 2;
                    } else {
                        tokens.emplace_back(TokenType::LESS_THAN, std::string_view{cur, 1});
                        cur++;
                    }
                    break;
                case '&':
                    if (*(cur+1) == '&') {
                        tokens.emplace_back(TokenType::LOGICAL_AND, std::string_view{cur, 2});
                        cur += 2;
                    } else {
                        tokens.emplace_back(TokenType::ADDRESS_OF, std::string_view{cur, 1});
                        cur++;
                    }
                    break;
                case '"':
                    start = cur++;
                    while (*cur != '"') cur++;
                    end = cur;
                    tokens.emplace_back(TokenType::STRING_LITERAL, std::string_view{start, end+1});
                    cur++;
                    break;
                case ';':
                    tokens.emplace_back(TokenType::SEMICOLON, std::string_view{cur, 1});
                    cur++;
                    break;
                case ':':
                    tokens.emplace_back(TokenType::COLON, std::string_view{cur, 1});
                    cur++;
                    break;
                case ',':
                    tokens.emplace_back(TokenType::COMMA, std::string_view{cur, 1});
                    cur++;
                    break;
                case '.':
                    tokens.emplace_back(TokenType::DOT, std::string_view{cur, 1});
                    cur++;
                    break;
                case '!':
                    if (*(cur+1) == '=') {
                        tokens.emplace_back(TokenType::NOT_EQUAL, std::string_view{cur, 2});
                        cur += 2;
                    } else {
                        tokens.emplace_back(TokenType::LOGICAL_NOT, std::string_view{cur, 1});
                        cur++;
                    }
                    break;
                case '|':
                    if (*(cur+1) == '|') {
                        tokens.emplace_back(TokenType::LOGICAL_OR, std::string_view{cur, 2});
                        cur += 2;
                    } else {
                        tokens.emplace_back(TokenType::PIPE, std::string_view{cur, 1});
                        cur++;
                    }
                    break;
                case EOF:
                    tokens.emplace_back(TokenType::END_OF_FILE, std::string_view{cur, 1});
                    break;
                default:
                    throw std::runtime_error("Invalid token: " + std::string{*cur});
            }
        }
    }

    return tokens;
}

/*
void test_lex(std::string path) {
    std::ifstream file{path};
    if (!file.is_open())
        throw std::runtime_error("can't open file");

    std::string source_text;
    std::string line;

    while (getline(file, line)) {
        source_text += line + '\n'; // append new line manually
    }

    auto tokens = lex(source_text);

    for (const auto& tok : tokens) {
        std::cout << tok.to_string() << '\n';
    }

}

int main() {
    test_lex("../test/sample_program.c");
    //test_lex("../test/sample_tokens.txt");

}
*/