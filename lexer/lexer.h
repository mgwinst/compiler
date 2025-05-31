#include <cstdint>
#include <string>

enum class TokenType : uint16_t {
    IDENTIFIER = 128,

    // INTEGER_CONSTANT,
    // FLOAT_CONSTANT,
    NUMERIC_LITERAL,
    STRING_LITERAL,

    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,
    KEYWORD_FOR,
    KEYWORD_RETURN,
    KEYWORD_CONST,
    
    ADD,
    SUBTRACT,
    MULTIPLY,
    DEREFERENCE = MULTIPLY,
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
    LBRACKET,
    RBRACKET,
    LBRACE,
    RBRACE,
    SINGLE_QUOTE,
    DOUBLE_QUOTE,
    SEMICOLON,
    COLON,
    COMMA,
    DOT,
    QUESTION,

    ADDRESS_OF,
    ARROW,

    END_OF_FILE,
    INVALID,   

    // handle bitwise later
};

struct Token {
    TokenType type;
    std::string value;

    std::string to_string() const {
        return std::to_string(static_cast<int>(type)) + " " + value;
    }
};

[[nodiscard]] std::vector<Token> lex(std::string_view);

void test_lex(std::string);
