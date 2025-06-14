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

[[nodiscard]] auto Lexer::get_token() -> Token {
    auto start = cur, end = cur;

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
            auto start_col_num = col_num;
            while (std::isdigit(*cur)) {
                cur++; col_num++;
            }
            end = cur;
            return Token{TokenType::NUMERIC_LITERAL, std::string_view{start, end}, line_num, start_col_num, static_cast<std::size_t>(end-start)};
        
        // identifiers, types and keywords
        } else if (std::isalpha(*cur)) {
            start = cur;
            while (std::isalnum(*cur) || *cur == '_') {
                cur++; col_num++;
            }
            end = cur;

            std::string_view value{start, end};

            if (auto kw = keywords.find(value); kw != keywords.end()) {
                return Token{kw->second, std::string_view{start, end}, line_num, col_num - (end-start), static_cast<std::size_t>(end-start)};
            } else if (auto t = lang_types.find(value); t != lang_types.end()) {
                return Token{TokenType::TYPE, std::string_view{start, end}, line_num, col_num - (end-start), static_cast<std::size_t>(end-start)};
            } else {
                return Token{TokenType::IDENTIFIER, std::string_view{start, end}, line_num, col_num - (end-start), static_cast<std::size_t>(end-start)};
            }

        // symbols
        } else if (auto double_symbol = double_symbol_map.find(std::string_view{cur, 2}); double_symbol != double_symbol_map.end()) {
            Token token{double_symbol->second, std::string_view{cur, 2}, line_num, col_num, 2};
            cur += 2; col_num += 2;
            return token;
        } else if (auto symbol = single_symbol_map.find(*cur); symbol != single_symbol_map.end()) {
            return Token{symbol->second, std::string_view{cur++, 1}, line_num, col_num++, 1};
        } else {
            return Token{TokenType::INVALID, std::string_view{cur, 1}, line_num, col_num, 1};
        }
    }

    return Token{TokenType::END_OF_FILE, {}, line_num, col_num, 1};
}

[[nodiscard]] auto Lexer::peek_token() -> Token {
    auto peek_cur = cur;
    auto start = peek_cur, end = peek_cur;

    while (peek_cur != source.end()) {
        // whitespace
        if (std::isspace(*peek_cur)) {
            peek_cur++;
        
        // digits
        } else if (std::isdigit(*peek_cur)) {
            start = peek_cur;
            while (std::isdigit(*peek_cur)) {
                peek_cur++;
            }
            end = peek_cur;
            return Token{TokenType::NUMERIC_LITERAL, std::string_view{start, end}};
        
        // identifiers, types and keywords
        } else if (std::isalpha(*peek_cur)) {
            start = peek_cur;
            while (std::isalnum(*peek_cur) || *peek_cur == '_') {
                peek_cur++;
            }
            end = peek_cur;

            std::string_view value{start, end};

            if (auto kw = keywords.find(value); kw != keywords.end()) {
                return Token{kw->second, std::string_view{start, end}};
            } else if (auto t = lang_types.find(value); t != lang_types.end()) {
                return Token{TokenType::TYPE, std::string_view{start, end}};
            } else {
                return Token{TokenType::IDENTIFIER, std::string_view{start, end}};
            }

        // symbols
        } else if (auto double_symbol = double_symbol_map.find(std::string_view{peek_cur, 2}); double_symbol != double_symbol_map.end()) {
            Token token{double_symbol->second, std::string_view{peek_cur, 2}};
            return token;
        } else if (auto symbol = single_symbol_map.find(*peek_cur); symbol != single_symbol_map.end()) {
            return Token{symbol->second, std::string_view{peek_cur, 1}};
        } else {
            return Token{TokenType::INVALID, std::string_view{peek_cur, 1}};
        }
    }

    return Token{TokenType::END_OF_FILE, std::nullopt};
}

auto test_lex(std::string path) -> void {
    std::ifstream file{path};
    assert(file.is_open());

    std::string source_text;
    std::string line;

    while (getline(file, line)) {
        source_text += line + '\n';
    }

    Lexer lexer{source_text};
    
    while (1) {
        auto cur_token = lexer.get_token();
        auto next_token = lexer.peek_token();

        std::cout << cur_token.to_string() << " " << next_token.to_string_less() << '\n';
        if (cur_token.type == TokenType::END_OF_FILE) break;
    }

}

int main() {
    // test_lex("../test/sample_program.c");
    test_lex("../test/sample_tokens.txt");

}







