#include <iostream>
#include <utility>
#include <map>

#include "lexer/token.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "parser/ast.hpp"

void Parser::eat_token()
{
    cur_token = lexer.get_token();
    next_token = lexer.peek_token();
}

void Parser::expect(TokenType token_type)
{
    if (cur_token.type != token_type) {
        panic_mode();
    }
}

void Parser::panic_mode()
{
    diagnostics.add_error(SyntaxError{cur_token});

    while (cur_token.type != TokenType::RBRACE && cur_token.type != TokenType::KEYWORD_FUNCTION) {
        eat_token();
    }
}

void Parser::main_parse()
{
    while (cur_token.type != TokenType::END_OF_FILE) {
        switch (cur_token.type) {
            case TokenType::TYPE:
            case TokenType::KEYWORD_CONST: {
                ast.add_decl(parse_var_decl());
                break;
            }
            case TokenType::KEYWORD_FUNCTION: {
                ast.add_decl(parse_func_decl());
                break;
            }
            case TokenType::KEYWORD_STRUCT: {
                // ast.add_decl(parse_struct_decl());
                break;
            }
            default:
                break;
        }

        eat_token();
    }
}

std::optional<Decl> Parser::parse_var_decl()
{
    std::string type{};
    std::string name{};
    bool is_const = false;

    if (cur_token.type == TokenType::KEYWORD_CONST) {
        is_const = true;
        eat_token();
    }

    if (cur_token.type == TokenType::TYPE) {
        type = *cur_token.lexeme;
        eat_token();
    }

    expect(TokenType::IDENTIFIER);

    name = *cur_token.lexeme;
    eat_token();

    if (cur_token.type == TokenType::SEMICOLON) {
        return VarDecl{is_const, type, name};
    } else if (cur_token.type == TokenType::EQUAL) {
        eat_token();
        return VarDecl(is_const, type, name /* parse_expr() */);
    }
}

