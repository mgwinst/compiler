#include <iostream>
#include <utility>
#include <expected>
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

bool Parser::is_cur_token(TokenType token_type)
{
    return cur_token.type == token_type ? true : false;
}

void Parser::panic(const ParseError& error)
{
    diagnostics.add_error(error);

    while (!is_cur_token(TokenType::RBRACE) &&
           !is_cur_token(TokenType::KEYWORD_FUNCTION) && 
           !is_cur_token(TokenType::END_OF_FILE)) {
        eat_token();
    }
}

void Parser::main_parse()
{
    while (cur_token.type != TokenType::END_OF_FILE) {
        switch (cur_token.type) {
            case TokenType::TYPE:
            case TokenType::KEYWORD_CONST: {
                auto var = parse_var_decl();
                if (!var.has_value())
                    panic(var.error());
                else
                    ast.add_decl(var.value());
                break;
            }
            case TokenType::KEYWORD_FUNCTION: {
                // auto func = parse_func_decl();
                //ast.add_decl();
                break;
            }
            case TokenType::KEYWORD_STRUCT: {
                // auto s = parse_struct_decl(); 
                // ast.add_decl();
                break;
            }
            default:
                break;
        }

        eat_token();
    }
}

std::expected<Decl, ParseError> Parser::parse_var_decl()
{
    std::string type{};
    std::string name{};
    bool is_const = false;

    if (is_cur_token(TokenType::KEYWORD_CONST)) {
        is_const = true;
        eat_token();
    }

    if (is_cur_token(TokenType::TYPE)) {
        type = *cur_token.lexeme;
        eat_token();
    }

    if (is_cur_token(TokenType::IDENTIFIER)) {
        name = *cur_token.lexeme;
        eat_token();
    } else {
        return std::unexpected(SyntaxError{ cur_token });
    }

    if (is_cur_token(TokenType::SEMICOLON)) {
        return VarDecl{is_const, type, name};
    } else if (is_cur_token(TokenType::EQUAL)) {
        eat_token();
        return VarDecl(is_const, type, name /* parse_expr() */);
    }
}

