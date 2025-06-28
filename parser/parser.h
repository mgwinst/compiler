#pragma once

struct Parser {
    Lexer lexer;
    double num_value{};
    std::string ident_str{};
    
    Parser(std::string source_text) : lexer{source_text} {}

    std::unique_ptr<AST::Expr> LogError(std::string_view str);
    std::unique_ptr<AST::Expr> ParseIntegerLiteralExpr();
    std::unique_ptr<AST::Expr> ParseParenExpr();
    std::unique_ptr<AST::Expr> ParseIdentifierExpr();
    std::unique_ptr<AST::Expr> ParsePrimary();
};
