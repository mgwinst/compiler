#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <cctype>
#include <fstream>
#include <iostream>

#include "lexer.h"

namespace {
    const std::unordered_set<std::string_view> type_keywords = {
        "int", "int8", "int16", "int32", "int64",
        "uint", "uint8", "uint16", "uint32", "uint64",
        "float", "float16", "float32", "float64",
        "char", "string", "struct", "bool", "void"
    };

    const std::unordered_map<State, Token> state_to_token = {
        {}
    }

    Category char_to_category[256];

    State transition_table[NUM_STATES][NUM_CHAR_CATEGORIES];
}

void init_char_categories() {
    for (int i = 0; i < 256; i++) {
        char_to_category[i] = Category::OTHER;
    }
    
    for (char c = '0'; c <= '9'; c++) {
        char_to_category[(int)c] = Category::DIGIT;
    }

    for (char c = 'A'; c <= 'Z'; c++) {
        char_to_category[(int)c] = Category::LETTER;
    }

    for (char c = 'a'; c <= 'z'; c++) {
        char_to_category[(int)c] = Category::LETTER;
    }

    char_to_category[(int)'='] = Category::EQUAL;
    char_to_category[(int)'+'] = Category::PLUS;
    char_to_category[(int)'-'] = Category::MINUS;
    char_to_category[(int)'*'] = Category::STAR;
    char_to_category[(int)'/'] = Category::SLASH;
    char_to_category[(int)'<'] = Category::LESS_THAN;
    char_to_category[(int)'>'] = Category::GREATER_THAN;
    char_to_category[(int)'('] = Category::LPAREN;
    char_to_category[(int)')'] = Category::RPAREN;
    char_to_category[(int)'{'] = Category::LBRACKET;
    char_to_category[(int)'}'] = Category::RBRACKET;
    char_to_category[(int)'['] = Category::LBRACE;
    char_to_category[(int)']'] = Category::RBRACE;
    char_to_category[(int)'&'] = Category::AMPERSAND,
    char_to_category[(int)'"'] = Category::DOUBLE_QUOTE;
    char_to_category[(int)';'] = Category::SEMICOLON;
    char_to_category[(int)':'] = Category::COLON;
    char_to_category[(int)','] = Category::COMMA;
    char_to_category[(int)'.'] = Category::DOT;
    char_to_category[(int)'!'] = Category::EXCLA_POINT;
    char_to_category[(int)'?'] = Category::QUESTION;
    char_to_category[(int)'^'] = Category::CARROT;
    char_to_category[(int)'_'] = Category::UNDERSCORE;

    // all Categoroy::WHITESPACE or unique?
    char_to_category[(int)' '] = Category::WHITESPACE;
    char_to_category[(int)'\n'] = Category::WHITESPACE;
    char_to_category[(int)'\r'] = Category::WHITESPACE;
    char_to_category[(int)'\t'] = Category::WHITESPACE;
}

void init_transition_table() {
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            transition_table[i][j] = State::INVALID;
        }
    }

    transition_table[(int)State::INITIAL][(int)Category::LETTER] = State::ID_START; 
    transition_table[(int)State::ID_START][(int)Category::LETTER] = State::ID_CHAR; 
    transition_table[(int)State::ID_CHAR][(int)Category::LETTER] = State::ID_CHAR;
    transition_table[(int)State::ID_CHAR][(int)Category::DIGIT] = State::ID_CHAR;
    transition_table[(int)State::ID_CHAR][(int)Category::WHITESPACE] = State::ID_ACCEPT;

    transition_table[(int)State::INITIAL][(int)Category::DIGIT] = State::DIGIT_START;
    transition_table[(int)State::DIGIT_START][(int)Category::DIGIT] = State::DIGIT_CHAR;
    transition_table[(int)State::DIGIT_CHAR][(int)Category::DIGIT] = State::DIGIT_CHAR;
    transition_table[(int)State::DIGIT_CHAR][(int)Category::WHITESPACE] = State::DIGIT_ACCEPT;

    transition_table[(int)State::INITIAL][(int)Category::DOUBLE_QUOTE] = State::STRING_LITERAL_START; 
    transition_table[(int)State::STRING_LITERAL_START][(int)Category::LETTER] = State::STRING_LITERAL_CHAR; 
    transition_table[(int)State::STRING_LITERAL_START][(int)Category::DIGIT] = State::STRING_LITERAL_CHAR; 
    transition_table[(int)State::STRING_LITERAL_CHAR][(int)Category::LETTER] = State::STRING_LITERAL_CHAR; 
    transition_table[(int)State::STRING_LITERAL_CHAR][(int)Category::DIGIT] = State::STRING_LITERAL_CHAR; 
    transition_table[(int)State::STRING_LITERAL_CHAR][(int)Category::WHITESPACE] = State::STRING_LITERAL_CHAR; 
    transition_table[(int)State::STRING_LITERAL_CHAR][(int)Category::DOUBLE_QUOTE] = State::STRING_LITERAL_ACCEPT; 

    transition_table[(int)State::INITIAL][(int)Category::EQUAL] = State::ASSIGN_ACCEPT;
    transition_table[(int)State::INITIAL][(int)Category::PLUS] = State::ADD_ACCEPT;
    transition_table[(int)State::INITIAL][(int)Category::MINUS] = State::SUB_ACCEPT;
    transition_table[(int)State::INITIAL][(int)Category::STAR] = State::MULTIPLY_ACCEPT;
    transition_table[(int)State::INITIAL][(int)Category::SLASH] = State::DIVIDE_ACCEPT;
    transition_table[(int)State::INITIAL][(int)Category::LESS_THAN] = State::LESS_THAN_ACCEPT;
    transition_table[(int)State::INITIAL][(int)Category::GREATER_THAN] = State::GREATER_THAN_ACCEPT;
    transition_table[(int)State::INITIAL][(int)Category::LPAREN] = State::LPAREN_ACCEPT;
    transition_table[(int)State::INITIAL][(int)Category::RPAREN] = State::RPAREN_ACCEPT;
    transition_table[(int)State::INITIAL][(int)Category::LBRACKET] = State::LBRACKET_ACCEPT;
    transition_table[(int)State::INITIAL][(int)Category::RBRACKET] = State::RBRACKET_ACCEPT;
    transition_table[(int)State::INITIAL][(int)Category::LBRACE] = State::LBRACE_ACCEPT;
    transition_table[(int)State::INITIAL][(int)Category::RBRACE] = State::RBRACE_ACCEPT; 
    transition_table[(int)State::INITIAL][(int)Category::AMPERSAND] = State::ADDRESSOF_ACCEPT; 
    transition_table[(int)State::INITIAL][(int)Category::SEMICOLON] = State::SEMICOLON_ACCEPT;
    transition_table[(int)State::INITIAL][(int)Category::COLON] = State::COLON_ACCEPT; 
    transition_table[(int)State::INITIAL][(int)Category::COMMA] = State::COMMA_ACCEPT;
    transition_table[(int)State::INITIAL][(int)Category::DOT] = State::DOT_ACCEPT; 
    transition_table[(int)State::INITIAL][(int)Category::EXCLA_POINT] = State::EXCLA_ACCEPT;
    transition_table[(int)State::INITIAL][(int)Category::QUESTION] = State::QUESTION_ACCEPT;
    transition_table[(int)State::INITIAL][(int)Category::CARROT] = State::CARROT_ACCEPT; 
    transition_table[(int)State::INITIAL][(int)Category::UNDERSCORE] = State::UNDERSCORE_ACCEPT;
    transition_table[(int)State::INITIAL][(int)Category::BAR] = State::BAR_ACCEPT;

    transition_table[(int)State::ASSIGN_ACCEPT][(int)Category::EQUAL] = State::EQUALS_ACCEPT;
    transition_table[(int)State::ADD_ACCEPT][(int)Category::PLUS] = State::INCREMENT_ACCEPT;
    transition_table[(int)State::ADD_ACCEPT][(int)Category::EQUAL] = State::ADD_ASSIGN_ACCEPT;
    transition_table[(int)State::SUB_ACCEPT][(int)Category::MINUS] = State::DECREMENT_ACCEPT;
    transition_table[(int)State::SUB_ACCEPT][(int)Category::EQUAL] = State::SUB_ASSIGN_ACCEPT;
    transition_table[(int)State::MULTIPLY_ACCEPT][(int)Category::EQUAL] = State::MULTIPLY_ASSIGN_ACCEPT;
    transition_table[(int)State::DIVIDE_ACCEPT][(int)Category::EQUAL] = State::DIVIDE_ASSIGN_ACCEPT;
    transition_table[(int)State::LESS_THAN_ACCEPT][(int)Category::EQUAL] = State::LESS_EQUAL_ACCEPT;
    transition_table[(int)State::GREATER_THAN_ACCEPT][(int)Category::EQUAL] = State::GREATER_EQUAL_ACCEPT;
    transition_table[(int)State::ADDRESSOF_ACCEPT][(int)Category::AMPERSAND] = State::LOGICAL_AND_ACCEPT;
    transition_table[(int)State::EXCLA_ACCEPT][(int)Category::EQUAL] = State::LOGICAL_NOT_ACCEPT;
    transition_table[(int)State::BAR_ACCEPT][(int)Category::BAR] = State::LOGICAL_OR_ACCEPT;
    transition_table[(int)State::DIVIDE_ACCEPT][(int)Category::SLASH] = State::COMMENT_ACCEPT;
    transition_table[(int)State::SUB_ACCEPT][(int)Category::GREATER_THAN] = State::ARROW_ACCEPT;
}



std::optional<Token> transition(State& current_state, const char* cur_char) {
    auto new_state = transition_table[(int)current_state][(int)(*cur_char)];
    
}


[[nodiscard]] std::vector<Token> lex2(std::string_view source) {
    std::vector<Token> tokens;   

    auto cur = source.begin();
    auto start = cur, end = cur;

    State current_state = State::INITIAL;

    while (cur != source.end()) {
        transition(current_state, cur);
    }

    return tokens;
}

void test_lex(std::string path) {
    std::ifstream file{path};
    if (!file.is_open())
        throw std::runtime_error("can't open file: sample_program.c");

    std::string source_text;
    std::string line;

    while (getline(file, line)) {
        source_text += line;
    }

    auto tokens = lex(source_text);

    for (const auto& tok : tokens) {
        std::cout << tok.to_string() << '\n';
    }

}

int main() {
    // test_lex("../test/sample_program.c");
    // test_lex("../test/sample_tokens.txt");


}