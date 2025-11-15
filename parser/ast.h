#pragma once

#include <vector>
#include <variant>
#include <utility>
#include <optional>

// StmtRef body -> blocks

using DeclRef = std::size_t;
using ExprRef = std::size_t;
using StmtRef = std::size_t;

// ************** DECLARATIONS **************

struct GlobalVarDecl
{
    bool is_const;
    std::string type, ident;
    std::optional<ExprRef> init;

    GlobalVarDecl(bool is_const, std::string type, std::string ident, std::optional<ExprRef> init = std::nullopt);
};

struct LocalVarDecl
{
    bool is_const;
    std::string type, ident;
    std::optional<ExprRef> init;

    LocalVarDecl(bool is_const, std::string type, std::string ident, std::optional<ExprRef> init = std::nullopt);
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
    StmtRef body;

    FuncDecl(std::string ident, std::string return_type, std::vector<std::pair<std::string, std::string>> parameters, StmtRef body);
};

// ************** STATEMENTS **************

struct BlockStmt
{
    std::vector<std::variant<DeclRef, StmtRef, ExprRef>> contents; // any tricks?

    BlockStmt(std::vector<std::variant<DeclRef, StmtRef, ExprRef>> contents);
};

struct ReturnStmt
{
    ExprRef value;

    ReturnStmt(ExprRef value);
};

struct IfStmt
{
    ExprRef cond;
    std::vector<ExprRef> if_else_exprs;

    IfStmt(ExprRef cond, std::vector<ExprRef> if_else_exprs);
};

struct WhileStmt
{
    ExprRef cond;
    StmtRef body;

    WhileStmt(ExprRef cond, StmtRef body);
};

struct ForStmt
{
    ExprRef init, cond, update;
    StmtRef body;

    ForStmt(ExprRef init, ExprRef cond, ExprRef update, StmtRef body);
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
    ExprRef arg;

    UnaryExpr(std::string op, ExprRef arg);
};

struct BinaryExpr
{
    std::string op;
    ExprRef left, right;

    BinaryExpr(std::string op, ExprRef left, ExprRef right);
};

struct VariableExpr
{
    std::string ident;
    VariableExpr(std::string);
};

struct AssignExpr
{
    std::string assignee;
    ExprRef value;

    AssignExpr(std::string assignee, ExprRef value);
};

struct IndexExpr
{
    std::string array;
    ExprRef index;

    IndexExpr(std::string array, ExprRef index);
};

struct CallExpr
{
    std::string ident;
    std::vector<ExprRef> args;

    CallExpr(std::string ident, std::vector<ExprRef> args);
};

using Decl = std::variant<
    GlobalVarDecl,
    LocalVarDecl,
    StructDecl,
    FuncDecl
    >;

using Stmt = std::variant<
    BlockStmt,
    ReturnStmt,
    IfStmt,
    WhileStmt,
    ForStmt
    >;

using Expr = std::variant<
    IntegerLiteralExpr,
    FloatLiteralExpr,
    CharLiteralExpr,
    StringLiteralExpr,
    BooleanExpr,
    UnaryExpr,
    BinaryExpr,
    VariableExpr,
    AssignExpr,
    IndexExpr,
    CallExpr
    >;

struct AST
{
    std::vector<Decl> decls;
    std::vector<Stmt> stmts;
    std::vector<Expr> exprs;

    template <typename T>
    DeclRef add_decl(T d)
    {
        decls.push_back(d);
        return decls.size() - 1;
    }

    template <typename T>
    StmtRef add_stmt(T s)
    {
        stmts.push_back(s);
        return stmts.size() - 1;
    }
    
    template <typename T>
    ExprRef add_expr(T e)
    {
        exprs.push_back(d);
        return exprs.size() - 1;
    }
};