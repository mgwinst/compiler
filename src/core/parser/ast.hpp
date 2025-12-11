#pragma once

#include <vector>
#include <variant>
#include <optional>
#include <utility>

#include "utils/utils.hpp"
#include "utils/concepts.hpp"

using DeclRef = std::size_t;
using ExprRef = std::size_t;

// ************** DECLARATIONS **************

struct CompilationUnitDecl
{
    std::string name_;
    std::vector<DeclRef> decls_;

    CompilationUnitDecl(StringLike auto&& name) noexcept :
        name_{ std::forward<decltype(name)>(name) },
        decls_{} {}

    CompilationUnitDecl(StringLike auto&& name, Contiguous auto&& v) noexcept :
        name_{ std::forward<decltype(name)>(name) },
        decls_{ std::forward<decltype(name)>(v) } {}
};

struct VarDecl
{
    using Initializer = std::variant<ExprRef, std::vector<ExprRef>>;

    Constness constness_;
    std::string type_, name_;
    std::optional<Initializer> init_;

    VarDecl(Constness constness, StringLike auto&& type, StringLike auto&& name, std::optional<Initializer> init = std::nullopt) noexcept :
        constness_{ constness },
        type_{ std::forward<decltype(type)>(type) }, 
        name_{ std::forward<decltype(name)>(name) }, 
        init_{ std::move(init) } {}
};

struct ParamDecl
{
    Constness constness_;
    std::string type_, name_;

    ParamDecl(Constness constness, StringLike auto&& type, StringLike auto&& name) noexcept :
        constness_{ constness },
        type_{ std::forward<decltype(type)>(type) }, 
        name_{ std::forward<decltype(name)>(name) } {}
};

struct FuncDecl
{
    Constness constness_;
    std::string name_, return_type_;
    std::vector<DeclRef> params_;
    ExprRef body_;

    FuncDecl(Constness constness, StringLike auto&& name, StringLike auto&& return_type, Contiguous auto&& params, ExprRef body) noexcept :
        constness_{ constness },
        name_{ std::forward<decltype(name)>(name) },
        return_type_{ std::forward<decltype(return_type)>(return_type) },
        params_{ std::forward<decltype(params)>(params) },
        body_{ body } {}
};

struct StructDef
{
    std::string type_;
    std::vector<DeclRef> fields_;

    StructDef(StringLike auto&& type, Contiguous auto&& fields) noexcept :
        type_{ std::forward<decltype(type)>(type) },
        fields_{ std::forward<decltype(fields)>(fields) } {}
};

// ************** EXPRESSIONS **************

struct CompoundStmt
{
    std::vector<DeclRef> decls_;
    std::vector<ExprRef> exprs_;
    std::optional<ExprRef> return_stmt_;

    CompoundStmt(Contiguous auto&& decls, Contiguous auto&& exprs, std::optional<ExprRef> return_stmt = std::nullopt) noexcept :
        decls_{ std::forward<decltype(decls)>(decls) },
        exprs_{ std::forward<decltype(exprs)>(exprs) },
        return_stmt_{ return_stmt } {}

    CompoundStmt(Contiguous auto&& exprs, std::optional<ExprRef> return_stmt = std::nullopt) noexcept :
        exprs_{ std::forward<decltype(exprs)>(exprs) },
        return_stmt_{ return_stmt } {}
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

    IfStmt(ExprRef cond, Contiguous auto&& if_else_exprs) noexcept :
        cond_{ cond },
        if_else_exprs_{ std::forward<decltype(if_else_exprs)>(if_else_exprs) } {}
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

    StringLiteralExpr(StringLike auto&& value) :
        value_{ std::forward<decltype(value)>(value) } {}
};

struct BooleanLiteralExpr
{
    bool value_;

    BooleanLiteralExpr(bool value) noexcept :
        value_{ value } {}
};

struct UnaryExpr
{
    std::string op_;
    ExprRef operand_;
    bool is_postfix_ = false;

    UnaryExpr(StringLike auto&& op, ExprRef operand, bool is_postfix = false) noexcept :
        op_{ std::forward<decltype(op)>(op) },
        operand_{ operand },
        is_postfix_{ is_postfix } {}
};

struct BinaryExpr
{
    std::string op_;
    ExprRef left_, right_;

    BinaryExpr(StringLike auto&& op, ExprRef left, ExprRef right) noexcept :
        op_{ std::forward<decltype(op)>(op) },
        left_{ left },
        right_{ right } {}
};

struct ReferenceExpr
{
    std::string name_;

    ReferenceExpr(StringLike auto&& name) noexcept : 
        name_{ std::forward<decltype(name)>(name) } {}
};

struct CallExpr
{
    ExprRef callee_;
    std::vector<ExprRef> args_;

    CallExpr(ExprRef callee, Contiguous auto&& args) noexcept :
        callee_{ callee },
        args_{ std::forward<decltype(args)>(args) } {}
};

struct MemberExpr
{
    // std::string name_; .x / ->x
    ExprRef base_;
    ExprRef member_;
    
    MemberExpr(ExprRef base, ExprRef member) : 
        base_{ base },
        member_{ member } {}
};

struct ArraySubscriptExpr
{
    ExprRef base_;
    ExprRef index_;

    ArraySubscriptExpr(ExprRef base, ExprRef index) noexcept :
        base_{ base },
        index_{ index } {}
};

struct InitListExpr
{
    std::vector<ExprRef> init_values_;
    
    InitListExpr(Contiguous auto&& init_values) :
        init_values_{ std::forward<decltype(init_values)(init_values) } {}
};

struct ExplicitCastExpr
{

};

struct ImplicitCastExpr
{

};

struct NewExpr
{

};

struct ConstructExpr
{

};

struct DestructExpr
{

};

using Decl = std::variant<
    CompilationUnitDecl,
    VarDecl,
    ParamDecl,
    FuncDecl,
    StructDef
    >;

using Stmt = std::variant<
    CompoundStmt,
    ReturnStmt,
    IfStmt, 
    WhileStmt, 
    ForStmt
    >;

using LiteralExpr = std::variant<
    IntegerLiteralExpr,
    FloatLiteralExpr,
    CharLiteralExpr,
    StringLiteralExpr,
    BooleanLiteralExpr
    >;

using PrimaryExpr = std::variant<
    LiteralExpr,
    ReferenceExpr,
    CallExpr,
    MemberExpr,
    ArraySubscriptExpr,
    InitListExpr
    >;

using CastExpr = std::variant<
    ExplicitCastExpr, 
    ImplicitCastExpr
    >;

using LifetimeExpr = std::variant<
    NewExpr, 
    ConstructExpr, 
    DestructExpr
    >;

using Expr = std::variant<
    Stmt,
    PrimaryExpr,
    UnaryExpr,
    BinaryExpr,
    CastExpr,
    LifetimeExpr
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

    std::string decl_to_str(const DeclRef ref, std::string indent) const noexcept;
    std::string expr_to_str(const ExprRef ref, std::string indent) const noexcept;

    void print() const noexcept;
};

struct SymbolTable
{
    int x;
};

struct CompilationUnit
{
    AST ast_;
    SymbolTable symbol_table_;
};