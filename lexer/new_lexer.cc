#include <string>
#include <vector>
#include <unordered_set>
#include <cctype>
#include <fstream>
#include <iostream>

#include "lexer.h"

namespace {
    const std::unordered_set<std::string_view> type_keywords = {
        "int8", "int16", "int32", "int64",
        "uint8", "uint16", "uint32", "uint64",
        "float16", "float32", "float64",
        "char", "string", "struct", "bool",
        "void", "union"
    };

    const std::unordered_set<TokenType> tokens_require_lexeme = {
        TokenType::IDENTIFIER,
        TokenType::CHAR_LITERAL,
        TokenType::STRING_LITERAL,
        TokenType::NUMERIC_LITERAL,
        TokenType::TYPE,
    };
}

[[nodiscard]] std::vector<Token> lex(std::string_view source) {
    std::vector<Token> tokens;

    auto cur = source.begin();
    auto start = cur, end = cur;

    std::size_t line_num = 1, col_num = 1, token_len = 1;

    TokenType type;
    
    while (cur != source.end()) {
        if (std::isspace(*cur)) {
            if (*cur == '\n') {
                line_num++;
                col_num = 1;
            } else {
                col_num++;
            }
            cur++;

        } else if (std::isdigit(*cur)) {
            start = cur;
            while (std::isdigit(*cur)) {
                cur++;
                col_num++;
            }
            end = cur;
            tokens.emplace_back(TokenType::NUMERIC_LITERAL, std::string_view{start, end}, line_num, col_num - (end-start), (end-start));

        } else if (std::isalpha(*cur)) {
            start = cur;
            while (std::isalnum(*cur) || *cur == '_') {
                cur++;
                col_num++;
            }
            end = cur;

            std::string_view value{start, end};
            type = TokenType::IDENTIFIER;

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

            tokens.emplace_back(type, std::string_view{start, end}, line_num, col_num - (end-start), (end-start));

        } else {
            
        }
    }

    return tokens;
}

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