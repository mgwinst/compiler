#pragma once

#include <variant>
#include <vector>
#include <memory>
#include <print>
#include <span>

struct Expr;
using ExprHandle = std::unique_ptr<Expr>;

struct IntegerLiteralExpr {
    int value;
};

struct FloatLiteralExpr {
    float value;
};

struct CharLiteralExpr {
    const char ch;
};

struct StringLiteralExpr {
    std::string_view value;
};

struct ArrayLiteralExpr {
    std::vector<ExprHandle> array_elems;
};

struct ArrayIndexingExpr {
    std::string_view array;
    ExprHandle index;
};

struct BooleanExpr {
    bool value;
};

struct UnaryExpr {
    std::string_view op;
    ExprHandle arg;
};

struct BinaryExpr {
    const char op;
    ExprHandle lhs, rhs;
};

struct VariableExpr {
    std::string_view name;
};

struct IfExpr {
    ExprHandle cond_expr;
    std::vector<ExprHandle> if_else_exprs;
};

struct AssignExpr {
    std::string_view var_name;
    ExprHandle value;
};

struct CompoundExpr {
    std::vector<ExprHandle> expressions;
};

// types only
struct FuncDeclExpr {
    std::string_view name;
    std::string_view return_type;
    std::vector<std::string_view> param_types;
};

struct FuncDefExpr {
    std::string_view name;
    std::string_view return_type;
    std::vector<std::pair<std::string_view, std::string_view>> parameters;
    ExprHandle body;
};

struct FuncCallExpr {
    std::string_view name;
    std::vector<ExprHandle> args;
};

struct ReturnExpr {
    ExprHandle value;
};

struct WhileExpr {
    ExprHandle condition;
    ExprHandle body;
};

struct ForExpr {
    ExprHandle init;
    ExprHandle condition;
    ExprHandle post;
    ExprHandle body;
};

struct StructExpr {
    std::string_view name;
    std::vector<std::pair<std::string_view, std::string_view>> members;

};

struct UnionExpr {
    std::string_view name;
    std::vector<std::string_view> members;
};

struct Expr {
    std::variant<
        IntegerLiteralExpr,
        FloatLiteralExpr,
        CharLiteralExpr,
        StringLiteralExpr,
        ArrayLiteralExpr,
        ArrayIndexingExpr,
        BooleanExpr,
        BinaryExpr,
        VariableExpr,
        IfExpr,
        AssignExpr,
        CompoundExpr,
        FuncDeclExpr,
        FuncDefExpr,
        FuncCallExpr,
        ReturnExpr,
        WhileExpr,
        ForExpr,
        StructExpr,
        UnionExpr
    > kind;
};


