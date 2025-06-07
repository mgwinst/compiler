#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <cctype>
#include <fstream>
#include <iostream>
#include <cassert>

#include "lexer.h"

namespace {

    const std::unordered_set<std::string_view> lang_types {
        "int", "int8", "int16", "int32", "int64",
        "uint", "uint8", "uint16", "uint32", "uint64",
        "float" "float16", "float32", "float64",
        "char", "string", "struct", "bool", "void", "union"
    };
    
    const std::unordered_map<std::string_view, TokenType> keywords {
        {"if", TokenType::KEYWORD_IF},
        {"else", TokenType::KEYWORD_ELSE},
        {"while", TokenType::KEYWORD_WHILE},
        {"for", TokenType::KEYWORD_FOR},
        {"return", TokenType::KEYWORD_RETURN},
        {"typedef", TokenType::KEYWORD_TYPEDEF},
    };

    const std::unordered_map<char, TokenType> single_symbol_map {
        {'=', TokenType::EQUAL},
        {'+', TokenType::PLUS},
        {'-', TokenType::MINUS},
        {'*', TokenType::STAR},
        {'/', TokenType::SLASH},
        {'%', TokenType::PERCENT},
        {'<', TokenType::LESS},
        {'>', TokenType::GREATER},
        {'(', TokenType::LPAREN},
        {')', TokenType::RPAREN},
        {'{', TokenType::LBRACKET},
        {'}', TokenType::RBRACKET},
        {'[', TokenType::LBRACE},
        {']', TokenType::RBRACE},
        {'&', TokenType::AMPERSAND},
        {'"', TokenType::DOUBLE_QUOTE},
        {'\'', TokenType::SINGLE_QUOTE},
        {';', TokenType::SEMICOLON},
        {':', TokenType::COLON},
        {',', TokenType::COMMA},
        {'.', TokenType::DOT},
        {'!', TokenType::BANG},
        {'|', TokenType::PIPE},
    };

    const std::unordered_map<std::string_view, TokenType> double_symbol_map {
        {"==", TokenType::EQUAL_EQUAL},
        {"!=", TokenType::BANG_EQUAL},
        {"+=", TokenType::PLUS_EQUAL},
        {"-=", TokenType::MINUS_EQUAL},
        {"*=", TokenType::STAR_EQUAL},
        {"/=", TokenType::SLASH_EQUAL},
        {"%=", TokenType::PERCENT_EQUAL},
        {"<=", TokenType::LESS_EQUAL},
        {">=", TokenType::GREATER_EQUAL},
        {"++", TokenType::PLUS_PLUS},
        {"--", TokenType::MINUS_MINUS},
        {"<<", TokenType::LESS_LESS},
        {">>", TokenType::GREATER_GREATER},
        {"&&", TokenType::AMPERSAND_AMPERSAND},
        {"||", TokenType::PIPE_PIPE},
        {"->", TokenType::ARROW},
        {"//", TokenType::SLASH_SLASH},
    };
}

[[nodiscard]] std::vector<Token> lex(std::string_view source) {
    std::vector<Token> tokens;

    auto cur = source.begin();
    auto start = cur, end = cur;

    std::size_t line_num = 1, col_num = 1;

    while (cur != source.end()) {
        // whitespace
        if (std::isspace(*cur)) {
            if (*cur == '\n') {
                line_num++;
                col_num = 1;
            } else {
                col_num++;
            }
            cur++;
        
        // digits
        } else if (std::isdigit(*cur)) {
            start = cur;
            while (std::isdigit(*cur)) {
                cur++; col_num++;
            }
            end = cur;
            tokens.emplace_back(TokenType::NUMERIC_LITERAL, std::string_view{start, end}, line_num, col_num - (end-start), (end-start));
        
        // identifiers, types and keywords
        } else if (std::isalpha(*cur)) {
            start = cur;
            while (std::isalnum(*cur) || *cur == '_') {
                cur++; col_num++;
            }
            end = cur;

            std::string_view value{start, end};

            if (auto kw = keywords.find(value); kw != keywords.end()) {
                tokens.emplace_back(kw->second, std::string_view{start, end}, line_num, col_num - (end-start), (end-start));
            } else if (auto t = lang_types.find(value); t != lang_types.end()) {
                tokens.emplace_back(TokenType::TYPE, std::string_view{start, end}, line_num, col_num - (end-start), (end-start));
            } else {
                tokens.emplace_back(TokenType::IDENTIFIER, std::string_view{start, end}, line_num, col_num - (end-start), (end-start));
            }

        // symbols
        } else if (auto double_symbol = double_symbol_map.find(std::string_view{cur, 2}); double_symbol != double_symbol_map.end()) {
            tokens.emplace_back(double_symbol->second, std::string_view{cur, 2}, line_num, col_num, 2);
            cur += 2; col_num += 2;
        } else if (auto symbol = single_symbol_map.find(*cur); symbol != single_symbol_map.end()) {
            tokens.emplace_back(symbol->second, std::string_view{cur, 1}, line_num, col_num, 1);
            cur++; col_num++;
        } else {
            std::runtime_error("Invalid token: " + std::string{*cur});
        }
    }

    return tokens;
}

void test_lex(std::string path) {
    std::ifstream file{path};
    assert(file.is_open());

    std::string source_text;
    std::string line;

    while (getline(file, line)) {
        source_text += line + '\n';
    }

    auto tokens = lex(source_text);

    for (const auto& tok : tokens) {
        std::cout << tok.to_string() << '\n';
    }

}

int main() {
    // test_lex("../test/sample_program.c");
    test_lex("../test/sample_tokens.txt");

}







