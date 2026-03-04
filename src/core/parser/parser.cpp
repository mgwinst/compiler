#include <expected>

#include "../lexer/token.hpp"
#include "../lexer/lexer.hpp"
#include "parser.hpp"
#include "../utils/utils.hpp"
#include "../utils/string_utils.hpp"
#include "../utils/alias.hpp"
#include "../utils/macros.hpp"

ParseContext::ParseContext(SourceFile& source_file) noexcept :
    source_file_{ std::move(source_file) },
    diagnostics_{},
    lexer_{ source_file_.data },
    ast_{} {}

const Token ParseContext::next_token() const noexcept
{
    return lexer_.peek_token();
}

void ParseContext::eat_token() noexcept
{
    prev_token_ = cur_token_;
    cur_token_ = lexer_.get_token();
}

bool ParseContext::is_cur_token(const TokenType token_type) const noexcept
{
    return cur_token_.type_ == token_type ? true : false;
}

bool ParseContext::is_next_token(const TokenType token_type) const noexcept
{
    return lexer_.peek_token().type_ == token_type ? true : false;
}

void ParseContext::panic(const ParseError& error) noexcept
{
    diagnostics_.add_error(error);

    while (!is_cur_token(TokenType::KEYWORD_STRUCT) &&
           !is_cur_token(TokenType::KEYWORD_FUNCTION) &&
           !is_cur_token(TokenType::KEYWORD_VAR) &&
           !is_cur_token(TokenType::RBRACE) &&
           !is_cur_token(TokenType::SEMICOLON) &&
           !is_cur_token(TokenType::END_OF_FILE)) {
        eat_token();
    }
}

std::expected<std::string_view, ParseError> ParseContext::match(TokenType token_type) noexcept
{
    if (is_cur_token(token_type)) {
        auto lexeme = cur_token_.lexeme_;
        eat_token();

        return lexeme;
    }

    return std::unexpected{ SyntaxError{ prev_token_ } };
}

std::expected<ASTNodeRef, ParseError> ParseContext::parse_compilation_unit() noexcept
{
    auto comp_unit = ast_.emplace<Syntax::CompilationUnitDecl>(std::move(source_file_.file_path));

    eat_token();

    while (!is_cur_token(TokenType::END_OF_FILE)) {
        auto decl = parse_decl();   

        if (!decl)
            panic(decl.error());
        else
            ast_.nodes_[comp_unit].as<Syntax::CompilationUnitDecl>().decls_.push_back(*decl);
    }

    return comp_unit;
}

// var x: int
// var x: const int
// var x: Foo
// var x: int[4]
// var x: int*
// var x: int**

// call this after encountering 'var <ident>:'
std::expected<ASTNodeRef, ParseError> ParseContext::parse_type() noexcept
{
    bool is_const = false;
    if (is_cur_token(TokenType::KEYWORD_CONST)) {
        is_const = true;
        eat_token();
    }

    if (!is_cur_token(TokenType::IDENTIFIER) && !is_cur_token(TokenType::TYPE))
        return std::unexpected{ SyntaxError{cur_token_, "Expecting built-in or user-defined type"} }; 


    auto type_name = std::string{ cur_token_.lexeme_ };
    auto type = ast_.emplace<Syntax::NamedTypeExpr>(std::move(type_name));
    eat_token();

    if (is_const) {
        type = ast_.emplace<Syntax::QualifierTypeExpr>(QualifierKind::Const, type);
    }
    
    switch (cur_token_.type_) {
        case TokenType::LBRACKET: {
            eat_token();

            if (is_cur_token(TokenType::RBRACKET)) {
                eat_token();
                return ast_.emplace<Syntax::ArrayTypeExpr>(type);
            }

            auto size = parse_expr();
            if (!size) return std::unexpected{ size.error() };

            expect(TokenType::RBRACKET);
            return ast_.emplace<Syntax::ArrayTypeExpr>(type, *size);
        }

        case TokenType::STAR: {
            eat_token();
            return ast_.emplace<Syntax::PointerTypeExpr>(type);
        }

        case TokenType::AMPERSAND: {
            eat_token();
            return ast_.emplace<Syntax::ReferenceTypeExpr>(type);
        }

        default:
            return type;
    }

}

// fn, struct, var -> all decls possible
std::expected<ASTNodeRef, ParseError> ParseContext::parse_decl() noexcept
{
    switch (cur_token_.type_) {
        case TokenType::KEYWORD_FUNCTION: {
            eat_token();

            auto func_decl = parse_func_decl();
            if (!func_decl) return std::unexpected{ func_decl.error() };

            return *func_decl;
        }

        case TokenType::KEYWORD_STRUCT: {
            eat_token();

            auto s = parse_struct_def();
            if (!s) return std::unexpected{ s.error() };
            
            return *s;
        }
        
        case TokenType::KEYWORD_VAR: {
            eat_token();
            
            auto var = parse_var_decl();
            if (!var) return std::unexpected{ var.error() };

            return *var;
        }

        default: {
            return std::unexpected{ SyntaxError{ cur_token_ } };
        }
    }
}

// var x: int;
// var x: int = 0;

// var x: Point;
// var x: Point = {};
// var x: Point = {1, 2}

// var x: int[4];
// var x: int[4] = {}
// var x: int[4] = {1, 2, 3, 4}

std::expected<ASTNodeRef, ParseError> ParseContext::parse_var_decl() noexcept
{
    auto [name] = expect(TokenType::IDENTIFIER);
    if (!name) return std::unexpected{ name.error() };

    EXPECT_COLON();
    
    auto type = parse_type();
    if (!type) return std::unexpected{ type.error() };

    if (is_cur_token(TokenType::EQUAL)) {
        if (is_next_token(TokenType::LBRACE)) {
            eat_token();

            auto expr = parse_init_list_expr();
            if (!expr) return std::unexpected{ expr.error() };

            EXPECT_SEMICOLON();

            return ast_.emplace<Syntax::VarDecl>(std::string{ *name }, *type, *expr);
        }

        eat_token();

        auto expr = parse_expr();
        if (!expr) return std::unexpected{ expr.error() };

        EXPECT_SEMICOLON();

        return ast_.emplace<Syntax::VarDecl>(std::string{ *name }, *type, *expr);
    } else {
        EXPECT_SEMICOLON();
        
        return ast_.emplace<Syntax::VarDecl>(std::string{ *name }, *type);
    }
}

std::expected<ASTNodeRef, ParseError> ParseContext::parse_func_decl() noexcept
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

    auto [arrow] = expect(TokenType::ARROW);
    if (!arrow) return std::unexpected{ SyntaxError{cur_token_, "missing '->' in trailing return type"} };

    auto return_type = parse_type();
    if (!return_type) return std::unexpected{ return_type.error() };

    EXPECT_LBRACE();

    auto body = parse_compound_stmt();
    if (!body) return std::unexpected{ body.error() };

    return ast_.emplace<Syntax::FuncDecl>(std::string{ *name }, std::move(params), *return_type, *body);
}

std::expected<ASTNodeRef, ParseError> ParseContext::parse_param_decl() noexcept
{
    auto [name] = expect(TokenType::IDENTIFIER);
    if (!name) return std::unexpected{ name.error() };

    EXPECT_COLON();
    
    auto type = parse_type();
    if (!type) return std::unexpected{ type.error() };

    return ast_.emplace<Syntax::ParamDecl>(std::string{ *name }, *type);
}

std::expected<ASTNodeRef, ParseError> ParseContext::parse_struct_def() noexcept
{
    auto [name] = expect(TokenType::IDENTIFIER);
    if (!name) return std::unexpected{SyntaxError{cur_token_, "missing struct identifier"}};

    EXPECT_LBRACE();

    std::vector<ASTNodeRef> fields;
 
    while (!is_cur_token(TokenType::RBRACE)) {
        auto field = parse_field();

        if (!field) return std::unexpected{ field.error() };

        fields.push_back(*field);
    }

    EXPECT_RBRACE();

    return ast_.emplace<Syntax::RecordDecl>(RecordKind::Struct, std::string{ *name }, std::move(fields));
}

std::expected<ASTNodeRef, ParseError> ParseContext::parse_field() noexcept
{
    EXPECT_VAR(); // because parse_var_decl() expects var token to have already been eaten

    return parse_var_decl();
}

// if / while / for statements can only occur in compound statements
std::expected<ASTNodeRef, ParseError> ParseContext::parse_compound_stmt() noexcept
{
    std::vector<ASTNodeRef> children;

    while (!is_cur_token(TokenType::RBRACE)) {
        switch (cur_token_.type_) {
            case TokenType::KEYWORD_VAR: {
                eat_token();

                auto decl = parse_var_decl();
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

    return ast_.emplace<Syntax::CompoundStmt>(std::move(children));
}

std::expected<ASTNodeRef, ParseError> ParseContext::parse_while_loop() noexcept
{
    EXPECT_LPAREN();

    auto expr = parse_expr();
    if (!expr) return std::unexpected{ expr.error() };

    EXPECT_RPAREN();

    EXPECT_LBRACE();
    
    auto compound_stmt = parse_compound_stmt();
    if (!compound_stmt) return std::unexpected{ compound_stmt.error() };

    return ast_.emplace<Syntax::WhileStmt>(*expr, *compound_stmt);
}

std::expected<ASTNodeRef, ParseError> ParseContext::parse_for_loop() noexcept
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

    return ast_.emplace<Syntax::ForStmt>(*init, *cond, *update, *body);
}

std::expected<ASTNodeRef, ParseError> ParseContext::parse_if_stmt() noexcept
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

        return ast_.emplace<Syntax::IfStmt>(*cond, *then_branch_body, *else_branch_body);
    }

    return ast_.emplace<Syntax::IfStmt>(*cond, *then_branch_body);
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

std::expected<ASTNodeRef, ParseError> ParseContext::nud(const Token token) noexcept
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

            return ast_.emplace<Syntax::UnaryExpr>(std::string{ token.lexeme_ }, *op); // postfix = false (default value)
        }

        case TokenType::IDENTIFIER: {
            return ast_.emplace<Syntax::ReferenceExpr>(std::string{ token.lexeme_ });
        }

        case TokenType::NUMERIC_LITERAL: {
            auto value = sv_to_numeric<int64_t>(token.lexeme_);
            if (!value) return std::unexpected{ cur_token_ };

            return ast_.emplace<Syntax::IntegerLiteralExpr>(*value);
        }

        case TokenType::CHAR_LITERAL: {
            break;
        }

        case TokenType::STRING_LITERAL: {
            return ast_.emplace<Syntax::StringLiteralExpr>(std::string{ token.lexeme_ });
        }

        case TokenType::KEYWORD_TRUE: {
            return ast_.emplace<Syntax::BooleanLiteralExpr>(true);
        }

        case TokenType::KEYWORD_FALSE: {
            return ast_.emplace<Syntax::BooleanLiteralExpr>(false);
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

std::expected<ASTNodeRef, ParseError> ParseContext::led(const Token token, ASTNodeRef left) noexcept
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

            return ast_.emplace<Syntax::BinaryExpr>(std::string{ token.lexeme_ }, left, *right);
        }

        case TokenType::PLUS_PLUS:
        case TokenType::MINUS_MINUS: {
            return ast_.emplace<Syntax::UnaryExpr>(std::string{token.lexeme_}, left, true);  // postfix = true, since this is only time true, bool seems okay for now, move to clear enum later
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
            return ast_.emplace<Syntax::CallExpr>(left, std::move(args));
        }

        // arr[x]
        case TokenType::LBRACKET: {
            auto index = parse_expr();
            if (!index) return std::unexpected{ index.error() };

            EXPECT_RBRACKET();
            return ast_.emplace<Syntax::ArraySubscriptExpr>(left, *index);
        }

        case TokenType::DOT:
        case TokenType::ARROW: {
            auto [member_name] = expect(TokenType::IDENTIFIER);
            if (!member_name) return std::unexpected{ member_name.error() };

            auto is_arrow = token.type_ == TokenType::ARROW ? true : false;

            return ast_.emplace<Syntax::MemberExpr>(left, std::string{ *member_name }, is_arrow);
        }

        default:
            return std::unexpected{ token };
    }
}

std::expected<ASTNodeRef, ParseError> ParseContext::parse_expr(int min_prec) noexcept
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

std::expected<ASTNodeRef, ParseError> ParseContext::parse_init_list_expr() noexcept
{
    eat_token();   

    std::vector<ASTNodeRef> init_values;

    while (!is_cur_token(TokenType::RBRACE)) {
        auto expr = parse_expr();
        if (!expr) return std::unexpected{ expr.error() };

        init_values.push_back(*expr);

        if (is_cur_token(TokenType::COMMA)) eat_token();
    }

    EXPECT_RBRACE();

    return ast_.emplace<Syntax::InitListExpr>(std::move(init_values)); 
}