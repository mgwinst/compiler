#pragma once

#include <vector>
#include <variant>
#include <utility>
#include <optional>
#include <format>

// StmtRef body -> blocks

using DeclRef = std::size_t;
using ExprRef = std::size_t;
using StmtRef = std::size_t;

// ************** DECLARATIONS **************

struct VarDecl
{
    bool is_const_;
    std::string type_, ident_;
    std::optional<ExprRef> init_;

    VarDecl(bool is_const, std::string type, std::string ident, std::optional<ExprRef> init = std::nullopt);
};

struct StructDecl
{
    std::string ident_;
    std::vector<std::pair<std::string, std::string>> fields_;

    StructDecl(std::string ident, std::vector<std::pair<std::string, std::string>> fields);
};

struct FuncDecl
{
    std::string ident_, return_type_;
    std::vector<std::pair<std::string, std::string>> parameters_;
    StmtRef body_;

    FuncDecl(std::string ident, std::string return_type, std::vector<std::pair<std::string, std::string>> parameters, StmtRef body);
};

// ************** STATEMENTS **************

struct CompoundStmt
{
    std::vector<std::variant<DeclRef, StmtRef, ExprRef>> contents_;

    CompoundStmt(std::vector<std::variant<DeclRef, StmtRef, ExprRef>> contents);
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
    StmtRef body_;

    WhileStmt(ExprRef cond, StmtRef body);
};

struct ForStmt
{
    ExprRef init_, cond_, update_;
    StmtRef body_;

    ForStmt(ExprRef init, ExprRef cond, ExprRef update, StmtRef body);
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
    std::string ident_;

    RefExpr(std::string ident);
};

struct IndexExpr
{
    std::string array_;
    ExprRef index_;

    IndexExpr(std::string array, ExprRef index);
};

struct CallExpr
{
    std::string ident_;
    std::vector<ExprRef> args_;

    CallExpr(std::string ident, std::vector<ExprRef> args);
};

using Decl = std::variant<
    VarDecl,
    StructDecl,
    FuncDecl
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

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

/*
    template <typename T>
    [[nodiscard]] auto to_string(const T& expr) -> std::string {
        return std::visit(overloaded{
            [](const LocalVarDecl& var) {},
            [](const FuncDecl& func) {},
            [](const StructDecl& struct) {},
        }, expr);
    }
*/

struct AST
{
    std::vector<Decl> decls;
    std::vector<Expr> exprs;

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
};