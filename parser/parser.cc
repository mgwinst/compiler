#include <utility>

#include "token.h"
#include "lexer/lexer.h"
#include "parser.h"
#include "ast_nodes.h"

auto Parser::LogError(std::string_view str) -> std::unique_ptr<AST::Expr> {
    std::cerr << std::format("Error {}\n", str);
    return nullptr;
}

auto Parser::ParseIntegerLiteralExpr() -> std::unique_ptr<AST::Expr> {
    auto result = std::make_unique<AST::Expr>(std::in_place_type<AST::IntegerLiteralExpr>, num_value);
    lexer.cur_token = lexer.get_token();
    return result;
}

auto Parser::ParseParenExpr() -> std::unique_ptr<AST::Expr> {
    lexer.cur_token = lexer.get_token();
    auto v = ParseExpr();

    if (!v) return nullptr;
    if (lexer.cur_token.lexeme.value() != ")") return LogError("Expected ')'");

    lexer.cur_token = lexer.get_token();
    return v;
}

auto Parser::ParseIdentifierExpr() -> std::unique_ptr<AST::Expr> {
    std::string id_name = ident_str;
    lexer.cur_token = lexer.get_token();

    if (lexer.cur_token.lexeme.value() != "(")
        return std::make_unique<AST::Expr>(std::in_place_type<AST::VariableExpr>, id_name);
    
    lexer.cur_token = lexer.get_token();
    std::vector<std::unique_ptr<AST::Expr>> args;
    if (lexer.cur_token.lexeme.value() != ")") {
        while (true) {
            if (auto arg = ParseExpr()) args.push_back(std::move(arg));
            else return nullptr;
            if (lexer.cur_token.lexeme.value() == ")") break;
            if (lexer.cur_token.lexeme.value() != ",") return LogError("Expected ')' or ',' in argument list");
            lexer.cur_token = lexer.get_token();
        }
    }

    lexer.cur_token = lexer.get_token();
    return std::make_unique<AST::Expr>(std::in_place_type<AST::FuncCallExpr>, id_name, std::move(args));
}

auto Parser::ParsePrimary() -> std::unique_ptr<AST::Expr> {
    switch (lexer.cur_token.type) {
    default:
        return LogError("unknown token when expecting an expression");
    case TokenType::IDENTIFIER:
        return ParseIdentifierExpr();
    case TokenType::NUMERIC_LITERAL:
        return ParseIntegerLiteralExpr();
    case TokenType::LPAREN:
        return ParseParenExpr();
    }
}