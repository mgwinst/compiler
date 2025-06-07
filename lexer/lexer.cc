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

inline char peekchar(const char* cur_char) {
    return *cur_char;
}

[[nodiscard]] std::vector<Token> lex(std::string_view source) {
    std::vector<Token> tokens;

    auto cur = source.begin();
    auto start = cur, end = cur;

    std::size_t line_num = 1, col_num = 1;
    std::size_t token_len = 0;

    TokenType tok_type;
    
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
            tok_type = TokenType::IDENTIFIER;

            if (value == "if")
                tok_type = TokenType::KEYWORD_IF;
            else if (value == "else")
                tok_type = TokenType::KEYWORD_ELSE;
            else if (value == "while")
                tok_type = TokenType::KEYWORD_WHILE;
            else if (value == "for")
                tok_type = TokenType::KEYWORD_FOR;
            else if (value == "return") 
                tok_type = TokenType::KEYWORD_RETURN;
            else if (value == "const")
                tok_type = TokenType::KEYWORD_CONST;
            else if (value == "typedef")
                tok_type = TokenType::KEYWORD_TYPEDEF;
            else if (type_keywords.contains(value))
                tok_type = TokenType::TYPE;

            tokens.emplace_back(tok_type, std::string_view{start, end}, line_num, col_num - (end-start), (end-start));

        } else {
            switch (*cur) {
                case '=':
                    if (peekchar(cur) == '=') {
                        tok_type = TokenType::EQUALS;
                        token_len = 2;
                    } else {
                        tok_type = TokenType::ASSIGN;
                        token_len = 1;
                    }
                    break;
                case '+':
                    if (peekchar(cur) == '=') {
                        tok_type = TokenType::ADD_ASSIGN;
                        token_len = 2;
                    } else if (peekchar(cur) == '+') {
                        tok_type = TokenType::INCREMENT;
                        token_len = 2;
                    } else {
                        tok_type = TokenType::ADD;
                        token_len = 1;
                    }
                    break;
                case '-':
                    if (peekchar(cur) == '=') {
                        tok_type = TokenType::SUBTRACT_ASSIGN;
                        token_len = 2;
                    }else if (peekchar(cur) == '-') {
                        tok_type = TokenType::DECREMENT;
                        token_len = 2;
                    } else if (peekchar(cur) == '>') {
                        tok_type = TokenType::ARROW;
                        token_len = 2;
                    } else {
                        tok_type = TokenType::SUBTRACT;
                        token_len = 1;
                    }
                    break;
                case '*':
                    if (peekchar(cur) == '=') {
                        tok_type = TokenType::MULTIPLY_ASSIGN;
                        token_len = 2;
                    } else {
                        tok_type = TokenType::MULTIPLY;
                        token_len = 1;
                    }
                    break;
                case '/':
                    if (peekchar(cur) == '=') {
                        tok_type = TokenType::DIVIDE_ASSIGN;
                        token_len = 2;
                    } else if (peekchar(cur) == '/') {
                        tok_type = TokenType::COMMENT;
                        token_len = 2;
                    } else {
                        tok_type = TokenType::DIVIDE;
                        token_len = 1;
                    }
                    break;
                case '%':
                    tok_type = TokenType::MODULUS;
                    token_len = 1;
                    break;
                case '(':
                    tok_type = TokenType::LPAREN;
                    token_len = 1;
                    break;
                case ')':
                    tok_type = TokenType::RPAREN;
                    token_len = 1;
                    break;
                case '{':
                    tok_type = TokenType::LBRACKET;
                    token_len = 1;
                    break;
                case '}':
                    tok_type = TokenType::RBRACKET;
                    token_len = 1;
                    break;
                case '[':
                    tok_type = TokenType::LBRACE;
                    token_len = 1;
                    break;
                case ']':
                    tok_type = TokenType::RBRACE;
                    token_len = 1;
                    break;
                case '>':
                    if (peekchar(cur) == '=') {
                        tok_type = TokenType::GREATER_EQUAL;
                        token_len = 2;
                    } else {
                        tok_type = TokenType::GREATER_THAN;
                        token_len = 1;
                    }
                    break;
                case '<':
                    if (peekchar(cur) == '=') {
                        tok_type = TokenType::LESS_EQUAL;
                        token_len = 2;
                    } else {
                        tok_type = TokenType::LESS_THAN;
                        token_len = 1;
                    }
                    break;
                case '&':
                    if (peekchar(cur) == '&') {
                        tok_type = TokenType::LOGICAL_AND;
                        token_len = 2;
                    } else {
                        tok_type = TokenType::ADDRESS_OF;
                        token_len = 1;
                    }
                    break;
                case '"':
                    token_len = 0;
                    start = cur;
                    while (*start != '"') {
                        start++;
                        token_len++;
                    }
                    token_len++;
                    tok_type = TokenType::STRING_LITERAL;
                    break;
                case ';':
                    tok_type = TokenType::SEMICOLON;
                    token_len = 1;
                    break;
                case ':':
                    tok_type = TokenType::COLON;
                    token_len = 1;
                    break;
                case ',':
                    tok_type = TokenType::COMMA;
                    token_len = 1;
                    break;
                case '.':
                    tok_type = TokenType::DOT;
                    token_len = 1;
                    break;
                case '!':
                    if (peekchar(cur) == '=') {
                        tok_type = TokenType::NOT_EQUAL;
                        token_len = 2;
                    } else {
                        tok_type = TokenType::LOGICAL_NOT;
                        token_len = 1;
                    }
                    break;
                case '|':
                    if (peekchar(cur) == '|') {
                        tok_type = TokenType::LOGICAL_OR;
                        token_len = 2;
                    } else {
                        tok_type = TokenType::PIPE;
                        token_len = 1;
                    }
                    break;
                case EOF:
                    tok_type = TokenType::END_OF_FILE;
                    break;
                default:
                    throw std::runtime_error("Invalid token: " + std::string{*cur});
            }

            // just for debugging we are going to provide the lexeme for symbols
            std::string_view lexeme{cur, token_len};
            cur += token_len;
            col_num += token_len;
            tokens.emplace_back(tok_type, lexeme, line_num, col_num, token_len);
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
