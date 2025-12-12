#pragma once

#include <vector>
#include <variant>
#include <optional>
#include <utility>
#include <vector>

#include "utils/utils.hpp"
#include "utils/concepts.hpp"

using NodeRef = std::size_t;

// ************** DECLARATIONS **************

struct CompilationUnitDecl
{
    std::string name_;
    std::vector<NodeRef> decls_;

    CompilationUnitDecl(StringLike auto&& name) noexcept :
        name_{ std::forward<decltype(name)>(name) },
        decls_{} {}
};

struct VarDecl
{
    Constness constness_;
    std::string type_, name_;
    std::optional<NodeRef> init_; // single expr or init_list

    VarDecl(Constness constness, StringLike auto&& type, StringLike auto&& name, std::optional<NodeRef> init = std::nullopt) noexcept :
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
    std::vector<NodeRef> params_;
    NodeRef body_;

    FuncDecl(Constness constness, StringLike auto&& name, StringLike auto&& return_type, Contiguous auto&& params, NodeRef body) noexcept :
        constness_{ constness },
        name_{ std::forward<decltype(name)>(name) },
        return_type_{ std::forward<decltype(return_type)>(return_type) },
        params_{ std::forward<decltype(params)>(params) },
        body_{ body } {}
};

struct StructDecl
{
    std::string type_;
    std::vector<NodeRef> fields_;

    StructDecl(StringLike auto&& type, Contiguous auto&& fields) noexcept :
        type_{ std::forward<decltype(type)>(type) },
        fields_{ std::forward<decltype(fields)>(fields) } {}
};

// ************** EXPRESSIONS **************

struct CompoundStmt
{
    std::vector<NodeRef> decls_;
    std::vector<NodeRef> exprs_;
    std::optional<NodeRef> return_stmt_;

    CompoundStmt(Contiguous auto&& decls, Contiguous auto&& exprs, std::optional<NodeRef> return_stmt = std::nullopt) noexcept :
        decls_{ std::forward<decltype(decls)>(decls) },
        exprs_{ std::forward<decltype(exprs)>(exprs) },
        return_stmt_{ return_stmt } {}

    CompoundStmt(Contiguous auto&& exprs, std::optional<NodeRef> return_stmt = std::nullopt) noexcept :
        exprs_{ std::forward<decltype(exprs)>(exprs) },
        return_stmt_{ return_stmt } {}
};

struct ReturnStmt
{
    NodeRef value_;

    ReturnStmt(NodeRef value) noexcept :
        value_{ value } {}
};

struct IfStmt {
    NodeRef cond_;
    NodeRef then_stmt_;
    std::optional<NodeRef> else_stmt_;

    IfStmt(NodeRef cond, NodeRef then_stmt, std::optional<NodeRef> else_stmt_ = std::nullopt) noexcept :
        cond_{ cond },
        then_stmt_{ then_stmt },
        else_stmt_{ else_stmt_ } {}
        
};

struct WhileStmt
{
    NodeRef cond_;
    NodeRef body_;

    WhileStmt(NodeRef cond, NodeRef body) noexcept :
        cond_{ cond },
        body_{ body } {}
};

struct ForStmt
{
    NodeRef init_, cond_, update_;
    NodeRef body_;

    ForStmt(NodeRef init, NodeRef cond, NodeRef update, NodeRef body) noexcept :
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
    NodeRef operand_;
    bool is_postfix_ = false;

    UnaryExpr(StringLike auto&& op, NodeRef operand, bool is_postfix = false) noexcept :
        op_{ std::forward<decltype(op)>(op) },
        operand_{ operand },
        is_postfix_{ is_postfix } {}
};

struct BinaryExpr
{
    std::string op_;
    NodeRef left_, right_;

    BinaryExpr(StringLike auto&& op, NodeRef left, NodeRef right) noexcept :
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
    NodeRef callee_;
    std::vector<NodeRef> args_;

    CallExpr(NodeRef callee, Contiguous auto&& args) noexcept :
        callee_{ callee },
        args_{ std::forward<decltype(args)>(args) } {}
};

struct MemberExpr
{
    NodeRef base_;
    NodeRef member_;
    bool is_arrow_ = false;
    
    MemberExpr(NodeRef base, NodeRef member, bool is_arrow = false) :
        base_{ base },
        member_{ member },
        is_arrow_{ is_arrow } {}
};

struct ArraySubscriptExpr
{
    NodeRef base_;
    NodeRef index_;

    ArraySubscriptExpr(NodeRef base, NodeRef index) noexcept :
        base_{ base },
        index_{ index } {}
};

struct InitListExpr
{
    std::vector<NodeRef> init_values_;
    
    InitListExpr(Contiguous auto&& init_values) :
        init_values_{ std::forward<decltype(init_values)>(init_values) } {}
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

struct DeleteExpr
{

};

struct ConstructExpr
{

};

struct DestructExpr
{

};

enum class NodeKind : uint8_t {
    CompilationUnitDecl,
    VarDecl,
    ParamDecl,
    FuncDecl,
    StructDecl,

    CompoundStmt,
    ReturnStmt,
    IfStmt,
    WhileStmt,
    ForStmt,

    IntegerLiteralExpr,
    FloatLiteralExpr,
    CharLiteralExpr,
    StringLiteralExpr,
    BooleanLiteralExpr,

    UnaryExpr,
    BinaryExpr,
    ReferenceExpr,
    CallExpr,
    MemberExpr,
    ArraySubscriptExpr,
    InitListExpr,

    ExplicitCastExpr,
    ImplicitCastExpr,
    NewExpr,
    DeleteExpr,
    ConstructExpr,
    DestructExpr
};

struct Node
{
    NodeKind kind;
    alignas(16) std::byte data[128];
};

struct AST
{   
    std::vector<Node> nodes_;

    NodeRef root() const noexcept;

    template <typename T, typename... Args>
        requires std::is_constructible_v<T, Args...>
    [[nodiscard]] auto emplace(Args&&... args) noexcept
    {
        nodes_.emplace_back(std::in_place_type<T>, std::forward<Args>(args)...);
        return NodeRef{ nodes_.size() - 1 };
    }
};




/*
struct SymbolTable
{
    int x;
};

struct CompilationUnit
{
    AST ast_;
    SymbolTable symbol_table_;
};
*/