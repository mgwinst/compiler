#pragma once

#include <cstdint>
#include <vector>
#include <optional>
#include <utility>
#include <vector>
#include <new>

#include "../utils/concepts.hpp"

using ASTNodeRef = std::size_t;

// ************** DECLARATIONS **************

namespace SyntaxTree
{
    struct CompilationUnitDecl
    {
        std::string name_;
        std::vector<ASTNodeRef> decls_;

        CompilationUnitDecl(StringLike auto&& name) noexcept :
            name_{ std::forward<decltype(name)>(name) },
            decls_{} {}
    };

    // AST nodes should keep std::span into source for errors / debugging

    struct VarDecl
    {
        bool is_const_;
        bool is_array_;
        std::string type_;
        std::string name_;
        std::optional<ASTNodeRef> init_; // single expr or init_list

        VarDecl(bool is_const, bool is_array, StringLike auto&& type, StringLike auto&& name, std::optional<ASTNodeRef> init = std::nullopt) noexcept :
            is_const_{ is_const },
            is_array_{ is_array },
            type_{ std::forward<decltype(type)>(type) }, 
            name_{ std::forward<decltype(name)>(name) }, 
            init_{ std::move(init) } {}
    };

    struct ParamDecl
    {
        bool is_const_;
        std::string type_;
        std::string name_;

        ParamDecl(bool is_const, StringLike auto&& type, StringLike auto&& name) noexcept :
            is_const_{ is_const },
            type_{ std::forward<decltype(type)>(type) },
            name_{ std::forward<decltype(name)>(name) } {}
    };

    struct FuncDecl
    {
        std::string name_;
        std::vector<ASTNodeRef> params_;
        std::string return_type_;
        ASTNodeRef body_;

        FuncDecl(StringLike auto&& name, Contiguous auto&& params, StringLike auto&& return_type, ASTNodeRef body) noexcept :
            name_{ std::forward<decltype(name)>(name) },
            params_{ std::forward<decltype(params)>(params) },
            return_type_{ std::forward<decltype(return_type)>(return_type) },
            body_{ body } {}
    };

    struct StructDecl
    {
        std::string type_;
        std::vector<ASTNodeRef> fields_;

        StructDecl(StringLike auto&& type, Contiguous auto&& fields) noexcept :
            type_{ std::forward<decltype(type)>(type) },
            fields_{ std::forward<decltype(fields)>(fields) } {}
    };

    // ************** EXPRESSIONS **************

    struct CompoundStmt
    {
        std::vector<ASTNodeRef> decls_;
        std::vector<ASTNodeRef> exprs_;
        std::optional<ASTNodeRef> return_stmt_;

        CompoundStmt(Contiguous auto&& exprs, std::optional<ASTNodeRef> return_stmt = std::nullopt) noexcept :
            exprs_{ std::forward<decltype(exprs)>(exprs) },
            return_stmt_{ return_stmt } {}
    };

    struct ReturnStmt
    {
        ASTNodeRef value_;

        ReturnStmt(ASTNodeRef value) noexcept :
            value_{ value } {}
    };

    // nest IfStmt nodes for elif chains (no need for dedicated else-if nodes)
    struct IfStmt {
        ASTNodeRef cond_;
        ASTNodeRef then_stmt_; 
        std::optional<ASTNodeRef> else_stmt_;

        IfStmt(ASTNodeRef cond, ASTNodeRef then_stmt, std::optional<ASTNodeRef> else_stmt_ = std::nullopt) noexcept :
            cond_{ cond },
            then_stmt_{ then_stmt },
            else_stmt_{ else_stmt_ } {}
    };

    struct WhileStmt
    {
        ASTNodeRef cond_;
        ASTNodeRef body_;

        WhileStmt(ASTNodeRef cond, ASTNodeRef body) noexcept :
            cond_{ cond },
            body_{ body } {}
    };

    struct ForStmt
    {
        ASTNodeRef init_, cond_, update_;
        ASTNodeRef body_;

        ForStmt(ASTNodeRef init, ASTNodeRef cond, ASTNodeRef update, ASTNodeRef body) noexcept :
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
        ASTNodeRef operand_;
        bool is_postfix_ = false;

        UnaryExpr(StringLike auto&& op, ASTNodeRef operand, bool is_postfix = false) noexcept :
            op_{ std::forward<decltype(op)>(op) },
            operand_{ operand },
            is_postfix_{ is_postfix } {}
    };

    struct BinaryExpr
    {
        std::string op_;
        ASTNodeRef left_, right_;

        BinaryExpr(StringLike auto&& op, ASTNodeRef left, ASTNodeRef right) noexcept :
            op_{ std::forward<decltype(op)>(op) },
            left_{ left },
            right_{ right } {}
    };

    struct ReferenceExpr
    {
        std::string name_;
        // symbol to entity that it is referring to

        ReferenceExpr(StringLike auto&& name) noexcept : 
            name_{ std::forward<decltype(name)>(name) } {}
    };

    struct CallExpr
    {
        ASTNodeRef callee_; // either callee is a reference expression, and if so that would hold a symbol pointer or 
        std::vector<ASTNodeRef> args_;

        CallExpr(ASTNodeRef callee, Contiguous auto&& args) noexcept :
            callee_{ callee },
            args_{ std::forward<decltype(args)>(args) } {}
    };

    struct MemberExpr
    {
        ASTNodeRef base_;
        std::string member_;
        bool is_arrow_ = false;
        
        MemberExpr(ASTNodeRef base, StringLike auto&& member, bool is_arrow = false) :
            base_{ base },
            member_{ std::forward<decltype(member)>(member) },
            is_arrow_{ is_arrow } {}
    };

    struct ArraySubscriptExpr
    {
        ASTNodeRef base_;
        ASTNodeRef index_;

        ArraySubscriptExpr(ASTNodeRef base, ASTNodeRef index) noexcept :
            base_{ base },
            index_{ index } {}
    };

    struct InitListExpr
    {
        std::vector<ASTNodeRef> init_values_;
        
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

} // namespace Syntax

enum class ASTNodeKind : uint8_t 
{
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
inline constexpr ASTNodeKind ast_node_kind_v = ASTNodeKind::Invalid;

template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::CompilationUnitDecl>   = ASTNodeKind::CompilationUnitDecl;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::VarDecl>               = ASTNodeKind::VarDecl;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::ParamDecl>             = ASTNodeKind::ParamDecl;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::FuncDecl>              = ASTNodeKind::FuncDecl;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::StructDecl>            = ASTNodeKind::StructDecl;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::CompoundStmt>          = ASTNodeKind::CompoundStmt;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::ReturnStmt>            = ASTNodeKind::ReturnStmt;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::IfStmt>                = ASTNodeKind::IfStmt;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::WhileStmt>             = ASTNodeKind::WhileStmt;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::ForStmt>               = ASTNodeKind::ForStmt;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::IntegerLiteralExpr>    = ASTNodeKind::IntegerLiteralExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::FloatLiteralExpr>      = ASTNodeKind::FloatLiteralExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::CharLiteralExpr>       = ASTNodeKind::CharLiteralExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::StringLiteralExpr>     = ASTNodeKind::StringLiteralExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::BooleanLiteralExpr>    = ASTNodeKind::BooleanLiteralExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::UnaryExpr>             = ASTNodeKind::UnaryExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::BinaryExpr>            = ASTNodeKind::BinaryExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::ReferenceExpr>         = ASTNodeKind::ReferenceExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::CallExpr>              = ASTNodeKind::CallExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::MemberExpr>            = ASTNodeKind::MemberExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::ArraySubscriptExpr>    = ASTNodeKind::ArraySubscriptExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::InitListExpr>          = ASTNodeKind::InitListExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::ExplicitCastExpr>      = ASTNodeKind::ExplicitCastExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::ImplicitCastExpr>      = ASTNodeKind::ImplicitCastExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::NewExpr>               = ASTNodeKind::NewExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::DeleteExpr>            = ASTNodeKind::DeleteExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::ConstructExpr>         = ASTNodeKind::ConstructExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<SyntaxTree::DestructExpr>          = ASTNodeKind::DestructExpr;

class ASTNode
{
public:
    template <typename T>
        requires (ast_node_kind_v<T> != ASTNodeKind::Invalid && sizeof(T) < 127)
    ASTNode(std::in_place_type_t<T>, auto&&... args) : kind_{ ast_node_kind_v<T> }
    {
        ::new (data_) T{ std::forward<decltype(args)>(args)... };
    }

    ~ASTNode()
    {
        switch (kind_) {
            case ASTNodeKind::CompilationUnitDecl:  destroy<SyntaxTree::CompilationUnitDecl>();  break;
            case ASTNodeKind::VarDecl:              destroy<SyntaxTree::VarDecl>();              break;
            case ASTNodeKind::ParamDecl:            destroy<SyntaxTree::ParamDecl>();            break;
            case ASTNodeKind::FuncDecl:             destroy<SyntaxTree::FuncDecl>();             break;
            case ASTNodeKind::StructDecl:           destroy<SyntaxTree::StructDecl>();           break;
            case ASTNodeKind::CompoundStmt:         destroy<SyntaxTree::CompoundStmt>();         break;
            case ASTNodeKind::ReturnStmt:           destroy<SyntaxTree::ReturnStmt>();           break;
            case ASTNodeKind::IfStmt:               destroy<SyntaxTree::IfStmt>();               break;
            case ASTNodeKind::WhileStmt:            destroy<SyntaxTree::WhileStmt>();            break;
            case ASTNodeKind::ForStmt:              destroy<SyntaxTree::ForStmt>();              break;
            case ASTNodeKind::IntegerLiteralExpr:   destroy<SyntaxTree::IntegerLiteralExpr>();   break;
            case ASTNodeKind::FloatLiteralExpr:     destroy<SyntaxTree::FloatLiteralExpr>();     break;
            case ASTNodeKind::CharLiteralExpr:      destroy<SyntaxTree::CharLiteralExpr>();      break;
            case ASTNodeKind::StringLiteralExpr:    destroy<SyntaxTree::StringLiteralExpr>();    break;
            case ASTNodeKind::BooleanLiteralExpr:   destroy<SyntaxTree::BooleanLiteralExpr>();   break;
            case ASTNodeKind::UnaryExpr:            destroy<SyntaxTree::UnaryExpr>();            break;
            case ASTNodeKind::BinaryExpr:           destroy<SyntaxTree::BinaryExpr>();           break;
            case ASTNodeKind::ReferenceExpr:        destroy<SyntaxTree::ReferenceExpr>();        break;
            case ASTNodeKind::CallExpr:             destroy<SyntaxTree::CallExpr>();             break;
            case ASTNodeKind::MemberExpr:           destroy<SyntaxTree::MemberExpr>();           break;
            case ASTNodeKind::ArraySubscriptExpr:   destroy<SyntaxTree::ArraySubscriptExpr>();   break;
            case ASTNodeKind::InitListExpr:         destroy<SyntaxTree::InitListExpr>();         break;
            case ASTNodeKind::ExplicitCastExpr:     destroy<SyntaxTree::ExplicitCastExpr>();     break;
            case ASTNodeKind::ImplicitCastExpr:     destroy<SyntaxTree::ImplicitCastExpr>();     break;
            case ASTNodeKind::NewExpr:              destroy<SyntaxTree::NewExpr>();              break;
            case ASTNodeKind::DeleteExpr:           destroy<SyntaxTree::DeleteExpr>();           break;
            case ASTNodeKind::ConstructExpr:        destroy<SyntaxTree::ConstructExpr>();        break;
            case ASTNodeKind::DestructExpr:         destroy<SyntaxTree::DestructExpr>();         break;
            case ASTNodeKind::Invalid: [[fallthrough]];
            default: break;
        }
    }

    ASTNodeKind get_kind() const noexcept
    {
        return kind_;
    }

    // these should return T* so you can return optional reference
    template <typename T>
    [[nodiscard]] const T& as() const
    {
        if (kind_ == ast_node_kind_v<T>)
            return *std::launder(reinterpret_cast<const T*>(data_));
    }

    template <typename T>
    [[nodiscard]] T& as()
    {
        if (kind_ == ast_node_kind_v<T>)
            return *std::launder(reinterpret_cast<T*>(data_));
    }

private:
    alignas(64) std::byte data_[127];
    ASTNodeKind kind_;

    template <typename T>
    void destroy()
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
            std::launder(reinterpret_cast<T*>(data_))->~T();
    }
};