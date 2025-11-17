#pragma once

#include <vector>
#include <variant>
#include <utility>
#include <optional>
#include <format>
#include <print>

#include "utils/utils.hpp"

using DeclRef = std::size_t;
using ExprRef = std::size_t;

// ************** DECLARATIONS **************

struct VarDecl
{
    std::string type_, name_;
    std::optional<ExprRef> init_;

    VarDecl(std::string type, std::string name, std::optional<ExprRef> init = std::nullopt);
};

struct ConstVarDecl
{
    std::string type_, name_;
    ExprRef init_;

    ConstVarDecl(std::string type, std::string name, ExprRef init); // const must have init value
};

struct StructDecl
{
    std::string name_;
    std::vector<std::pair<std::string, std::string>> fields_;

    StructDecl(std::string name, std::vector<std::pair<std::string, std::string>> fields);
};

struct FuncDecl
{
    std::string name_, return_type_;
    std::vector<std::pair<std::string, std::string>> parameters_;
    ExprRef body_;

    FuncDecl(std::string name, std::string return_type, std::vector<std::pair<std::string, std::string>> parameters, ExprRef body);
};

struct ParamDecl
{
    bool is_const_;
    std::string type_, name_;

    ParamDecl(bool is_const, std::string type, std::string name);
};

// ************** STATEMENTS **************

struct CompoundStmt
{
    std::vector<DeclRef> decls_;
    std::vector<ExprRef> exprs_;

    CompoundStmt(std::vector<DeclRef> decls, std::vector<ExprRef> exprs);
};

struct ReturnStmt
{
    ExprRef value_;

    ReturnStmt(ExprRef value);
};

struct IfStmt
{
    ExprRef cond_;
    std::vector<ExprRef> if_else_exprs_;

    IfStmt(ExprRef cond, std::vector<ExprRef> if_else_exprs);
};

struct WhileStmt
{
    ExprRef cond_;
    ExprRef body_;

    WhileStmt(ExprRef cond, ExprRef body);
};

struct ForStmt
{
    ExprRef init_, cond_, update_;
    ExprRef body_;

    ForStmt(ExprRef init, ExprRef cond, ExprRef update, ExprRef body);
};

// ************** EXPRESSIONS **************

struct IntegerLiteralExpr
{
    int32_t value_;

    IntegerLiteralExpr(int32_t value);
};

struct FloatLiteralExpr
{
    float value_;

    FloatLiteralExpr(float value);
};

struct CharLiteralExpr
{
    const char value_;

    CharLiteralExpr(const char value);
};

struct StringLiteralExpr
{
    std::string value_;

    StringLiteralExpr(std::string value);
};

struct BooleanExpr
{
    bool value_;

    BooleanExpr(bool value);
};

struct UnaryExpr
{
    std::string op_;
    ExprRef arg_;

    UnaryExpr(std::string op, ExprRef arg);
};

struct BinaryExpr
{
    std::string op_;
    ExprRef left_, right_;

    BinaryExpr(std::string op, ExprRef left, ExprRef right);
};

struct RefExpr
{
    std::string name_;

    RefExpr(std::string name);
};

struct IndexExpr
{
    std::string array_;
    ExprRef index_;

    IndexExpr(std::string array, ExprRef index);
};

struct CallExpr
{
    std::string name_;
    std::vector<ExprRef> args_;

    CallExpr(std::string name, std::vector<ExprRef> args);
};

using Decl = std::variant<
    VarDecl,
    ConstVarDecl,
    StructDecl,
    FuncDecl,
    ParamDecl
    >;

using Expr = std::variant<
    CompoundStmt,
    ReturnStmt,
    IfStmt,
    WhileStmt,
    ForStmt,
    IntegerLiteralExpr,
    FloatLiteralExpr,
    CharLiteralExpr,
    StringLiteralExpr,
    BooleanExpr,
    UnaryExpr,
    BinaryExpr,
    RefExpr,
    IndexExpr,
    CallExpr
    >;

template <typename T>
[[nodiscard]] auto to_string(const T& expr) -> std::string {
    return std::visit(overloaded{
        [](const VarDecl& var) {
            return std::format("VarDecl ['{}', {}]", var.name_, var.type_);
        },
        [](const ConstVarDecl& var) {
            return std::format("ConstVarDecl ['{}', {}]", var.name_, var.type_);
        },
        [](const FuncDecl& func) {
            std::string param_types;
            for (const auto& type : func.parameters_) {
                param_types += ", " + type.second;
            }
            return std::format("FuncDecl '{}' ({}) -> ({})", func.name_, param_types, func.return_type_);
        },
        [](const StructDecl& s) {
            return std::format("");
        },
        [](const ParamDecl& param) {
            return std::format("");
        }
    }, expr);
}

class AST
{
public:
    template <typename T>
    DeclRef add_decl(T d)
    {
        decls.push_back(d);
        return decls.size() - 1;
    }

    template <typename T>
    ExprRef add_expr(T e)
    {
        exprs.push_back(e);
        return exprs.size() - 1;
    }

    void print()
    {
        for (const auto& item : decls) {
            std::println("{}", to_string(item));
        }
    }

private:
    std::vector<Decl> decls;
    std::vector<Expr> exprs;
};

/*
    struct CompilationUnit
    {
        std::string file_name;
        AST ast;

        (global symbol table)
    };
*/