#include "token.h"
#include "lexer.h"
#include "ast_nodes.h"

namespace {
    double numeric_value;
    std::string identifier_str;
    Lexer lexer;

}

std::unique_ptr<AST::Expr> LogError(std::string_view str) {
    std::cerr << std::format("Error {}\n", str);
    return nullptr;
}

std::unique_ptr<AST::Expr> ParseIntegerLiteralExpr() {
    auto result = std::make_unique<AST::Expr>(std::in_place_type<AST::IntegerLiteralExpr>, numeric_value);
    lexer.cur_token = lexer.get_token();
    return result;
}

std::unique_ptr<AST::Expr> ParseParenExpr() {
    lexer.cur_token = lexer.get_token();
    auto v = ParseExpr();

    if (!v) return nullptr;
    if (lexer.cur_token.lexeme.value() != ")") return LogError("Expected ')'");

    lexer.cur_token = lexer.get_token();
    return v;
}

std::unique_ptr<AST::Expr> ParseIdentifierExpr() {
    std::string id_name = identifier_str;
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

std::unique_ptr<AST::Expr> ParsePrimary() {
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
