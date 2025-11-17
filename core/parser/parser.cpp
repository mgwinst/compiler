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

bool Parser::is_next_token(TokenType token_type)
{
    return next_token.type == token_type ? true : false;
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
            case TokenType::TYPE: {
                auto var = parse_var_decl();
                if (!var.has_value())
                    panic(var.error());
                else
                    ast.add_decl(var.value());
                break;
            }
            case TokenType::KEYWORD_CONST: {
                auto cvar = parse_const_var_decl();
                if (!cvar.has_value())
                    panic(cvar.error());
                else
                    ast.add_decl(cvar.value());
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

    if (!(is_cur_token(TokenType::TYPE) && is_next_token(TokenType::IDENTIFIER))) {
        return std::unexpected(SyntaxError{ cur_token });
    } else {
        type = *cur_token.lexeme;
        eat_token();
        name = *cur_token.lexeme;
        eat_token();
    }

    if (is_cur_token(TokenType::SEMICOLON)) {
        return VarDecl{type, name};
    } else if (is_cur_token(TokenType::EQUAL)) {
        eat_token();
        return VarDecl(type, name /* parse_expr() */);
    } else {
        return std::unexpected(SyntaxError{ cur_token });
    }
}

std::expected<Decl, ParseError> Parser::parse_const_var_decl()
{
    std::string type{};
    std::string name{};

    if (!is_cur_token(TokenType::KEYWORD_CONST)) {
        return std::unexpected(SyntaxError{ cur_token });
    }

    eat_token();

    if (!(is_cur_token(TokenType::TYPE) && is_next_token(TokenType::IDENTIFIER))) {
        return std::unexpected(SyntaxError{ cur_token });
    } else {
        type = *cur_token.lexeme;
        eat_token();
        name = *cur_token.lexeme;
        eat_token();
    }

    if (!is_cur_token(TokenType::EQUAL)) {
        return std::unexpected(SyntaxError{ cur_token });
    } else {
        eat_token();
        return ConstVarDecl(type, name /* parse_expr() */);
    }
}

