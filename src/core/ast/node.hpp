#pragma once

#include <vector>
#include <optional>
#include <utility>
#include <vector>
#include <new>

#include "../sema/types.hpp"
#include "../utils/utils.hpp"
#include "../utils/concepts.hpp"

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
    TypeRef type_;
    std::string name_;
    std::optional<NodeRef> init_; // single expr or init_list

    VarDecl(TypeRef type, StringLike auto&& name, std::optional<NodeRef> init = std::nullopt) noexcept :
        type_{ type }, 
        name_{ std::forward<decltype(name)>(name) }, 
        init_{ std::move(init) } {}
};

struct ParamDecl
{
    TypeRef type_;
    std::string name_;

    ParamDecl(TypeRef type, StringLike auto&& name) noexcept :
        type_{ type }, 
        name_{ std::forward<decltype(name)>(name) } {}
};

struct FuncDecl
{
    std::string name_;
    std::vector<NodeRef> params_;
    TypeRef return_type_;
    NodeRef body_;

    FuncDecl(StringLike auto&& name, Contiguous auto&& params, TypeRef return_type, NodeRef body) noexcept :
        name_{ std::forward<decltype(name)>(name) },
        params_{ std::forward<decltype(params)>(params) },
        return_type_{ return_type },
        body_{ body } {}
};

struct StructDecl
{
    TypeRef type_;
    std::vector<NodeRef> fields_;

    StructDecl(TypeRef type, Contiguous auto&& fields) noexcept :
        type_{ type },
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
    int64_t value_;

    IntegerLiteralExpr(int64_t value) noexcept :
        value_{ value } {}
};

struct FloatLiteralExpr
{
    long double value_;

    FloatLiteralExpr(long double value) noexcept :
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
    std::string member_;
    bool is_arrow_ = false;
    
    MemberExpr(NodeRef base, StringLike auto&& member, bool is_arrow = false) :
        base_{ base },
        member_{ std::forward<decltype(member)>(member) },
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
    DestructExpr,
    Invalid
};

template <typename T>
inline constexpr NodeKind node_kind_v = NodeKind::Invalid;

template <> inline constexpr NodeKind node_kind_v<CompilationUnitDecl>   = NodeKind::CompilationUnitDecl;
template <> inline constexpr NodeKind node_kind_v<VarDecl>               = NodeKind::VarDecl;
template <> inline constexpr NodeKind node_kind_v<ParamDecl>             = NodeKind::ParamDecl;
template <> inline constexpr NodeKind node_kind_v<FuncDecl>              = NodeKind::FuncDecl;
template <> inline constexpr NodeKind node_kind_v<StructDecl>            = NodeKind::StructDecl;
template <> inline constexpr NodeKind node_kind_v<CompoundStmt>          = NodeKind::CompoundStmt;
template <> inline constexpr NodeKind node_kind_v<ReturnStmt>            = NodeKind::ReturnStmt;
template <> inline constexpr NodeKind node_kind_v<IfStmt>                = NodeKind::IfStmt;
template <> inline constexpr NodeKind node_kind_v<WhileStmt>             = NodeKind::WhileStmt;
template <> inline constexpr NodeKind node_kind_v<ForStmt>               = NodeKind::ForStmt;
template <> inline constexpr NodeKind node_kind_v<IntegerLiteralExpr>    = NodeKind::IntegerLiteralExpr;
template <> inline constexpr NodeKind node_kind_v<FloatLiteralExpr>      = NodeKind::FloatLiteralExpr;
template <> inline constexpr NodeKind node_kind_v<CharLiteralExpr>       = NodeKind::CharLiteralExpr;
template <> inline constexpr NodeKind node_kind_v<StringLiteralExpr>     = NodeKind::StringLiteralExpr;
template <> inline constexpr NodeKind node_kind_v<BooleanLiteralExpr>    = NodeKind::BooleanLiteralExpr;
template <> inline constexpr NodeKind node_kind_v<UnaryExpr>             = NodeKind::UnaryExpr;
template <> inline constexpr NodeKind node_kind_v<BinaryExpr>            = NodeKind::BinaryExpr;
template <> inline constexpr NodeKind node_kind_v<ReferenceExpr>         = NodeKind::ReferenceExpr;
template <> inline constexpr NodeKind node_kind_v<CallExpr>              = NodeKind::CallExpr;
template <> inline constexpr NodeKind node_kind_v<MemberExpr>            = NodeKind::MemberExpr;
template <> inline constexpr NodeKind node_kind_v<ArraySubscriptExpr>    = NodeKind::ArraySubscriptExpr;
template <> inline constexpr NodeKind node_kind_v<InitListExpr>          = NodeKind::InitListExpr;
template <> inline constexpr NodeKind node_kind_v<ExplicitCastExpr>      = NodeKind::ExplicitCastExpr;
template <> inline constexpr NodeKind node_kind_v<ImplicitCastExpr>      = NodeKind::ImplicitCastExpr;
template <> inline constexpr NodeKind node_kind_v<NewExpr>               = NodeKind::NewExpr;
template <> inline constexpr NodeKind node_kind_v<DeleteExpr>            = NodeKind::DeleteExpr;
template <> inline constexpr NodeKind node_kind_v<ConstructExpr>         = NodeKind::ConstructExpr;
template <> inline constexpr NodeKind node_kind_v<DestructExpr>          = NodeKind::DestructExpr;

class Node
{
public:
    template <typename T>
        requires (node_kind_v<T> != NodeKind::Invalid)
    Node(std::in_place_type_t<T>, auto&&... args) : kind_{ node_kind_v<T> }
    {
        ::new (data_) T{ std::forward<decltype(args)>(args)... };
    }

    ~Node()
    {
        switch (kind_) {
            case NodeKind::CompilationUnitDecl:  destroy<CompilationUnitDecl>();  break;
            case NodeKind::VarDecl:              destroy<VarDecl>();              break;
            case NodeKind::ParamDecl:            destroy<ParamDecl>();            break;
            case NodeKind::FuncDecl:             destroy<FuncDecl>();             break;
            case NodeKind::StructDecl:           destroy<StructDecl>();           break;
            case NodeKind::CompoundStmt:         destroy<CompoundStmt>();         break;
            case NodeKind::ReturnStmt:           destroy<ReturnStmt>();           break;
            case NodeKind::IfStmt:               destroy<IfStmt>();               break;
            case NodeKind::WhileStmt:            destroy<WhileStmt>();            break;
            case NodeKind::ForStmt:              destroy<ForStmt>();              break;
            case NodeKind::IntegerLiteralExpr:   destroy<IntegerLiteralExpr>();   break;
            case NodeKind::FloatLiteralExpr:     destroy<FloatLiteralExpr>();     break;
            case NodeKind::CharLiteralExpr:      destroy<CharLiteralExpr>();      break;
            case NodeKind::StringLiteralExpr:    destroy<StringLiteralExpr>();    break;
            case NodeKind::BooleanLiteralExpr:   destroy<BooleanLiteralExpr>();   break;
            case NodeKind::UnaryExpr:            destroy<UnaryExpr>();            break;
            case NodeKind::BinaryExpr:           destroy<BinaryExpr>();           break;
            case NodeKind::ReferenceExpr:        destroy<ReferenceExpr>();        break;
            case NodeKind::CallExpr:             destroy<CallExpr>();             break;
            case NodeKind::MemberExpr:           destroy<MemberExpr>();           break;
            case NodeKind::ArraySubscriptExpr:   destroy<ArraySubscriptExpr>();   break;
            case NodeKind::InitListExpr:         destroy<InitListExpr>();         break;
            case NodeKind::ExplicitCastExpr:     destroy<ExplicitCastExpr>();     break;
            case NodeKind::ImplicitCastExpr:     destroy<ImplicitCastExpr>();     break;
            case NodeKind::NewExpr:              destroy<NewExpr>();              break;
            case NodeKind::DeleteExpr:           destroy<DeleteExpr>();           break;
            case NodeKind::ConstructExpr:        destroy<ConstructExpr>();        break;
            case NodeKind::DestructExpr:         destroy<DestructExpr>();         break;
            case NodeKind::Invalid: [[fallthrough]];
            default: break;
        }
    }

    NodeKind get_kind() const noexcept
    {
        return kind_;
    }

    template <typename T>
    [[nodiscard]] const T& as() const
    {
        if (kind_ == node_kind_v<T>)
            return *std::launder(reinterpret_cast<const T*>(data_));
        
    }

    template <typename T>
    [[nodiscard]] T& as()
    {
        if (kind_ == node_kind_v<T>)
            return *std::launder(reinterpret_cast<T*>(data_));
    }

private:
    NodeKind kind_;
    alignas(64) std::byte data_[128];

    template <typename T>
    void destroy()
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
            std::launder(reinterpret_cast<T*>(data_))->~T();
    }
};
