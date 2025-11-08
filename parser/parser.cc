#include <utility>
#include <map>

#include "lexer/token.h"
#include "lexer/lexer.h"
#include "parser.h"
#include "ast.h"

std::unordered_map<TokenType, uint16_t> binary_op_precedence {
    {TokenType::STAR, 3},
    {TokenType::SLASH, 3},
    {TokenType::PERCENT, 3},

    {TokenType::PLUS, 4},
    {TokenType::MINUS, 4},

    {TokenType::LESS, 6},
    {TokenType::LESS_EQUAL, 6},
    {TokenType::GREATER, 6},
    {TokenType::GREATER_EQUAL, 6},

    {TokenType::EQUAL_EQUAL, 7},
    {TokenType::BANG_EQUAL, 7},

    {TokenType::AMPERSAND, 8},
    {TokenType::CARROT, 9},
    {TokenType::PIPE, 10},
    {TokenType::AMPERSAND_AMPERSAND, 11},
    {TokenType::PIPE_PIPE, 12},

    {TokenType::EQUAL, 14},
    {TokenType::PLUS_EQUAL, 14},
    {TokenType::MINUS_EQUAL, 14},
    {TokenType::STAR_EQUAL, 14},
    {TokenType::SLASH_EQUAL, 14},
    {TokenType::PERCENT_EQUAL, 14},

    {TokenType::COMMA, 15},
};
