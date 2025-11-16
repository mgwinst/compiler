#include <iostream>
#include <utility>
#include <map>

#include "lexer/token.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "parser/ast.h"

void Parser::main_parse()
{
    eat_token();

    while (cur_token.type != TokenType::END_OF_FILE) {
        switch (cur_token.type) {
            case TokenType::TYPE:
            case TokenType::KEYWORD_CONST: {
                DeclRef ref = ast.add_decl(parse_var_decl());
                break;
            }
            case TokenType::KEYWORD_FUNCTION: {
                DeclRef ref = ast.add_decl(parse_func_decl());
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

Decl Parser::parse_var_decl()
{
    std::string type{};
    std::string ident{};
    bool is_const = false;

    if (cur_token.type == TokenType::KEYWORD_CONST) {
        is_const = true;
        eat_token();
    }

    if (cur_token.type == TokenType::TYPE) {
        type = *cur_token.lexeme;
        eat_token();
    }

    if (cur_token.type != TokenType::IDENTIFIER) {
        // syntax error
    }

    ident = *cur_token.lexeme;
    eat_token();

    if (cur_token.type == TokenType::SEMICOLON) {
        return VarDecl{is_const, type, ident};
    } else if (cur_token.type == TokenType::EQUAL) {
        eat_token();
        return VarDecl(is_const, type, ident /* parse_expr() */);
    }
}

Decl Parser::parse_func_decl()
{
    std::string ident;
    std::string return_type;
    std::vector<std::pair<std::string, std::string>> params;

    eat_token();

    if (cur_token.type != TokenType::IDENTIFIER) {
        // syntax error
    }

    ident = *cur_token.lexeme;

    eat_token();

    if (cur_token.type != TokenType::LPAREN) {
        // syntax error
    }

    if (next_token.type == TokenType::RPAREN) {
        eat_token();
        eat_token();
    }

    eat_token();
    
    while (true) {
        if (cur_token.type == TokenType::TYPE && next_token.type == TokenType::IDENTIFIER) {
            params.emplace_back(*cur_token.lexeme, *next_token.lexeme);
            eat_token();
            eat_token();
        } else {
            // syntax error
        }

        if (cur_token.type == TokenType::COMMA) {
            eat_token();
        } else if (cur_token.type == TokenType::RPAREN) {
            eat_token();
            break;
        } else {
            // syntax error
        }
    }

    if (cur_token.type != TokenType::ARROW) {
        // syntax error
    }

    eat_token();   

    if (cur_token.type != TokenType::TYPE) {
        // syntax error
    }

    return_type = *cur_token.lexeme;

    eat_token();

    if (cur_token.type != TokenType::LBRACE) {
        // syntax error
    }

    auto stmtref = ast.add_stmt(parse_block_stmt());

    return FuncDecl{ident, return_type, params, stmtref};
}


