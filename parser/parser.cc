#include <iostream>
#include <utility>
#include <map>

#include "lexer/token.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "parser/ast.h"

void Parser::main_parse()
{
    // uint32_t scope = 0;
    // or something like...
    // auto ctx = ParseContext::____;

    eat_token();

    while (cur_token.type != TokenType::END_OF_FILE) {
        switch (cur_token.type) {
            case TokenType::TYPE: {
                break;
            }
            case TokenType::KEYWORD_CONST: {
                break;
            }
            case TokenType::KEYWORD_FUNCTION: {
                break;
            }
            case TokenType::KEYWORD_STRUCT: {
                break;
            }
            default:
                break;
        }
        eat_token();
    }
}

// e.g.)
// const int32 x = 10;
// int32 x = 10;
// int32 x;

DeclRef Parser::parse_var_decl()
{

}
