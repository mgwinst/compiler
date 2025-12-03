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
    auto comp_unit = ast_.emplace_decl<CompilationUnitDecl>(std::move(source_file_.file_path));

    eat_token();
    while (cur_token_.type_ != TokenType::END_OF_FILE) {
        switch (cur_token_.type_) {
            case TokenType::KEYWORD_CONST: {
                eat_token(); // eat the const, handlers don't care once they called, don't check again in a parse handler, we check here once.
                auto const_decl = parse_const_decl();               
                if (!const_decl)
                    panic(const_decl.error());
                else
                    std::get<CompilationUnitDecl>(ast_.decls_[comp_unit]).decls_.push_back(*const_decl); // wrap this?

                break;
            }
            case TokenType::KEYWORD_FUNCTION: {
                bool is_const = false;
                auto func_decl = parse_func_decl(is_const);
                if (!func_decl)
                    panic(func_decl.error());
                else
                    std::get<CompilationUnitDecl>(ast_.decls_[comp_unit]).decls_.push_back(*func_decl);
                break;
            }
            case TokenType::TYPE: {
                auto var = parse_var_decl();
                if (!var)
                    panic(var.error());
                else 
                    std::get<CompilationUnitDecl>(ast_.decls_[comp_unit]).decls_.push_back(*var);
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

std::expected<DeclRef, ParseError> Parser::parse_const_decl() noexcept
{
    if (is_cur_token(TokenType::KEYWORD_FUNCTION)) {
        bool is_const = true;
        return parse_func_decl(is_const);
    } else if (is_cur_token(TokenType::TYPE)) {
        return parse_const_var_decl();
    } else {
        return std::unexpected{ SyntaxError{cur_token_} };
    }
}

std::expected<DeclRef, ParseError> Parser::parse_var_decl() noexcept
{
    std::string type{};
    std::string name{};

    if (!(is_cur_token(TokenType::TYPE) && is_next_token(TokenType::IDENTIFIER))) {
        return std::unexpected{ SyntaxError{ cur_token_ } };
    } else {
        type = cur_token_.lexeme_;
        name = next_token_.lexeme_;
        eat_token();
        eat_token();
    }

    if (is_cur_token(TokenType::SEMICOLON)) {
        return ast_.emplace_decl<VarDecl>(std::move(type), std::move(name));
    } else if (is_cur_token(TokenType::EQUAL)) {
        eat_token();
        auto expr = parse_expr(0);
        if (!expr) 
            return std::unexpected(expr.error());
        return ast_.emplace_decl<VarDecl>(std::move(type), std::move(name), *expr);
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
        type = cur_token_.lexeme_;
        name = next_token_.lexeme_;
        eat_token();
        eat_token();
    }

    if (!is_cur_token(TokenType::EQUAL)) {
        return std::unexpected{ SyntaxError{ cur_token_ } };
    } else {
        // make sure that there is expr here, don't handle "if next token ; -> error in expr handlers, do it here because unique to this node type"

        eat_token();
        auto expr = parse_expr(0);
        if (!expr)
            return std::unexpected{ expr.error() };
        return ast_.emplace_decl<ConstVarDecl>(std::move(type), std::move(name), *expr);
    }
}

std::expected<DeclRef, ParseError> Parser::parse_param_decl() noexcept
{
    return -1;
}

std::expected<DeclRef, ParseError> Parser::parse_func_decl(bool is_const) noexcept
{
    return -1;
}

std::expected<DeclRef, ParseError> Parser::parse_struct_decl() noexcept
{
    return -1;
}

int lbp(const Token& token) noexcept
{
    if (auto prec = token_prec.find(token.type_); prec != token_prec.end()) {
        return prec->second;
    } else {
        return -1; // handle this error better
    }
}

std::expected<ExprRef, ParseError> Parser::nud(const Token& token)
{
    switch (token.type_) {
        case TokenType::IDENTIFIER: {
            return ast_.emplace_expr<ReferenceExpr>(token.lexeme_.data());
        }
        case TokenType::RBRACE: {
            break;
        }
        default:
        
            break;
    }

    return -1;
}

std::expected<ExprRef, ParseError> Parser::led(const Token& token, const ExprRef left)
{
    switch (token.type_) {
        case TokenType::EQUAL: {
            eat_token();
            return ast_.emplace_expr<BinaryExpr>(token.lexeme_.data(), left, *parse_expr(lbp(token)));
        }
        default:
            break;
    }

    return -1;
}

std::expected<ExprRef, ParseError> Parser::parse_expr(int rbp = 0) noexcept
{
    auto left = nud(cur_token_);
    
    eat_token();

    while (rbp < lbp(cur_token_)) {
        left = led(cur_token_, *left);
    }

    return left;
}

/*
    // parse_params() for () in func declaration

    fn main() -> int
    {
        a = (b + c) * d;

        arr[a + b] = 10;

        if (a == b) {
            a = 20;
        }
        
    }
*/