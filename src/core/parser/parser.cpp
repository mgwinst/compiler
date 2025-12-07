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

const Token Parser::next_token() const noexcept
{
    return lexer_.peek_token();
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

std::expected<std::string_view, ParseError> Parser::match(TokenType token_type) noexcept
{
    if (is_cur_token(token_type)) {
        auto lexeme = cur_token().lexeme_;
        eat_token();

        return lexeme;
    }
    return std::unexpected{ SyntaxError{ cur_token() } };
}

std::expected<DeclRef, ParseError> Parser::parse_compilation_unit() noexcept
{
    auto comp_unit = ast_.emplace_decl<CompilationUnitDecl>(std::move(source_file_.file_path));

    eat_token();

    while (!is_cur_token(TokenType::END_OF_FILE)) {
        auto decl = parse_decl();       
        if (!decl) panic(decl.error());

        std::get<CompilationUnitDecl>(ast_.decls_[comp_unit]).decls_.push_back(*decl);

        eat_token();
    }

    return comp_unit;
}

std::expected<DeclRef, ParseError> Parser::parse_decl() noexcept
{
    switch (cur_token().type_) {
        case TokenType::KEYWORD_CONST: {
            eat_token();
            std::expected<DeclRef, ParseError> const_decl;

            switch (cur_token().type_) {
                case TokenType::KEYWORD_STRUCT:
                    eat_token();
                    const_decl = parse_struct(Constness::CONST);
                    break;
                case TokenType::KEYWORD_FUNCTION:
                    eat_token();
                    const_decl = parse_func_decl(Constness::CONST);
                    break;
                case TokenType::TYPE:
                    const_decl = parse_var_decl(Constness::CONST);
                    break;
                default:
                    return std::unexpected{ SyntaxError{cur_token(), "expected function or variable declaration"} };
            }

            if (!const_decl) return std::unexpected{ const_decl.error() };

            return *const_decl;
        }

        case TokenType::KEYWORD_FUNCTION: {
            eat_token();

            auto func_decl = parse_func_decl(Constness::NON_CONST);
            if (!func_decl) return std::unexpected{ func_decl.error() };

            return *func_decl;
        }

        case TokenType::TYPE: {
            auto var = parse_var_decl(Constness::MUTABLE);
            if (!var) return std::unexpected{ var.error() };

            return *var;
        }

        case TokenType::KEYWORD_STRUCT: {
            eat_token();

            auto s = parse_struct(Constness::MUTABLE);
            if (!s) return std::unexpected{ s.error() };
            
            return *s;
        }

        default:
            return std::unexpected{ SyntaxError{ cur_token() }}; // fix this later
    }
}


std::expected<DeclRef, ParseError> Parser::parse_var_decl(Constness constness) noexcept
{
    auto [type, name] = expect(TokenType::TYPE, TokenType::IDENTIFIER);

    if (!type) return std::unexpected{ type.error() };
    if (!name) return std::unexpected{ name.error() };

    switch (cur_token().type_) {
        case TokenType::SEMICOLON: {
            return ast_.emplace_decl<VarDecl>(constness, std::string{ *type }, std::string{ *name });
        }

        case TokenType::EQUAL: {
            eat_token();

            auto expr = parse_expr();
            if (!expr) return std::unexpected{ expr.error() };

            return ast_.emplace_decl<VarDecl>(constness, std::string{ *type }, std::string{ *name }, *expr);
        }

        default:
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

    return ast_.emplace_decl<ParamDecl>(constness, std::string{ *type }, std::string{ *name });
}

std::expected<DeclRef, ParseError> Parser::parse_func_decl(Constness constness) noexcept
{
    auto [name] = expect(TokenType::IDENTIFIER);
    if (!name) return std::unexpected{ SyntaxError{cur_token(), "missing function identifier"}}; 

    auto [lparen] = expect(TokenType::LPAREN);
    if (!lparen) return std::unexpected{ SyntaxError{cur_token(), "missing parameter list (...)"} };

    std::vector<DeclRef> params;

    while (!is_cur_token(TokenType::RPAREN)) {
        auto param_decl = parse_param_decl();
        if (!param_decl) return std::unexpected{ param_decl.error() };

        params.push_back(*param_decl);

        if (is_cur_token(TokenType::COMMA)) eat_token();
    }

    eat_token();
 
    auto [arrow] = expect(TokenType::ARROW);
    if (!arrow) return std::unexpected{ SyntaxError{cur_token(), "missing '->' trailing return type"} };

    auto [return_type] = expect(TokenType::TYPE);
    if (!return_type) return std::unexpected{ SyntaxError{cur_token(), "missing function return type"} };

    auto [lbrace] = expect(TokenType::LBRACE);
    if (!lbrace) return std::unexpected{ SyntaxError{cur_token(), "missing function body {...}"} };

    auto body = parse_compound_stmt();
    if (!body) return std::unexpected{ body.error() };

    return ast_.emplace_decl<FuncDecl>(constness, std::string{ *name }, std::string{ *return_type }, std::move(params), *body);
}

std::expected<DeclRef, ParseError> Parser::parse_struct(Constness constness) noexcept
{
    if (!is_cur_token(TokenType::IDENTIFIER)) // struct type
        return std::unexpected{SyntaxError{cur_token(), "missing struct type"}};

    std::expected<DeclRef, ParseError> s;

    switch (next_token().type_) {
        case TokenType::LBRACE: {
            // struct Foo {int a; int b};
            s = parse_struct_def();
            break;
        }

        case TokenType::IDENTIFIER: { // struct identifier
            // struct Foo x;
            // struct Foo x = y;
            // struct Foo x = {};
            // struct Foo x(y);
            // struct Foo x {10, 20};
            // struct Foo x{ .a = 10, .b = 20 };
            s = parse_struct_decl(constness);
            break;
        }

        default:
            break;
    }

    if (!s) return std::unexpected{ SyntaxError{cur_token()} }; // some error message about struct misuse

    return *s;
}

std::expected<DeclRef, ParseError> Parser::parse_field() noexcept
{
    std::expected<DeclRef, ParseError> field;

    switch (cur_token().type_) {
        case TokenType::KEYWORD_CONST: {
            eat_token();

            switch(cur_token().type_) {
                case TokenType::KEYWORD_STRUCT: {
                    field = parse_struct_decl(Constness::CONST);
                    break;
                }
                case TokenType::TYPE: {
                    field = parse_var_decl(Constness::CONST);
                    break;
                }
                default:
                    return std::unexpected{ field.error() };
            }
            break;
        }

        case TokenType::KEYWORD_STRUCT: {
            field = parse_struct_decl(Constness::MUTABLE);
            break;
        }

        case TokenType::TYPE: {
            field = parse_var_decl(Constness::MUTABLE);
            break;
        }

        default:
            return std::unexpected{ SyntaxError{cur_token(), "missing struct data field/member"} };
    }

    if (!field) return std::unexpected{ field.error() };

    return *field;
}

std::expected<DeclRef, ParseError> Parser::parse_struct_def() noexcept
{
    auto [type] = (expect(TokenType::IDENTIFIER));
    if (!type) return std::unexpected{SyntaxError{cur_token(), "missing struct identifier"}};

    auto [lbrace] = expect(TokenType::LBRACE);
    if (!lbrace) return std::unexpected{ SyntaxError{cur_token(), "missing struct body {...}"} };

    std::vector<DeclRef> fields;
 
    while (!is_cur_token(TokenType::RBRACE)) {
        auto field = parse_field();
        if (!field) return std::unexpected{ field.error() };

        fields.push_back(*field);

        eat_token();

        auto [semi_colon] = expect(TokenType::SEMICOLON);
        if (semi_colon) return std::unexpected{SyntaxError{cur_token(), "missing ';' after field declaration"}};
    }

    eat_token();

    return ast_.emplace_decl<StructDef>(std::string{ *type }, std::move(fields));
}



// struct Foo x;
// struct Foo x = y;
// struct Foo x = {};
// struct Foo x(y);
// struct Foo x {10, 20};
// struct Foo x{ .a = 10, .b = 20 };

std::expected<DeclRef, ParseError> Parser::parse_struct_decl(Constness constness) noexcept
{
    auto [type, name] = expect(TokenType::IDENTIFIER, TokenType::IDENTIFIER);
    if (!type) return std::unexpected{ SyntaxError{cur_token(), "missing struct type"} };
    if (!name) return std::unexpected{ SyntaxError{cur_token(), "missing struct identifier"} };

    switch (cur_token().type_) {
        case TokenType::SEMICOLON: {
            return ast_.emplace_decl<VarDecl>(constness, std::string{ *type }, std::string{ *name });
        }

        case TokenType::EQUAL: {
            eat_token();

            auto expr = parse_expr();
            if (!expr) return std::unexpected{ expr.error() };

            return ast_.emplace_decl<VarDecl>(constness, std::string{ *type }, std::string{ *name }, *expr);
        }

        case TokenType::LBRACE: {
            eat_token();

            std::vector<ExprRef> exprs;

            while (!is_cur_token(TokenType::RBRACE)) {
                auto expr = parse_expr();
                if (!expr) return std::unexpected{ expr.error() };

                exprs.push_back(*expr);

                if (is_cur_token(TokenType::COMMA)) eat_token();
            }

            return ast_.emplace_decl<VarDecl>(constness, std::string{ *type }, std::string{ *name }, exprs);
        }

        case TokenType::LPAREN: {
            eat_token();

            auto expr = parse_expr();
            if (!expr) return std::unexpected{ expr.error() };

            return ast_.emplace_decl<VarDecl>(constness, std::string{ *type }, std::string{ *name }, *expr);
        }
        
        default:
            return std::unexpected{ SyntaxError{cur_token(), ""}};
            
    }
}



















// *************** EXPRESSION PARSING ***************

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

std::expected<ExprRef, ParseError> Parser::parse_expr(int rbp) noexcept
{
    auto left = nud(cur_token());
    
    eat_token();

    if (is_cur_token(TokenType::COMMA))
        return left;

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

    while (!is_cur_token(TokenType::RBRACE)) {

    }

    return ast_.emplace_expr<CompoundStmt>(std::move(decls), std::move(exprs));
}