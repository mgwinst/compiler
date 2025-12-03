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

    template <StringLike T>
    CompilationUnitDecl(T&& name) noexcept :
        name_{ std::forward<T>(name) },
        decls_{} {}

    template <StringLike T, Contiguous Vec>
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

    template <StringLike T, StringLike U>
    ConstVarDecl(T&& type, U&& name, ExprRef init) noexcept :
        type_{ std::forward<T>(type) },
        name_{ std::forward<U>(name) },
        init_{ init } {}
};

struct ParamDecl
{
    bool is_const_;
    std::string type_, name_;

    template <StringLike T, StringLike U>
    ParamDecl(bool is_const, T&& type, U&& name) noexcept :
        is_const_{ is_const },
        type_{ std::forward<T>(type) },
        name_{ std::forward<U>(name) } {}
};

struct FuncDecl
{
    std::string name_, return_type_;
    std::vector<DeclRef> params_;
    ExprRef body_; // at least

    template <StringLike T, StringLike U, Contiguous Vec>
    FuncDecl(T&& name, U&& return_type, Vec&& params, ExprRef body) noexcept :
        name_{ std::forward<T>(name) },
        return_type_{ std::forward<U>(return_type) },
        params_{ std::forward<Vec>(params) },
        body_{ body } {}
};

struct StructDecl
{
    std::string name_;
    std::vector<std::pair<std::string, std::string>> fields_;

    template <StringLike T, Contiguous Vec>
    StructDecl(T&& name, Vec&& fields) noexcept :
        name_{ std::forward<T>(name) },
        fields_{ std::forward<Vec>(fields) } {}
};

// ************** STATEMENTS **************

struct CompoundStmt
{
    std::vector<DeclRef> decls_;
    std::vector<ExprRef> exprs_;

    template <Contiguous Vec>
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

    template <Contiguous Vec>
    IfStmt(ExprRef cond, Vec&& if_else_exprs) noexcept :
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

    template <StringLike T>
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

    template <StringLike T>
    UnaryExpr(T&& op, ExprRef arg) noexcept :
        op_{ std::forward<T>(op) },
        arg_{ arg } {}
};

struct BinaryExpr
{
    std::string op_;
    ExprRef left_, right_;

    template <StringLike T>
    BinaryExpr(T&& op, ExprRef left, ExprRef right) noexcept :
        op_{ std::forward<T>(op) },
        left_{ left },
        right_{ right } {}
};

struct ReferenceExpr
{
    std::string name_;

    template <StringLike T>
    ReferenceExpr(T&& name) noexcept : 
        name_{ std::forward<T>(name) } {}
};

struct IndexExpr
{
    ExprRef base_;
    ExprRef index_;

    IndexExpr(ExprRef base, ExprRef index) noexcept :
        base_{ base },
        index_{ index } {}
};

struct CallExpr
{
    ExprRef callee_;
    std::vector<ExprRef> args_;

    template <Contiguous Vec>
    CallExpr(ExprRef callee, Vec&& args) noexcept :
        callee_{ callee },
        args_{ std::forward<Vec>(args) } {}
};

using Decl = std::variant<
    CompilationUnitDecl,
    VarDecl,
    ConstVarDecl,
    ParamDecl,
    FuncDecl,
    StructDecl
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
    // CompoundAssignExpr += -= ... (clang AST has a special node for this)
    ReferenceExpr,
    IndexExpr,
    CallExpr
    >;

struct AST
{   
    std::vector<Decl> decls_;
    std::vector<Expr> exprs_;

    DeclRef root() const noexcept;

    template <typename T, typename... Args>
    requires std::is_constructible_v<T, Args...>
    [[nodiscard]] DeclRef emplace_decl(Args&&... args) noexcept
    {  
        decls_.emplace_back(std::in_place_type<T>, std::forward<Args>(args)...);
        return decls_.size() - 1;
    }

    template <typename T, typename... Args>
    requires std::is_constructible_v<T, Args...>
    [[nodiscard]] ExprRef emplace_expr(Args&&... args) noexcept
    {
        exprs_.emplace_back(std::in_place_type<T>, std::forward<Args>(args)...);
        return exprs_.size() - 1;
    }

    std::string decl_to_str(DeclRef ref, std::string indent) const noexcept;
    std::string expr_to_str(ExprRef ref, std::string indent) const noexcept;

    void print() const noexcept;
};

struct SymbolTable
{

};

struct CompilationUnit
{
    AST ast_;
    SymbolTable symbol_table_;
};