#pragma once

#include <vector>
#include <variant>
#include <utility>
#include <optional>
#include <format>
#include <print>
#include <type_traits>

#include "utils/utils.hpp"
#include "utils/concepts.hpp"

using DeclRef = std::size_t;
using ExprRef = std::size_t;

// ************** DECLARATIONS **************

struct CompilationUnitDecl
{
    std::string name_;
    std::vector<DeclRef> decls_;

    template <StringConvertible T, std::ranges::contiguous_range Vec>
    CompilationUnitDecl(T&& name, Vec&& v) noexcept :
        name_{ std::forward<T>(name) },
        decls_{ std::forward<Vec>(v) } {}
};

struct VarDecl
{
    std::string type_, name_;
    std::optional<ExprRef> init_;

    template <typename T, typename U>
    VarDecl(T&& type, U&& name, std::optional<ExprRef> init = std::nullopt) noexcept :
        type_{ std::forward<T>(type) }, 
        name_{ std::forward<U>(name) }, 
        init_{ init } {}
};

struct ConstVarDecl
{
    std::string type_, name_;
    ExprRef init_;

    template <StringConvertible T, StringConvertible U>
    ConstVarDecl(T&& type, U&& name, ExprRef init) noexcept :
        type_{ std::forward<T>(type) },
        name_{ std::forward<U>(name) },
        init_{ init } {}
};

struct ParamDecl
{
    bool is_const_;
    std::string type_, name_;

    template <StringConvertible T, StringConvertible U>
    ParamDecl(bool is_const, T&& type, U&& name) noexcept :
        is_const_{ is_const },
        type_{ std::forward<T>(type) },
        name_{ std::forward<U>(name) } {}
};

struct StructDecl
{
    std::string name_;
    std::vector<std::pair<std::string, std::string>> fields_;

    template <StringConvertible T, std::ranges::contiguous_range Vec>
    StructDecl(T&& name, Vec&& fields) noexcept :
        name_{ std::forward<T>(name) },
        fields_{ std::forward<Vec>(fields) } {}
};

struct FuncDecl
{
    std::string name_, return_type_;
    std::vector<DeclRef> params_;
    ExprRef body_;

    template <StringConvertible T, StringConvertible U, std::ranges::contiguous_range Vec>
    FuncDecl(T&& name, U&& return_type, Vec&& params, ExprRef body) noexcept :
        name_{ std::forward<T>(name) },
        return_type_{ std::forward<U>(return_type) },
        params_{ std::forward<Vec>(params) },
        body_{ body } {}
};

// ************** STATEMENTS **************

struct CompoundStmt
{
    std::vector<DeclRef> decls_;
    std::vector<ExprRef> exprs_;

    template <std::ranges::contiguous_range Vec>
    CompoundStmt(Vec&& decls, Vec&& exprs) noexcept :
        decls_{ std::forward<Vec>(decls) },
        exprs_{ std::forward<Vec>(exprs) } {}
};

struct ReturnStmt
{
    ExprRef value_;

    ReturnStmt(ExprRef value) noexcept :
        value_{ value } {}
};

struct IfStmt
{
    ExprRef cond_;
    std::vector<ExprRef> if_else_exprs_;

    template <std::ranges::contiguous_range Vec>
    IfStmt(ExprRef cond, Vec&& if_else_exprs) :
        cond_{ cond },
        if_else_exprs_{ std::forward<Vec>(if_else_exprs) } {}
};

struct WhileStmt
{
    ExprRef cond_;
    ExprRef body_;

    WhileStmt(ExprRef cond, ExprRef body) noexcept :
        cond_{ cond },
        body_{ body } {}
};

struct ForStmt
{
    ExprRef init_, cond_, update_;
    ExprRef body_;

    ForStmt(ExprRef init, ExprRef cond, ExprRef update, ExprRef body) noexcept :
        init_{ init },
        cond_{ cond },
        update_{ update },
        body_{ body } {}
};

// ************** EXPRESSIONS **************

struct IntegerLiteralExpr
{
    int32_t value_;

    IntegerLiteralExpr(int32_t value) noexcept :
        value_{ value } {}
};

struct FloatLiteralExpr
{
    float value_;

    FloatLiteralExpr(float value) noexcept :
        value_{ value } {}
};

struct CharLiteralExpr
{
    char value_;
   
    CharLiteralExpr(char value) noexcept :
        value_{ value } {}
};

struct StringLiteralExpr
{
    std::string value_;

    template <StringConvertible T>
    StringLiteralExpr(T&& value) :
        value_{ std::forward<T>(value) } {}
};

struct BooleanExpr
{
    bool value_;

    BooleanExpr(bool value) noexcept :
        value_{ value } {}
};

struct UnaryExpr
{
    std::string op_;
    ExprRef arg_;

    template <StringConvertible T>
    UnaryExpr(T&& op, ExprRef arg) noexcept :
        op_{ std::forward<T>(op) },
        arg_{ arg } {}
};

struct BinaryExpr
{
    std::string op_;
    ExprRef left_, right_;

    template <StringConvertible T>
    BinaryExpr(T&& op, ExprRef left, ExprRef right) noexcept :
        op_{ std::forward<T>(op) },
        left_{ left },
        right_{ right } {}
};

struct RefExpr
{
    std::string name_;

    template <StringConvertible T>
    RefExpr(T&& name) noexcept : 
        name_{ std::forward<T>(name) } {}
};

struct IndexExpr
{
    std::string array_;
    ExprRef index_;

    template <StringConvertible T>
    IndexExpr(T&& array, ExprRef index) noexcept :
        array_{ std::forward<T>(array) },
        index_{ index } {}
};

struct CallExpr
{
    std::string name_;
    std::vector<ExprRef> args_;


    template <StringConvertible T, std::ranges::contiguous_range Vec>
    CallExpr(T&& name, Vec&& args) noexcept :
        name_{ std::forward<T>(name) },
        args_{ std::forward<Vec>(args) } {}
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

    template <typename T, typename... Args>
    [[nodiscard]] DeclRef add_decl(Args&&... args)
    {
        decls.emplace_back(std::in_place_type<T>, std::forward<Args>(args)...);
        return decls.size() - 1;
    }

    template <typename T, typename... Args>
    [[nodiscard]] ExprRef add_expr(Args&&... args)
    {
        exprs.emplace_back(std::in_place_type<T>, std::forward<Args>(args)...);
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