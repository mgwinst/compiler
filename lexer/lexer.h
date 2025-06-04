#include <cstdint>
#include <string>
#include <optional>
#include <vector>

#define NUM_STATES 64
#define NUM_CHAR_CATEGORIES 64

enum class TokenType : uint16_t {
    IDENTIFIER = 128,

    NUMERIC_LITERAL,
    STRING_LITERAL,

    TYPE,

    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,
    KEYWORD_FOR,
    KEYWORD_RETURN,
    KEYWORD_CONST,
    KEYWORD_TYPEDEF,

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

    ADDRESS_OF,
    ARROW,

    COMMENT,
    END_OF_FILE,
    INVALID,

    // handle bitwise later
};

enum class State : uint16_t {
    INITIAL,

    ID_START,
    ID_CHAR,
    ID_ACCEPT,

    KEYWORD_ACCEPT,

    DIGIT_START,
    DIGIT_CHAR,
    DIGIT_ACCEPT,

    STRING_LITERAL_START,
    STRING_LITERAL_CHAR,
    STRING_LITERAL_ACCEPT,

    ASSIGN_ACCEPT,
    EQUALS_ACCEPT,

    ADD_ACCEPT,
    ADD_ASSIGN_ACCEPT,
    SUB_ACCEPT,
    SUB_ASSIGN_ACCEPT,
    INCREMENT_ACCEPT,
    DECREMENT_ACCEPT,
    MULTIPLY_ACCEPT,
    MULTIPLY_ASSIGN_ACCEPT,
    DIVIDE_ACCEPT,
    DIVIDE_ASSIGN_ACCEPT,

    LESS_THAN_ACCEPT,
    LESS_EQUAL_ACCEPT,
    GREATER_THAN_ACCEPT,
    GREATER_EQUAL_ACCEPT,

    LOGICAL_AND_ACCEPT,
    LOGICAL_OR_ACCEPT,
    LOGICAL_NOT_ACCEPT,

    LPAREN_ACCEPT,
    RPAREN_ACCEPT,
    LBRACKET_ACCEPT,
    RBRACKET_ACCEPT,
    LBRACE_ACCEPT,
    RBRACE_ACCEPT,
    SEMICOLON_ACCEPT,
    COLON_ACCEPT,
    COMMA_ACCEPT,
    DOT_ACCEPT,
    EXCLA_ACCEPT,
    QUESTION_ACCEPT,
    CARROT_ACCEPT,
    UNDERSCORE_ACCEPT,
    BAR_ACCEPT,
    COMMENT_ACCEPT,

    ADDRESSOF_ACCEPT,
    ARROW_ACCEPT,

    INVALID
};

enum class Category : uint16_t {
    WHITESPACE,
    LETTER,
    DIGIT,
    EQUAL,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    LESS_THAN,
    GREATER_THAN,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    AMPERSAND,
    SINGLE_QUOTE,
    DOUBLE_QUOTE,
    SEMICOLON,
    COLON,
    COMMA,
    DOT,
    CARROT,
    UNDERSCORE,
    EXCLA_POINT,
    QUESTION,
    BAR,
    OTHER
};

struct Token {
    TokenType type;
    std::optional<std::string_view> lexeme;
    std::size_t line_number;
    std::size_t column_number;
    std::size_t length;

    Token(TokenType t, std::string_view l, std::size_t ln, std::size_t cn, std::size_t len) :
        type{t}, lexeme{l}, line_number{ln}, column_number{cn}, length{len} {}

    ~Token() {}

    Token(const Token&) = delete;
    Token& operator=(const Token&) = delete;
    Token(Token&&) = delete;
    Token& operator=(Token&&) = delete;

    std::string to_string() const {
        return std::to_string(static_cast<int>(type)) + " " + std::string{lexeme.value()};
    }
};

[[nodiscard]] std::vector<Token> lex(std::string_view);

std::optional<Token> transition(State&, const char);

void test_lex(std::string);
