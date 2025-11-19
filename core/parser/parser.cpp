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
    cur_token_ = lexer_.get_token();
    next_token_ = lexer_.peek_token();
}

bool Parser::is_cur_token(TokenType token_type)
{
    return cur_token_.type == token_type ? true : false;
}

bool Parser::is_next_token(TokenType token_type)
{
    return next_token_.type == token_type ? true : false;
}

void Parser::panic(const ParseError& error)
{
    diagnostics_.add_error(error);

    while (!is_cur_token(TokenType::RBRACE) &&
           !is_cur_token(TokenType::KEYWORD_FUNCTION) && 
           !is_cur_token(TokenType::END_OF_FILE)) {
        eat_token();
    }
}

void Parser::main_parse()
{
    while (cur_token_.type != TokenType::END_OF_FILE) {
        switch (cur_token_.type) {
            case TokenType::TYPE: {
                auto var = parse_var_decl();
                if (!var.has_value())
                    panic(var.error());
                break;
            }
            case TokenType::KEYWORD_CONST: {
                auto cvar = parse_const_var_decl();
                if (!cvar.has_value())
                    panic(cvar.error());
                break;
            }
            case TokenType::KEYWORD_FUNCTION: {
                // auto func = parse_func_decl();
                //ast_.add_decl();
                break;
            }
            case TokenType::KEYWORD_STRUCT: {
                // auto s = parse_struct_decl(); 
                // ast_.add_decl();
                break;
            }
            default:
                break;
        }
        eat_token();
    }
}

std::expected<DeclRef, ParseError> Parser::parse_var_decl()
{
    std::string type{};
    std::string name{};

    if (!(is_cur_token(TokenType::TYPE) && is_next_token(TokenType::IDENTIFIER))) {
        return std::unexpected{ SyntaxError{ cur_token_ } };
    } else {
        type = *cur_token_.lexeme;
        eat_token();
        name = *cur_token_.lexeme;
        eat_token();
    }

    if (is_cur_token(TokenType::SEMICOLON)) {
        eat_token();
        return ast_.add_decl(VarDecl{type, name});
    } else if (is_cur_token(TokenType::EQUAL)) {
        /*
        eat_token();
        auto expr = parse_expr(0);
        if (!expr) 
            return std::unexpected(expr.error());
        return ast_.add_decl(VarDecl(type, name, *expr));
        */
    } else {
        return std::unexpected{ SyntaxError{ cur_token_ } };
    }
}

std::expected<DeclRef, ParseError> Parser::parse_const_var_decl()
{
    std::string type{};
    std::string name{};

    if (!is_cur_token(TokenType::KEYWORD_CONST)) {
        return std::unexpected{ SyntaxError{ cur_token_ } };
    }

    eat_token();

    if (!(is_cur_token(TokenType::TYPE) && is_next_token(TokenType::IDENTIFIER))) {
        return std::unexpected{ SyntaxError{ cur_token_ } };
    } else {
        type = *cur_token_.lexeme;
        eat_token();
        name = *cur_token_.lexeme;
        eat_token();
    }

    if (!is_cur_token(TokenType::EQUAL)) {
        return std::unexpected{ SyntaxError{ cur_token_ } };
    } else {
        /*
        eat_token();
        auto expr = parse_expr(0);
        if (!expr)
            return std::unexpected{ expr.error() };
        return ast_.add_decl(ConstVarDecl(type, name, *expr));
        */
    }
}