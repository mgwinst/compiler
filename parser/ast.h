#pragma once

#include <vector>
#include <variant>
#include <utility>
#include <optional>
#include <unordered_map>

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
            FuncCallExpr
        >;

    using Parameters = std::vector<std::pair<std::string, std::string>>;
    using Fields = std::vector<std::pair<std::string, std::string>>;

    // ************** DECLARATIONS **************

    struct LocalVariableDecl
    {
        std::string type, ident;
        std::optional<Expr> init;
        std::optional<bool> is_const;

        LocalVariableDecl(std::string type, std::string ident, std::optional<Expr> init = std::nullopt, std::optional<bool> is_const);
    };
    
    struct GlobalVariableDecl
    {
        std::string type, ident;
        std::optional<Expr> init;
        std::optional<bool> is_const;

        GlobalVariableDecl(std::string type, std::string ident, std::optional<Expr> init = std::nullopt, std::optional<bool> is_const);
    };

    struct StructDecl
    {
        std::string ident;
        std::vector<std::pair<std::string, std::string>> fields;

        StructDecl(std::string ident, std::vector<std::pair<std::string, std::string>> fields);
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
        Expr cond, body;

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
        std::string left_name;
        Expr value;

        AssignExpr(std::string left_name, Expr value);
    };

    struct IndexingExpr
    {
        std::string array;
        Expr index;

        IndexingExpr(std::string array, Expr index);
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
