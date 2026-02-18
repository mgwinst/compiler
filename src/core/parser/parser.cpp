#include <expected>

#include "../lexer/token.hpp"
#include "../lexer/lexer.hpp"
#include "parser.hpp"
#include "../utils/macros.hpp"
#include "../utils/string_utils.hpp"
#include "../utils/utils.hpp"

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

std::expected<ASTNodeRef, ParseError> Parser::parse_compilation_unit() noexcept
{
    auto comp_unit = ast_.emplace<SyntaxTree::CompilationUnitDecl>(std::move(source_file_.file_path));

    eat_token();

    while (!is_cur_token(TokenType::END_OF_FILE)) {
        auto decl = parse_decl();       

        if (!decl)
            panic(decl.error());
        else
            ast_.nodes_[comp_unit].as<SyntaxTree::CompilationUnitDecl>().decls_.push_back(*decl);
    }

    return comp_unit;
}

std::expected<ASTNodeRef, ParseError> Parser::parse_decl() noexcept
{
    switch (cur_token_.type_) {
        case TokenType::KEYWORD_CONST: {
            eat_token();
            std::expected<ASTNodeRef, ParseError> const_decl;

            switch (cur_token_.type_) {
                case TokenType::KEYWORD_STRUCT:
                    eat_token();
                    const_decl = parse_struct(CONST);
                    break;
                case TokenType::TYPE:
                    const_decl = parse_var_decl(CONST);
                    break;
                default:
                    return std::unexpected{ SyntaxError{cur_token_, "expected function or variable declaration"} };
            }

            if (!const_decl) return std::unexpected{ const_decl.error() };

            return *const_decl;
        }

        case TokenType::KEYWORD_FUNCTION: {
            eat_token();

            auto func_decl = parse_func_decl();
            if (!func_decl) return std::unexpected{ func_decl.error() };

            return *func_decl;
        }

        case TokenType::TYPE: {
            auto var = parse_var_decl(NON_CONST);
            if (!var) return std::unexpected{ var.error() };

            return *var;
        }

        case TokenType::KEYWORD_STRUCT: {
            eat_token();

            auto s = parse_struct(NON_CONST);
            if (!s) return std::unexpected{ s.error() };
            
            return *s;
        }

        default: {
            eat_token(); // eat the unexpected token
            return std::unexpected{ SyntaxError{ prev_token_ } };
        }
    }
}

std::expected<ASTNodeRef, ParseError> Parser::parse_init_list_expr() noexcept
{
    std::vector<ASTNodeRef> init_values;

    while (!is_cur_token(TokenType::RBRACE)) {
        auto expr = parse_expr();
        if (!expr) return std::unexpected{ expr.error() };

        init_values.push_back(*expr);

        if (is_cur_token(TokenType::COMMA)) eat_token();
    }

    EXPECT_RBRACE();

    return ast_.emplace<SyntaxTree::InitListExpr>(std::move(init_values)); }

std::expected<ASTNodeRef, ParseError> Parser::parse_var_decl(bool is_const) noexcept
{
    std::string type_string{};

    auto [type] = expect(TokenType::TYPE);
    if (!type) return std::unexpected{ type.error() };

    type_string += *type;

    switch (cur_token_.type_) {
        case TokenType::STAR:
        case TokenType::STAR_STAR:
        case TokenType::AMPERSAND:
        case TokenType::AMPERSAND_AMPERSAND: {
            type_string += std::string{ cur_token_.lexeme_ };
            eat_token();
            break;
        }

        default:
            break;
    }

    auto [name] = expect(TokenType::IDENTIFIER);
    if (!name) return std::unexpected{ name.error() };

    bool is_array = false;
    if (is_cur_token(TokenType::LBRACKET)) {       
        eat_token();

        if (!is_cur_token(TokenType::RBRACKET)) {
            auto expr = parse_expr();
            if (!expr) return std::unexpected{ expr.error() };
            // we have to make sure this expression is computable at compile time and is int
        }

        EXPECT_RBRACKET();

        is_array = true;
        type_string += "[]";
    }

    if (is_cur_token(TokenType::EQUAL)) {

        if (is_next_token(TokenType::LBRACE)) {
            eat_token();

            auto expr = parse_init_list_expr();
            if (!expr) return std::unexpected{ expr.error() };

            EXPECT_SEMICOLON();

            return ast_.emplace<SyntaxTree::VarDecl>(is_const, is_array, std::move(type_string), std::string{ *name }, *expr);
        }

        eat_token();

        auto expr = parse_expr();
        if (!expr) return std::unexpected{ expr.error() };

        EXPECT_SEMICOLON();

        return ast_.emplace<SyntaxTree::VarDecl>(is_const, is_array, std::move(type_string), std::string{ *name }, *expr);
    } else {
        EXPECT_SEMICOLON();

        return ast_.emplace<SyntaxTree::VarDecl>(is_const, is_array, std::move(type_string), std::string{ *name });
    }
}

std::expected<ASTNodeRef, ParseError> Parser::parse_param_decl() noexcept
{
    bool is_const = NON_CONST;

    if (is_cur_token(TokenType::KEYWORD_CONST)) {
        is_const = CONST;
        eat_token();
    }

    auto [type] = expect(TokenType::TYPE);
    if (!type) return std::unexpected{ type.error() };

    std::string type_string{ *type };

    switch (cur_token_.type_) {
        case TokenType::STAR:
        case TokenType::STAR_STAR:
        case TokenType::AMPERSAND:
        case TokenType::AMPERSAND_AMPERSAND: {
            type_string += std::string{ cur_token_.lexeme_ };
            eat_token();
            break;
        }
        default:
            break;
    }

    auto [name] = expect(TokenType::IDENTIFIER);
    if (!name) return std::unexpected{ name.error() };

    return ast_.emplace<SyntaxTree::ParamDecl>(is_const, std::move(type_string), std::string{ *name });
}

std::expected<ASTNodeRef, ParseError> Parser::parse_func_decl() noexcept
{
    auto [name] = expect(TokenType::IDENTIFIER);
    if (!name) return std::unexpected{ SyntaxError{cur_token_, "missing function identifier"}}; 

    EXPECT_LPAREN();

    std::vector<ASTNodeRef> params;

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

    return ast_.emplace<SyntaxTree::FuncDecl>(std::string{ *name }, std::move(params), std::move(full_return_type), *body);
}

std::expected<ASTNodeRef, ParseError> Parser::parse_struct(bool is_const) noexcept
{
    if (!is_cur_token(TokenType::IDENTIFIER)) // struct type
        return std::unexpected{ SyntaxError{ cur_token_ } };

    std::expected<ASTNodeRef, ParseError> s;

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
            s = parse_struct_decl(is_const);
            break;
        }

        default:
            break;
    }

    if (!s) return std::unexpected{ s.error() };

    return *s;
}

std::expected<ASTNodeRef, ParseError> Parser::parse_field() noexcept
{
    std::expected<ASTNodeRef, ParseError> field;

    switch (cur_token_.type_) {
        case TokenType::KEYWORD_CONST: {
            eat_token();

            switch(cur_token_.type_) {
                case TokenType::KEYWORD_STRUCT: {
                    eat_token();
                    field = parse_struct_decl(CONST);
                    break;
                }
                case TokenType::TYPE: {
                    field = parse_var_decl(CONST);
                    break;
                }
                default:
                    return std::unexpected{ field.error() };
            }
            break;
        }

        case TokenType::KEYWORD_STRUCT: {
            eat_token();
            field = parse_struct_decl(NON_CONST);
            break;
        }

        case TokenType::TYPE: {
            field = parse_var_decl(NON_CONST);
            break;
        }

        default:
            return std::unexpected{ SyntaxError{cur_token_, "missing struct data field/member"} };
    }

    if (!field) return std::unexpected{ field.error() };

    return *field;
}

std::expected<ASTNodeRef, ParseError> Parser::parse_struct_def() noexcept
{
    auto [type] = (expect(TokenType::IDENTIFIER));
    if (!type) return std::unexpected{SyntaxError{cur_token_, "missing struct identifier"}};

    EXPECT_LBRACE();

    std::vector<ASTNodeRef> fields;
 
    while (!is_cur_token(TokenType::RBRACE)) {
        auto field = parse_field();

        if (!field) return std::unexpected{ field.error() };

        fields.push_back(*field);
    }

    EXPECT_RBRACE();

    return ast_.emplace<SyntaxTree::StructDecl>(std::string{ *type }, std::move(fields));
}

std::expected<ASTNodeRef, ParseError> Parser::parse_struct_decl(bool is_const) noexcept
{
    bool is_array = false; // must handle array of structs 

    auto [type] = expect(TokenType::IDENTIFIER);
    if (!type) return std::unexpected{ SyntaxError{cur_token_, "missing struct type"} };

    std::string type_string{ *type };

    switch (cur_token_.type_) {
        case TokenType::STAR:
        case TokenType::STAR_STAR:
        case TokenType::AMPERSAND:
        case TokenType::AMPERSAND_AMPERSAND: {
            type_string += std::string{ cur_token_.lexeme_ };
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

            if (is_cur_token(TokenType::LBRACE)) {
                eat_token();

                auto expr = parse_init_list_expr();
                if (!expr) return std::unexpected{ expr.error() };
                
                EXPECT_SEMICOLON();

                return ast_.emplace<SyntaxTree::VarDecl>(is_const, is_array, std::move(type_string), std::string{ *name }, *expr);
            } else {
                auto expr = parse_expr();
                if (!expr) return std::unexpected{ expr.error() };

                EXPECT_SEMICOLON();

                return ast_.emplace<SyntaxTree::VarDecl>(is_const, is_array, std::move(type_string), std::string{ *name }, *expr);
            }
        }

        case TokenType::LBRACE: {
            eat_token();

            auto expr = parse_init_list_expr();
            if (!expr) return std::unexpected{ expr.error() };
            
            EXPECT_SEMICOLON();

            return ast_.emplace<SyntaxTree::VarDecl>(is_const, is_array, std::move(type_string), std::string{ *name }, *expr);
        }

        case TokenType::LPAREN: {
            eat_token();

            auto expr = parse_expr();
            if (!expr) return std::unexpected{ expr.error() };

            EXPECT_RPAREN();
            return ast_.emplace<SyntaxTree::VarDecl>(is_const, is_array, std::move(type_string), std::string{ *name }, *expr);
        }

        default: {
            EXPECT_SEMICOLON();
            return ast_.emplace<SyntaxTree::VarDecl>(is_const, is_array, std::move(type_string), std::string{ *name });
        }
    }
}

// if / while / for statements can only occur in compound statements
std::expected<ASTNodeRef, ParseError> Parser::parse_compound_stmt() noexcept
{
    std::vector<ASTNodeRef> children;

    while (!is_cur_token(TokenType::RBRACE)) {
        switch (cur_token_.type_) {
            case TokenType::KEYWORD_CONST:
            case TokenType::KEYWORD_STRUCT:
            case TokenType::TYPE: {
                auto decl = parse_decl();
                if (!decl) return std::unexpected{ decl.error() };
                
                children.push_back(*decl);
                break;
            }

            case TokenType::KEYWORD_FOR: {
                eat_token();

                auto for_loop = parse_for_loop();
                if (!for_loop) return std::unexpected { for_loop.error() };

                children.push_back(*for_loop);
                break;
            }

            case TokenType::KEYWORD_WHILE: {
                eat_token();

                auto while_loop = parse_while_loop();
                if (!while_loop) return std::unexpected { while_loop.error() };

                children.push_back(*while_loop);
                break;
            }

            case TokenType::KEYWORD_IF: {
                eat_token();

                auto if_stmt = parse_if_stmt();
                if (!if_stmt) return std::unexpected { if_stmt.error() };

                children.push_back(*if_stmt);
                break;
            }

            default: {
                auto expr = parse_expr();
                if (!expr) return std::unexpected{ expr.error() };

                EXPECT_SEMICOLON();
                children.push_back(*expr);
                break;
            }
        }

        // Must handle return statements
    }

    EXPECT_RBRACE();

    return ast_.emplace<SyntaxTree::CompoundStmt>(std::move(children));
}

std::expected<ASTNodeRef, ParseError> Parser::parse_while_loop() noexcept
{
    EXPECT_LPAREN();

    auto expr = parse_expr();
    if (!expr) return std::unexpected{ expr.error() };

    EXPECT_RPAREN();

    EXPECT_LBRACE();
    
    auto compound_stmt = parse_compound_stmt();
    if (!compound_stmt) return std::unexpected{ compound_stmt.error() };

    return ast_.emplace<SyntaxTree::WhileStmt>(*expr, *compound_stmt);
}

std::expected<ASTNodeRef, ParseError> Parser::parse_for_loop() noexcept
{
    EXPECT_LPAREN();

    auto init = parse_decl();
    if (!init) return std::unexpected{ init.error() };

    auto cond = parse_expr();
    if (!cond) return std::unexpected{ cond.error() };

    EXPECT_SEMICOLON();

    auto update = parse_expr();
    if (!update) return std::unexpected{ update.error() };

    EXPECT_RPAREN();

    EXPECT_LBRACE();

    auto body = parse_compound_stmt();
    if (!body) return std::unexpected{ body.error() };

    return ast_.emplace<SyntaxTree::ForStmt>(*init, *cond, *update, *body);
}

std::expected<ASTNodeRef, ParseError> Parser::parse_if_stmt() noexcept
{
    EXPECT_LPAREN();
    
    auto cond = parse_expr();
    if (!cond) return std::unexpected{ cond.error() };

    EXPECT_RPAREN();

    EXPECT_LBRACE();
    auto then_branch_body = parse_compound_stmt();
    if (!then_branch_body) return std::unexpected{ then_branch_body.error() };

    if (is_cur_token(TokenType::KEYWORD_ELSE)) {
        eat_token();

        EXPECT_LBRACE();

        auto else_branch_body = parse_compound_stmt();
        if (!else_branch_body) return std::unexpected{ else_branch_body.error() };

        return ast_.emplace<SyntaxTree::IfStmt>(*cond, *then_branch_body, *else_branch_body);
    }

    return ast_.emplace<SyntaxTree::IfStmt>(*cond, *then_branch_body);
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
    constexpr int32_t unary       = 120; // + - ! ~ & * ++ -- (pre)
    constexpr int32_t postfix     = 130; // () [] . -> ++ -- (post)
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

std::expected<ASTNodeRef, ParseError> Parser::nud(const Token token) noexcept
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

            return ast_.emplace<SyntaxTree::UnaryExpr>(std::string{ token.lexeme_ }, *op); // postfix = false (default value)
        }

        case TokenType::IDENTIFIER: {
            return ast_.emplace<SyntaxTree::ReferenceExpr>(std::string{ token.lexeme_ });
        }

        case TokenType::NUMERIC_LITERAL: {
            auto value = sv_to_numeric<int64_t>(token.lexeme_);
            if (!value) return std::unexpected{ cur_token_ };

            return ast_.emplace<SyntaxTree::IntegerLiteralExpr>(*value);
        }

        case TokenType::CHAR_LITERAL: {
            break;
        }

        case TokenType::STRING_LITERAL: {
            return ast_.emplace<SyntaxTree::StringLiteralExpr>(std::string{ token.lexeme_ });
        }

        case TokenType::KEYWORD_TRUE: {
            return ast_.emplace<SyntaxTree::BooleanLiteralExpr>(true);
        }

        case TokenType::KEYWORD_FALSE: {
            return ast_.emplace<SyntaxTree::BooleanLiteralExpr>(false);
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

std::expected<ASTNodeRef, ParseError> Parser::led(const Token token, ASTNodeRef left) noexcept
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

            return ast_.emplace<SyntaxTree::BinaryExpr>(std::string{ token.lexeme_ }, left, *right);
        }

        case TokenType::PLUS_PLUS:
        case TokenType::MINUS_MINUS: {
            return ast_.emplace<SyntaxTree::UnaryExpr>(std::string{token.lexeme_}, left, true);  // postfix = true, since this is only time true, bool seems okay for now, move to clear enum later
        }

        // func(x)
        case TokenType::LPAREN: {
            std::vector<ASTNodeRef> args;

            while (!is_cur_token(TokenType::RPAREN)) {
                auto arg = parse_expr();
                if (!arg) return std::unexpected{ arg.error() };

                args.push_back(*arg);

                if (is_cur_token(TokenType::COMMA)) eat_token();
            }

            EXPECT_RPAREN();
            return ast_.emplace<SyntaxTree::CallExpr>(left, std::move(args));
        }

        // arr[x]
        case TokenType::LBRACKET: {
            auto index = parse_expr();
            if (!index) return std::unexpected{ index.error() };

            EXPECT_RBRACKET();
            return ast_.emplace<SyntaxTree::ArraySubscriptExpr>(left, *index);
        }

        case TokenType::DOT:
        case TokenType::ARROW: {
            auto [member_name] = expect(TokenType::IDENTIFIER);
            if (!member_name) return std::unexpected{ member_name.error() };

            auto is_arrow = token.type_ == TokenType::ARROW ? true : false;

            return ast_.emplace<SyntaxTree::MemberExpr>(left, std::string{ *member_name }, is_arrow);
        }

        default:
            return std::unexpected{ token };
    }
}

std::expected<ASTNodeRef, ParseError> Parser::parse_expr(int min_prec) noexcept
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
