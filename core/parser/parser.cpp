#include <iostream>
#include <utility>
#include <expected>
#include <map>

#include "lexer/token.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "parser/ast.hpp"

Parser::Parser(SourceFile& source_file) noexcept :
    source_file_{ std::move(source_file) },
    lexer_{ source_file_.data },
    diagnostics_{},
    ast_{} {}

void Parser::eat_token() noexcept
{
    cur_token_ = lexer_.get_token();
    next_token_ = lexer_.peek_token();
}

bool Parser::is_cur_token(TokenType token_type) const noexcept
{
    return cur_token_.type_ == token_type ? true : false;
}

bool Parser::is_next_token(TokenType token_type) const noexcept
{
    return next_token_.type_ == token_type ? true : false;
}

void Parser::panic(const ParseError& error) noexcept
{
    diagnostics_.add_error(error);

    while (!is_cur_token(TokenType::RBRACE) &&
           !is_cur_token(TokenType::KEYWORD_FUNCTION) && 
           !is_cur_token(TokenType::END_OF_FILE)) {
        eat_token();
    }
}

DeclRef Parser::parse_compilation_unit() noexcept
{
    auto comp_unit = ast_.add_decl<CompilationUnitDecl>(std::move(source_file_.file_path));

    while (cur_token_.type_ != TokenType::END_OF_FILE) {
        switch (cur_token_.type_) {
            case TokenType::TYPE: {
                auto var = parse_var_decl();
                if (!var) {
                    panic(var.error());
                } else {
                    std::get<CompilationUnitDecl>(ast_.decls[comp_unit]).decls_.push_back(*var);
                }
                break;
            }
            case TokenType::KEYWORD_CONST: {
                auto cvar = parse_const_var_decl();
                if (!cvar)
                    panic(cvar.error());
                else
                    //decls.push_back(*cvar);
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

    return comp_unit;
}

std::expected<DeclRef, ParseError> Parser::parse_var_decl() noexcept
{
    std::string type{};
    std::string name{};

    if (!(is_cur_token(TokenType::TYPE) && is_next_token(TokenType::IDENTIFIER))) {
        return std::unexpected{ SyntaxError{ cur_token_ } };
    } else {
        type = *cur_token_.lexeme_;
        eat_token();
        name = *cur_token_.lexeme_;
        eat_token();
    }

    if (is_cur_token(TokenType::SEMICOLON)) {
        return ast_.add_decl<VarDecl>(std::move(type), std::move(name));
    } else if (is_cur_token(TokenType::EQUAL)) {
        /*
        eat_token();
        auto expr = parse_expr(0);
        if (!expr) 
            return std::unexpected(expr.error());
        return ast_.add_decl(VarDecl(type_, name, *expr));
        */
    } else {
        return std::unexpected{ SyntaxError{ cur_token_ } };
    }
}

std::expected<DeclRef, ParseError> Parser::parse_const_var_decl() noexcept
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
        type = *cur_token_.lexeme_;
        eat_token();
        name = *cur_token_.lexeme_;
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
        return ast_.add_decl(ConstVarDecl(type_, name, *expr));
        */
    }
}

std::expected<ExprRef, ParseError> Parser::parse_expr(int min_prec) noexcept
{

}