#pragma once

#include <vector>
#include <variant>
#include <memory>
#include <utility>
#include <format>
#include <type_traits>

namespace AST {

    struct ReturnStatement;
    struct IfStatement;
    struct WhileStatement;
    struct ForStatement;
    struct IntegerLiteralExpr;
    struct FloatLiteralExpr;
    struct CharLiteralExpr;
    struct StringLiteralExpr;
    struct BooleanExpr;
    struct UnaryExpr;
    struct BinaryExpr;
    struct VariableExpr;
    struct AssignExpr;
    struct IndexingExpr;
    struct BlockExpr;
    struct FuncCallExpr;
    
    using Expr = std::variant<   
            ReturnStatement,
            IfStatement,
            WhileStatement,
            ForStatement,
            IntegerLiteralExpr,
            FloatLiteralExpr,
            CharLiteralExpr,
            StringLiteralExpr,
            BooleanExpr,
            UnaryExpr,
            BinaryExpr,
            VariableExpr,
            AssignExpr,
            IndexingExpr,
            BlockExpr,
            FuncCallExpr>;

    // ************** DECLARATIONS **************

    struct VariableDecl
    {
        std::string type, ident;

        VariableDecl(std::string, std::string);
    };

    struct FuncDecl
    {
        std::string ident, return_type;
        std::vector<std::pair<std::string, std::string>> parameters;
        Expr body;

        FuncDecl(std::string ident, std::string return_type, std::vector<std::pair<std::string, std::string>> parameters, Expr body);
    };

    // ************** STATEMENTS **************

    struct ReturnStatement
    {
        Expr value;

        ReturnStatement(Expr value);
    };

    struct IfStatement
    {
        Expr cond;
        std::vector<Expr> if_else_exprs;

        IfStatement(Expr cond, std::vector<Expr> if_else_exprs);
    };

    struct WhileStatement
    {
        Expr cond;
        Expr body;

        WhileStatement(Expr cond, Expr body);
    };

    struct ForStatement
    {
        Expr init, cond, update, body;

        ForStatement(Expr init, Expr cond, Expr update, Expr body);
    };

    // ************** EXPRESSIONS **************

    struct IntegerLiteralExpr
    {
        int32_t value;
        IntegerLiteralExpr(int32_t value);
    };

    struct FloatLiteralExpr
    {
        float value;
        FloatLiteralExpr(float value);
    };

    struct CharLiteralExpr
    {
        const char value;
        CharLiteralExpr(const char value);
    };

    struct StringLiteralExpr
    {
        std::string value;
        StringLiteralExpr(std::string value);
    };

    struct BooleanExpr
    {
        bool value;
        BooleanExpr(bool value);
    };

    struct UnaryExpr
    {
        std::string op;
        Expr arg;

        UnaryExpr(std::string op, Expr arg);
    };

    struct BinaryExpr
    {
        std::string op;
        Expr left, right;

        BinaryExpr(const char op, Expr left, Expr right);
    };

    struct VariableExpr
    {
        std::string ident;
        VariableExpr(std::string);
    };

    struct AssignExpr
    {
        std::string var_name;
        Expr value;

        AssignExpr(std::string var_name, Expr value);
    };

    struct IndexingExpr
    {
        std::string array;
        Expr index;

        IndexingExpr(std::string array, std::unique_ptr<AST::Expr> index);
    };

    struct BlockExpr
    {
        std::vector<Expr> expressions;

        BlockExpr(std::vector<Expr> expressions);
    };

    struct FuncCallExpr
    {
        std::string ident;
        std::vector<Expr> args;

        FuncCallExpr(std::string ident, std::vector<Expr> args);
    };

} // namespace AST
