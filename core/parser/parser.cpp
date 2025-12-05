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

const Token& Parser::cur_token() const noexcept
{
    return lexer_.cur_token_;
}

void Parser::eat_token() noexcept
{
    lexer_.cur_token_ = lexer_.get_token();
}

bool Parser::is_cur_token(const TokenType token_type) const noexcept
{
    return lexer_.cur_token_.type_ == token_type ? true : false;
}

bool Parser::is_next_token(const TokenType token_type) const noexcept
{
    return lexer_.peek_token().type_ == token_type ? true : false;
}

void Parser::panic(const ParseError& error) noexcept
{
    diagnostics_.add_error(error);

    while (!is_cur_token(TokenType::RBRACE) &&
           !is_cur_token(TokenType::SEMICOLON) &&
           !is_cur_token(TokenType::END_OF_FILE)) {
        eat_token();
    }
}

std::expected<DeclRef, ParseError> Parser::parse_compilation_unit() noexcept
{
    auto comp_unit = ast_.emplace_decl<CompilationUnitDecl>(std::move(source_file_.file_path));

    eat_token();

    while (!is_cur_token(TokenType::END_OF_FILE)) {
        switch (lexer_.cur_token_.type_) {
            case TokenType::KEYWORD_CONST: {
                eat_token();
                std::expected<DeclRef, ParseError> const_decl;

                if (is_cur_token(TokenType::KEYWORD_FUNCTION))
                    const_decl = parse_func_decl(Constness::CONST);
                else if (is_cur_token(TokenType::TYPE))
                    const_decl = parse_var_decl(Constness::CONST);
                else
                    panic(SyntaxError{ cur_token() });

                if (!const_decl)
                    panic(const_decl.error());
                else
                    std::get<CompilationUnitDecl>(ast_.decls_[comp_unit]).decls_.push_back(*const_decl); // wrap this
                break;
            }
            case TokenType::KEYWORD_FUNCTION: {
                eat_token();

                auto func_decl = parse_func_decl(Constness::NON_CONST);
                if (!func_decl)
                    panic(func_decl.error());
                else
                    std::get<CompilationUnitDecl>(ast_.decls_[comp_unit]).decls_.push_back(*func_decl);
                break;
            }
            case TokenType::TYPE: {
                auto var = parse_var_decl(Constness::MUTABLE);
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

std::expected<DeclRef, ParseError> Parser::parse_var_decl(Constness constness) noexcept
{
    auto [type, name] = expect(TokenType::TYPE, TokenType::IDENTIFIER);
    
    if (!type) return std::unexpected{ type.error() };
    if (!name) return std::unexpected{ name.error() };

    if (is_cur_token(TokenType::SEMICOLON)) {
        return ast_.emplace_decl<VarDecl>(constness, std::move(*type), std::move(*name));
    } else if (is_cur_token(TokenType::EQUAL)) {
        eat_token();
        auto expr = parse_expr(0);
        if (!expr) 
            return std::unexpected(expr.error());
        return ast_.emplace_decl<VarDecl>(constness, std::move(*type), std::move(*name), *expr);
    } else {
        return std::unexpected{ SyntaxError{ cur_token() } };
    }
}

std::expected<DeclRef, ParseError> Parser::parse_param_decl() noexcept
{
    auto constness = Constness::MUTABLE;

    if (is_cur_token(TokenType::KEYWORD_CONST)) {
        constness = Constness::CONST;
        eat_token();
    }

    auto [type, name] = expect(TokenType::TYPE, TokenType::IDENTIFIER);

    if (!type) return std::unexpected{ type.error() };
    if (!name) return std::unexpected{ name.error() };

    return ast_.emplace_decl<ParamDecl>(constness, std::move(*type), std::move(*name));
}

std::expected<DeclRef, ParseError> Parser::parse_func_decl(Constness constness) noexcept
{
    if (!is_cur_token(TokenType::IDENTIFIER))
        return std::unexpected{ SyntaxError{cur_token(), "missing function identifier"} };
    
    auto name = cur_token().lexeme_;

    eat_token();

    if (!is_cur_token(TokenType::LPAREN))
        return std::unexpected{ SyntaxError{cur_token(), "missing parameter list (...)"} };
    
    eat_token();

    std::vector<DeclRef> params;

    while (!is_cur_token(TokenType::RPAREN)) {
        auto param_decl = parse_param_decl();

        if (!param_decl)
            return std::unexpected{ param_decl.error() };

        params.push_back(*param_decl);

        if (is_cur_token(TokenType::COMMA)) {
            eat_token();
        }
    }

    eat_token();
 
    if (!is_cur_token(TokenType::ARROW))
        return std::unexpected{ SyntaxError{cur_token(), "missing '->' trailing return type"} };

    eat_token();

    if (!is_cur_token(TokenType::TYPE))
        return std::unexpected{ SyntaxError{cur_token(), "missing function return type"} };

    auto return_type = cur_token().lexeme_;

    eat_token();

    if (!is_cur_token(TokenType::LBRACE))
        return std::unexpected{ SyntaxError{cur_token(), "missing function body {...}"} };

    eat_token();

    auto body = parse_compound_stmt();
    if (!body)
        return std::unexpected{ body.error() };

    return ast_.emplace_decl<FuncDecl>(constness, std::move(name), std::move(return_type), std::move(params), *body);
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
            return ast_.emplace_expr<ReferenceExpr>(std::string{token.lexeme_});
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
        case TokenType::SEMICOLON: {

        }
        case TokenType::EQUAL: {
            eat_token();
            return ast_.emplace_expr<BinaryExpr>(token.lexeme_, left, *parse_expr(lbp(token)));
        }
        default:
            break;
    }

    return -1;
}

std::expected<ExprRef, ParseError> Parser::parse_expr(int rbp = 0) noexcept
{
    auto left = nud(cur_token());
    
    eat_token();

    while (rbp < lbp(cur_token())) {
        left = led(cur_token(), *left);
    }

    return left;
}

// compoundstmt is most general node type
std::expected<ExprRef, ParseError> Parser::parse_compound_stmt() noexcept
{
    std::vector<DeclRef> decls;
    std::vector<ExprRef> exprs;

    return -1;
}