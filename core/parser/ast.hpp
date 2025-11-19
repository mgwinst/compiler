#pragma once

#include <vector>
#include <variant>
#include <utility>
#include <optional>
#include <format>
#include <print>
#include <type_traits>

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

struct ParamDecl
{
    bool is_const_;
    std::string type_, name_;

    ParamDecl(bool is_const, std::string type, std::string name);
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
    std::vector<DeclRef> params_;
    ExprRef body_;

    FuncDecl(std::string name, std::string return_type, std::vector<DeclRef> params, ExprRef body);
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

struct AST;

template <typename T>
[[nodiscard]] auto node_to_str(const AST& ast, const T& node) -> std::string;

struct AST
{
    std::vector<Decl> decls;
    std::vector<Expr> exprs;

    template <typename T>
    DeclRef add_decl(T&& d)
    {
        decls.push_back(std::forward<T>(d));
        return decls.size() - 1;
    }

    template <typename T>
    DeclRef add_decl(const T& d)
    {
        decls.push_back(d);
        return decls.size() - 1;
    }

    template <typename T>
    ExprRef add_expr(T&& e)
    {
        exprs.push_back(std::forward<T>(e));
        return exprs.size() - 1;
    }

    template <typename T>
    ExprRef add_expr(const T& e)
    {
        exprs.push_back(e);
        return exprs.size() - 1;
    }

    void print()
    {
        for (const auto& item : decls) {
            std::println("{}", node_to_str(*this, item));
        }
    }
};

template <typename> constexpr bool always_false_v = false;

template <typename T>
[[nodiscard]] auto node_to_str(const AST &ast, const T &node) -> std::string
{
    return std::visit([&ast] <typename U> (U&& node) -> std::string {
        using NodeType = std::decay_t<decltype(node)>;

        if constexpr (std::is_same_v<NodeType, VarDecl>)
        {
            if (!node.init_)
                return std::format("VarDecl ['{}', {}]", node.name_, node.type_);
            else
                return std::format("VarDecl ['{}', {}]\n\t {}", node.name_, node.type_, node_to_str(ast, ast.decls.at(*node.init_)));
        }
        else if constexpr (std::is_same_v<NodeType, ConstVarDecl>)
        {
            return std::format("ConstVarDecl ['{}', {}]", node.name_, node.type_);
        }
        else if constexpr (std::is_same_v<NodeType, ParamDecl>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, FuncDecl>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, StructDecl>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, CompoundStmt>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, ReturnStmt>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, IfStmt>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, WhileStmt>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, ForStmt>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, IntegerLiteralExpr>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, FloatLiteralExpr>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, CharLiteralExpr>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, StringLiteralExpr>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, BooleanExpr>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, UnaryExpr>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, BinaryExpr>)
        {
            return std::format("BinOp ['{}']\n\t {}\n {}\n}", node.op_, node_to_str(ast, ast.exprs.at(node.left_)), node_to_str(ast, ast.exprs.at(node.right_)));
        }
        else if constexpr (std::is_same_v<NodeType, RefExpr>)
        {
            return std::format("RefExpr ['{}']", node.name_);
        }
        else if constexpr (std::is_same_v<NodeType, IndexExpr>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, CallExpr>)
        {
            return std::format("");
        }
        else
        {
            static_assert(always_false_v<NodeType>, "type not defined in visitor...");
        }
    }, node);
}

struct CompilationUnit
{
    AST ast;
    // SymbolTable symbol_table;
};