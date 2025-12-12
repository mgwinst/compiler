#include <iostream>
#include <utility>
#include <expected>
#include <map>
#include <charconv>

#include "lexer/token.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "parser/ast.hpp"
#include "utils/macros.hpp"
#include "utils/string_utils.hpp"

Parser::Parser(SourceFile& source_file) noexcept :
    source_file_{ std::move(source_file) },
    diagnostics_{},
    lexer_{ source_file_.data },
    ast_{} {}

const Token Parser::next_token() const noexcept
{
    return lexer_.peek_token();
}

void Parser::eat_token() noexcept
{
    prev_token_ = cur_token_;
    cur_token_ = lexer_.get_token();
}

bool Parser::is_cur_token(const TokenType token_type) const noexcept
{
    return cur_token_.type_ == token_type ? true : false;
}

bool Parser::is_next_token(const TokenType token_type) const noexcept
{
    return lexer_.peek_token().type_ == token_type ? true : false;
}

void Parser::panic(const ParseError& error) noexcept
{
    diagnostics_.add_error(error);

    while (!is_cur_token(TokenType::TYPE) &&
           !is_cur_token(TokenType::KEYWORD_STRUCT) &&
           !is_cur_token(TokenType::KEYWORD_FUNCTION) &&
           !is_cur_token(TokenType::KEYWORD_CONST) &&
           !is_cur_token(TokenType::RBRACE) &&
           !is_cur_token(TokenType::SEMICOLON) &&
           !is_cur_token(TokenType::END_OF_FILE)) {
        eat_token();
    }
}

std::expected<std::string_view, ParseError> Parser::match(TokenType token_type) noexcept
{
    if (is_cur_token(token_type)) {
        auto lexeme = cur_token_.lexeme_;
        eat_token();

        return lexeme;
    }

    return std::unexpected{ SyntaxError{ prev_token_ } };
}

std::expected<NodeRef, ParseError> Parser::parse_compilation_unit() noexcept
{
    auto comp_unit = ast_.emplace<CompilationUnitDecl>(std::move(source_file_.file_path));

    eat_token();

    while (!is_cur_token(TokenType::END_OF_FILE)) {
        auto decl = parse_decl();       

        if (!decl)
            panic(decl.error());
        else
            std::get<CompilationUnitDecl>(ast_.nodes_[comp_unit]).decls_.push_back(*decl);
    }

    return comp_unit;
}

std::expected<NodeRef, ParseError> Parser::parse_decl() noexcept
{
    switch (cur_token_.type_) {
        case TokenType::KEYWORD_CONST: {
            eat_token();
            std::expected<NodeRef, ParseError> const_decl;

            switch (cur_token_.type_) {
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
                    return std::unexpected{ SyntaxError{cur_token_, "expected function or variable declaration"} };
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

        default: {
            eat_token(); // eat the unexpected token
            return std::unexpected{ SyntaxError{ prev_token_ } };
        }
    }
}

std::expected<NodeRef, ParseError> Parser::parse_init_list_expr() noexcept
{
    std::vector<NodeRef> init_values;

    while (!is_cur_token(TokenType::RBRACE)) {
        auto expr = parse_expr();
        if (!expr) return std::unexpected{ expr.error() };

        init_values.push_back(*expr);

        if (is_cur_token(TokenType::COMMA)) eat_token();
    }

    EXPECT_RBRACE();

    return ast_.emplace<InitListExpr>(std::move(init_values));
}

// must now accept var = {...}, do this later, array init and constructors
std::expected<NodeRef, ParseError> Parser::parse_var_decl(Constness constness) noexcept
{
    auto [type] = expect(TokenType::TYPE);
    if (!type) return std::unexpected{ type.error() };

    std::string full_type{ *type };   

    switch (cur_token_.type_) {
        case TokenType::STAR:
        case TokenType::STAR_STAR:
        case TokenType::AMPERSAND:
        case TokenType::AMPERSAND_AMPERSAND: {
            full_type += std::string{ cur_token_.lexeme_ };
            eat_token();
            break;
        }
        default:
            break;
    }

    auto [name] = expect(TokenType::IDENTIFIER);
    if (!name) return std::unexpected{ name.error() };

    if (is_cur_token(TokenType::LBRACKET)) {       
        eat_token();

        if (is_cur_token(TokenType::NUMERIC_LITERAL)) {
            auto size = cur_token_.lexeme_;
            full_type = full_type + "[" + std::string{ size } + "]";
            eat_token();
            EXPECT_RBRACKET();
        } else {
            EXPECT_RBRACKET();
            full_type = full_type + "[]";
        }
    }

    if (is_cur_token(TokenType::EQUAL)) {

        if (is_next_token(TokenType::LBRACE)) {
            eat_token();

            auto expr = parse_init_list_expr();
            if (!expr) return std::unexpected{ expr.error() };

            EXPECT_SEMICOLON();

            return ast_.emplace<VarDecl>(constness, std::move(full_type), std::string{ *name }, *expr);
        }

        eat_token();

        auto expr = parse_expr();
        if (!expr) return std::unexpected{ expr.error() };

        EXPECT_SEMICOLON();

        return ast_.emplace<VarDecl>(constness, std::move(full_type), std::string{ *name }, *expr);
    } else {
        EXPECT_SEMICOLON();

        return ast_.emplace<VarDecl>(constness, std::move(full_type), std::string{ *name });
    }
}

std::expected<NodeRef, ParseError> Parser::parse_param_decl() noexcept
{
    auto constness = Constness::MUTABLE;

    if (is_cur_token(TokenType::KEYWORD_CONST)) {
        constness = Constness::CONST;
        eat_token();
    }

    auto [type] = expect(TokenType::TYPE);
    if (!type) return std::unexpected{ type.error() };

    std::string full_type{ *type };

    switch (cur_token_.type_) {
        case TokenType::STAR:
        case TokenType::STAR_STAR:
        case TokenType::AMPERSAND:
        case TokenType::AMPERSAND_AMPERSAND: {
            full_type += std::string{ cur_token_.lexeme_ };
            eat_token();
            break;
        }
        default:
            break;
    }

    auto [name] = expect(TokenType::IDENTIFIER);
    if (!name) return std::unexpected{ name.error() };

    return ast_.emplace<ParamDecl>(constness, std::move(full_type), std::string{ *name });
}

std::expected<NodeRef, ParseError> Parser::parse_func_decl(Constness constness) noexcept
{
    auto [name] = expect(TokenType::IDENTIFIER);
    if (!name) return std::unexpected{ SyntaxError{cur_token_, "missing function identifier"}}; 

    EXPECT_LPAREN();

    std::vector<NodeRef> params;

    while (!is_cur_token(TokenType::RPAREN)) {
        auto param_decl = parse_param_decl();
        if (!param_decl) return std::unexpected{ param_decl.error() };

        params.push_back(*param_decl);

        if (is_cur_token(TokenType::COMMA)) eat_token();
    }

    eat_token();
 
    auto [arrow, return_type] = expect(TokenType::ARROW, TokenType::TYPE);

    if (!arrow) return std::unexpected{ SyntaxError{cur_token_, "missing '->' in trailing return type"} };
    if (!return_type) return std::unexpected{ SyntaxError{cur_token_, "missing function return type"} };

    std::string full_return_type{ *return_type };

    if (is_cur_token(TokenType::AMPERSAND) || is_cur_token(TokenType::STAR)) {
        full_return_type += std::string{ cur_token_.lexeme_ };
        eat_token();
    }

    EXPECT_LBRACE();

    auto body = parse_compound_stmt();
    if (!body) return std::unexpected{ body.error() };

    EXPECT_SEMICOLON();

    return ast_.emplace<FuncDecl>(constness, std::string{ *name }, std::move(full_return_type), std::move(params), *body);
}

std::expected<NodeRef, ParseError> Parser::parse_struct(Constness constness) noexcept
{
    if (!is_cur_token(TokenType::IDENTIFIER)) // struct type
        return std::unexpected{ SyntaxError{ cur_token_ } };

    std::expected<NodeRef, ParseError> s;

    switch (next_token().type_) {
        case TokenType::LBRACE: {
            // struct Foo {int a; int b};
            s = parse_struct_def();
            break;
        }

        case TokenType::STAR:
        case TokenType::STAR_STAR:
        case TokenType::AMPERSAND:
        case TokenType::AMPERSAND_AMPERSAND:
        case TokenType::IDENTIFIER: { // struct identifier
            // struct Foo x;
            // struct Foo x = y;
            // struct Foo x = {};
            // struct Foo x(y);
            // struct Foo x {10, 20};
            // struct Foo x { .a = 10, .b = 20 };
            s = parse_struct_decl(constness);
            break;
        }

        default:
            break;
    }

    if (!s) return std::unexpected{ s.error() };

    return *s;
}

std::expected<NodeRef, ParseError> Parser::parse_field() noexcept
{
    std::expected<NodeRef, ParseError> field;

    switch (cur_token_.type_) {
        case TokenType::KEYWORD_CONST: {
            eat_token();

            switch(cur_token_.type_) {
                case TokenType::KEYWORD_STRUCT: {
                    eat_token();
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
            eat_token();
            field = parse_struct_decl(Constness::MUTABLE);
            break;
        }

        case TokenType::TYPE: {
            field = parse_var_decl(Constness::MUTABLE);
            break;
        }

        default:
            return std::unexpected{ SyntaxError{cur_token_, "missing struct data field/member"} };
    }

    if (!field) return std::unexpected{ field.error() };

    return *field;
}

std::expected<NodeRef, ParseError> Parser::parse_struct_def() noexcept
{
    auto [type] = (expect(TokenType::IDENTIFIER));
    if (!type) return std::unexpected{SyntaxError{cur_token_, "missing struct identifier"}};

    EXPECT_LBRACE();

    std::vector<NodeRef> fields;
 
    while (!is_cur_token(TokenType::RBRACE)) {
        auto field = parse_field();

        if (!field) return std::unexpected{ field.error() };

        fields.push_back(*field);
    }

    EXPECT_RBRACE();

    return ast_.emplace<StructDecl>(std::string{ *type }, std::move(fields));
}

std::expected<NodeRef, ParseError> Parser::parse_struct_decl(Constness constness) noexcept
{
    auto [type] = expect(TokenType::IDENTIFIER);
    if (!type) return std::unexpected{ SyntaxError{cur_token_, "missing struct type"} };

    std::string full_type{ *type };

    switch (cur_token_.type_) {
        case TokenType::STAR:
        case TokenType::STAR_STAR:
        case TokenType::AMPERSAND:
        case TokenType::AMPERSAND_AMPERSAND: {
            full_type += std::string{ cur_token_.lexeme_ };
            eat_token();
            break;
        }
        default:
            break;
    }

    auto [name] = expect(TokenType::IDENTIFIER);
    if (!name) return std::unexpected{ SyntaxError{cur_token_, "missing struct identifier"} };

    switch (cur_token_.type_) {
        case TokenType::EQUAL: {
            eat_token();

            auto expr = parse_expr();
            if (!expr) return std::unexpected{ expr.error() };

            EXPECT_SEMICOLON();

            return ast_.emplace<VarDecl>(constness, std::move(full_type), std::string{ *name }, *expr);
        }

        case TokenType::LBRACE: {
            eat_token();

            auto expr = parse_init_list_expr();
            if (!expr) return std::unexpected{ expr.error() };
            
            EXPECT_SEMICOLON();

            return ast_.emplace<VarDecl>(constness, std::move(full_type), std::string{ *name }, *expr);
        }

        case TokenType::LPAREN: {
            eat_token();

            auto expr = parse_expr();
            if (!expr) return std::unexpected{ expr.error() };

            EXPECT_RPAREN();
            return ast_.emplace<VarDecl>(constness, std::move(full_type), std::string{ *name }, *expr);
        }

        default: {
            EXPECT_SEMICOLON();
            return ast_.emplace<VarDecl>(constness, std::move(full_type), std::string{ *name });
        }
    }
}

// compoundstmt is most general node type
std::expected<NodeRef, ParseError> Parser::parse_compound_stmt() noexcept
{
    // std::vector<NodeRef> decls;
    std::vector<NodeRef> exprs;

    while (!is_cur_token(TokenType::RBRACE)) {
        auto expr = parse_expr();
        if (!expr) return std::unexpected{ expr.error() };

        exprs.push_back(*expr);
    }

    EXPECT_RBRACE();

    return ast_.emplace<CompoundStmt>(std::move(exprs));
}

namespace prec 
{
    constexpr int32_t assignment  = 10;  // = += -= *= /= %=
    constexpr int32_t logical_or  = 20;  // ||
    constexpr int32_t logical_and = 30;  // &&
    constexpr int32_t bitwise_or  = 40;  // |
    constexpr int32_t bitwise_xor = 50;  // ^
    constexpr int32_t bitwise_and = 60;  // &
    constexpr int32_t equality    = 70;  // == !=
    constexpr int32_t cond        = 80;  // < > <= >=
    constexpr int32_t shift       = 90;  // << >>
    constexpr int32_t add         = 100; // + -
    constexpr int32_t mult        = 110; // * / % ** (might keep ** power to)
    constexpr int32_t postfix     = 120; // () [] . -> ++ -- (post)
    constexpr int32_t unary       = 130; // + - ! ~ & * ++ -- (pre)
}

auto infix_lbp(Token token) noexcept {
    switch (token.type_) {
        case TokenType::EQUAL:
        case TokenType::PLUS_EQUAL:
        case TokenType::MINUS_EQUAL:
        case TokenType::STAR_EQUAL:
        case TokenType::SLASH_EQUAL:
        case TokenType::PERCENT_EQUAL:
            return prec::assignment;

        case TokenType::PIPE_PIPE:
            return prec::logical_or;

        case TokenType::AMPERSAND_AMPERSAND:
            return prec::logical_and;

        case TokenType::PIPE:
            return prec::bitwise_or;

        case TokenType::CARROT:
            return prec::bitwise_xor;

        case TokenType::AMPERSAND:
            return prec::bitwise_and;

        case TokenType::EQUAL_EQUAL:
        case TokenType::BANG_EQUAL:
            return prec::equality;

        case TokenType::LESS:
        case TokenType::GREATER:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER_EQUAL:
            return prec::cond;

        case TokenType::LESS_LESS:
        case TokenType::GREATER_GREATER:
            return prec::shift;

        case TokenType::PLUS:
        case TokenType::MINUS:
            return prec::add;

        case TokenType::STAR:
        case TokenType::SLASH:
        case TokenType::PERCENT:
        case TokenType::STAR_STAR:
            return prec::mult;

        case TokenType::LPAREN:      // call
        case TokenType::LBRACKET:    // index
        case TokenType::DOT:         // member access
        case TokenType::ARROW:       // member access
        case TokenType::PLUS_PLUS:   // x++
        case TokenType::MINUS_MINUS: // x--
            return prec::postfix;

        default:
            return 0;
    }
}

std::expected<NodeRef, ParseError> Parser::nud(const Token token)
{
    eat_token();

    switch (token.type_) {
        case TokenType::STAR:
        case TokenType::STAR_STAR:
        case TokenType::AMPERSAND:
        case TokenType::PLUS:
        case TokenType::PLUS_PLUS:
        case TokenType::MINUS:
        case TokenType::MINUS_MINUS:
        case TokenType::BANG:
        case TokenType::CARROT: // move semantics later
        case TokenType::TILDE: {
            auto op = parse_expr(prec::unary);
            if (!op) return std::unexpected{ op.error() };

            return ast_.emplace<UnaryExpr>(std::string{ token.lexeme_ }, *op); // postfix = false (default value)
        }

        case TokenType::IDENTIFIER: {
            return ast_.emplace<ReferenceExpr>(std::string{ token.lexeme_ });
        }

        case TokenType::NUMERIC_LITERAL: {
            auto value = sv_to_numeric<int64_t>(token.lexeme_);
            if (!value) return std::unexpected{ cur_token_ };

            return ast_.emplace<IntegerLiteralExpr>(*value);
        }

        case TokenType::CHAR_LITERAL: {
            break;
        }

        case TokenType::STRING_LITERAL: {
            return ast_.emplace<StringLiteralExpr>(std::string{ token.lexeme_ });
        }

        case TokenType::KEYWORD_TRUE: {
            return ast_.emplace<BooleanLiteralExpr>(true);
        }

        case TokenType::KEYWORD_FALSE: {
            return ast_.emplace<BooleanLiteralExpr>(false);
        }

        case TokenType::LPAREN: {
            auto expr = parse_expr();
            if (!expr) return std::unexpected{ expr.error() };

            EXPECT_RPAREN();    
            return expr;
        }

        default:
            return std::unexpected{ SyntaxError{token, "unexpected symbol in expression"} };
    }
}

std::expected<NodeRef, ParseError> Parser::led(const Token token, const NodeRef left)
{
    eat_token();
    
    switch (token.type_) {
        case TokenType::EQUAL: 
        case TokenType::PLUS_EQUAL: 
        case TokenType::MINUS_EQUAL:
        case TokenType::STAR_EQUAL: 
        case TokenType::SLASH_EQUAL: 
        case TokenType::PERCENT_EQUAL:
        case TokenType::PLUS: 
        case TokenType::MINUS:
        case TokenType::STAR: 
        case TokenType::SLASH:
        case TokenType::PERCENT:
        case TokenType::LESS:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER:
        case TokenType::GREATER_EQUAL:
        case TokenType::EQUAL_EQUAL: 
        case TokenType::BANG_EQUAL:
        case TokenType::AMPERSAND: 
        case TokenType::PIPE: 
        case TokenType::CARROT:
        case TokenType::LESS_LESS: 
        case TokenType::GREATER_GREATER:
        case TokenType::AMPERSAND_AMPERSAND: 
        case TokenType::PIPE_PIPE:
        case TokenType::STAR_STAR: {
            auto right = parse_expr(infix_lbp(token));
            if (!right) return std::unexpected{ right.error() };

            return ast_.emplace<BinaryExpr>(std::string{ token.lexeme_ }, left, *right);
        }

        case TokenType::PLUS_PLUS:
        case TokenType::MINUS_MINUS: {
            return ast_.emplace<UnaryExpr>(std::string{token.lexeme_}, left, true);  // postfix = true, since this is only time true, bool seems okay for now, move to clear enum later
        }

        // func(x)
        case TokenType::LPAREN: {
            std::vector<NodeRef> args;

            while (!is_cur_token(TokenType::RPAREN)) {
                auto arg = parse_expr();
                if (!arg) return std::unexpected{ arg.error() };

                args.push_back(*arg);

                if (is_cur_token(TokenType::COMMA)) eat_token();
            }

            EXPECT_RPAREN();
            return ast_.emplace<CallExpr>(left, std::move(args));
        }

        // arr[x]
        case TokenType::LBRACKET: {
            auto index = parse_expr();
            if (!index) return std::unexpected{ index.error() };

            EXPECT_RBRACKET();
            return ast_.emplace<ArraySubscriptExpr>(left, *index);
        }

        case TokenType::DOT:
        case TokenType::ARROW: {
            auto member = parse_expr();
            if (!member) return std::unexpected{ member.error() };

            break;
        }

        default:
            return std::unexpected{ token };
    }
}

std::expected<NodeRef, ParseError> Parser::parse_expr(int min_prec) noexcept
{
    auto left = nud(cur_token_);
    if (!left) return std::unexpected{ left.error() };

    while (true) {
        auto lbp = infix_lbp(cur_token_);

        if (lbp <= min_prec) break;

        auto next = led(cur_token_, *left);
        if (!next) return std::unexpected{ next.error() };

        left = *next;
    }

    return left;
}
