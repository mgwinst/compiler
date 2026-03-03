#pragma once

#include <cstdint>
#include <vector>
#include <optional>
#include <utility>
#include <vector>
#include <new>

#include "../utils/macros.hpp"
#include "../utils/concepts.hpp"
#include "../utils/alias.hpp"
#include "../utils/enums.hpp"


// nodes should contain std::span<const char> source_span_ for errors
// nodes should have TypeExpr node instead of string to store type info?

// ************** DECLARATIONS **************

namespace Syntax
{
    struct CompilationUnitDecl
    {
        std::string name_;
        std::vector<ASTNodeRef> decls_;

        CompilationUnitDecl(StringLike auto&& name) noexcept :
            name_{ std::forward<decltype(name)>(name) },
            decls_{} {}
    };

    struct VarDecl
    {
        std::string name_;
        ASTNodeRef type_expr_;
        std::optional<ASTNodeRef> init_; // single expr or init_list_expr

        VarDecl(StringLike auto&& name, ASTNodeRef type_expr, std::optional<ASTNodeRef> init = std::nullopt) noexcept :
            name_{ std::forward<decltype(name)>(name) }, 
            type_expr_{ type_expr }, 
            init_{ std::move(init) } {}
    };

    // init value (default value) ?
    struct ParamDecl
    {
        std::string name_;
        ASTNodeRef type_expr_;

        ParamDecl(StringLike auto&& name, ASTNodeRef type_expr) noexcept :
            name_{ std::forward<decltype(name)>(name) },
            type_expr_{ type_expr } {}
    };

    struct FuncDecl
    {
        std::string name_;
        std::vector<ASTNodeRef> params_;
        ASTNodeRef return_type_;
        ASTNodeRef body_;

        FuncDecl(StringLike auto&& name, Contiguous auto&& params, ASTNodeRef return_type, ASTNodeRef body) noexcept :
            name_{ std::forward<decltype(name)>(name) },
            params_{ std::forward<decltype(params)>(params) },
            return_type_{ return_type },
            body_{ body } {}
    };

    struct RecordDecl
    {
        RecordKind kind_;
        std::string name_;
        std::vector<ASTNodeRef> fields_;

        RecordDecl(RecordKind kind, StringLike auto&& name, Contiguous auto&& fields) noexcept :
            kind_{ kind },
            name_{ std::forward<decltype(name)>(name) },
            fields_{ std::forward<decltype(fields)>(fields) } {}
    };

    // ************** EXPRESSIONS **************

    struct CompoundStmt
    {
        std::vector<ASTNodeRef> children_; // both expr/decls
        std::optional<ASTNodeRef> return_stmt_;

        CompoundStmt(Contiguous auto&& children, std::optional<ASTNodeRef> return_stmt = std::nullopt) noexcept :
            children_{ std::forward<decltype(children)>(children) },
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
        bool is_postfix_;

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

        ReferenceExpr(StringLike auto&& name) noexcept : 
            name_{ std::forward<decltype(name)>(name) } {}
    };

    struct CallExpr
    {
        ASTNodeRef callee_; // callee is a reference expression ^
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

    // ************** TYPE EXPRESSIONS **************

    struct QualifierTypeExpr
    {
        QualifierKind kind_;
        ASTNodeRef inner_;

        QualifierTypeExpr(QualifierKind kind, ASTNodeRef inner) :
            kind_{ kind }, inner_{ inner } {}
    };
    
    struct PointerTypeExpr
    {
        ASTNodeRef inner_;
    };

    struct ReferenceTypeExpr
    {
        ASTNodeRef inner_;
    };

    struct ArrayTypeExpr
    {
        ASTNodeRef inner_;
        std::optional<ASTNodeRef> size_;
    };

    struct NamedTypeExpr
    {
        std::string name_;

        NamedTypeExpr(StringLike auto&& name) :
            name_{ std::forward<decltype(name)>(name) } {}
    };

} // namespace Syntax

template <typename T>
inline constexpr ASTNodeKind ast_node_kind_v = ASTNodeKind::Invalid;

template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::CompilationUnitDecl> = ASTNodeKind::CompilationUnitDecl;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::VarDecl>             = ASTNodeKind::VarDecl;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::ParamDecl>           = ASTNodeKind::ParamDecl;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::FuncDecl>            = ASTNodeKind::FuncDecl;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::RecordDecl>          = ASTNodeKind::RecordDecl;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::CompoundStmt>        = ASTNodeKind::CompoundStmt;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::ReturnStmt>          = ASTNodeKind::ReturnStmt;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::IfStmt>              = ASTNodeKind::IfStmt;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::WhileStmt>           = ASTNodeKind::WhileStmt;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::ForStmt>             = ASTNodeKind::ForStmt;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::IntegerLiteralExpr>  = ASTNodeKind::IntegerLiteralExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::FloatLiteralExpr>    = ASTNodeKind::FloatLiteralExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::CharLiteralExpr>     = ASTNodeKind::CharLiteralExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::StringLiteralExpr>   = ASTNodeKind::StringLiteralExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::BooleanLiteralExpr>  = ASTNodeKind::BooleanLiteralExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::UnaryExpr>           = ASTNodeKind::UnaryExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::BinaryExpr>          = ASTNodeKind::BinaryExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::ReferenceExpr>       = ASTNodeKind::ReferenceExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::CallExpr>            = ASTNodeKind::CallExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::MemberExpr>          = ASTNodeKind::MemberExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::ArraySubscriptExpr>  = ASTNodeKind::ArraySubscriptExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::InitListExpr>        = ASTNodeKind::InitListExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::ExplicitCastExpr>    = ASTNodeKind::ExplicitCastExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::ImplicitCastExpr>    = ASTNodeKind::ImplicitCastExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::QualifierTypeExpr>   = ASTNodeKind::QualifierTypeExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::PointerTypeExpr>     = ASTNodeKind::PointerTypeExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::ReferenceTypeExpr>   = ASTNodeKind::ReferenceTypeExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::ArrayTypeExpr>       = ASTNodeKind::ArrayTypeExpr;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::NamedTypeExpr>       = ASTNodeKind::NamedTypeExpr;

class ASTNode
{
public:
    template <typename T>
        requires (ast_node_kind_v<T> != ASTNodeKind::Invalid)
    ASTNode(std::in_place_type_t<T>, auto&&... args) : kind_{ ast_node_kind_v<T> }
    {
        ::new (data_) T{ std::forward<decltype(args)>(args)... };
    }

    ~ASTNode()
    {
        switch (kind_) {
            case ASTNodeKind::CompilationUnitDecl:  destroy<Syntax::CompilationUnitDecl>();  break;
            case ASTNodeKind::VarDecl:              destroy<Syntax::VarDecl>();              break;
            case ASTNodeKind::ParamDecl:            destroy<Syntax::ParamDecl>();            break;
            case ASTNodeKind::FuncDecl:             destroy<Syntax::FuncDecl>();             break;
            case ASTNodeKind::RecordDecl:           destroy<Syntax::RecordDecl>();           break;
            case ASTNodeKind::CompoundStmt:         destroy<Syntax::CompoundStmt>();         break;
            case ASTNodeKind::ReturnStmt:           destroy<Syntax::ReturnStmt>();           break;
            case ASTNodeKind::IfStmt:               destroy<Syntax::IfStmt>();               break;
            case ASTNodeKind::WhileStmt:            destroy<Syntax::WhileStmt>();            break;
            case ASTNodeKind::ForStmt:              destroy<Syntax::ForStmt>();              break;
            case ASTNodeKind::IntegerLiteralExpr:   destroy<Syntax::IntegerLiteralExpr>();   break;
            case ASTNodeKind::FloatLiteralExpr:     destroy<Syntax::FloatLiteralExpr>();     break;
            case ASTNodeKind::CharLiteralExpr:      destroy<Syntax::CharLiteralExpr>();      break;
            case ASTNodeKind::StringLiteralExpr:    destroy<Syntax::StringLiteralExpr>();    break;
            case ASTNodeKind::BooleanLiteralExpr:   destroy<Syntax::BooleanLiteralExpr>();   break;
            case ASTNodeKind::UnaryExpr:            destroy<Syntax::UnaryExpr>();            break;
            case ASTNodeKind::BinaryExpr:           destroy<Syntax::BinaryExpr>();           break;
            case ASTNodeKind::ReferenceExpr:        destroy<Syntax::ReferenceExpr>();        break;
            case ASTNodeKind::CallExpr:             destroy<Syntax::CallExpr>();             break;
            case ASTNodeKind::MemberExpr:           destroy<Syntax::MemberExpr>();           break;
            case ASTNodeKind::ArraySubscriptExpr:   destroy<Syntax::ArraySubscriptExpr>();   break;
            case ASTNodeKind::InitListExpr:         destroy<Syntax::InitListExpr>();         break;
            case ASTNodeKind::ExplicitCastExpr:     destroy<Syntax::ExplicitCastExpr>();     break;
            case ASTNodeKind::ImplicitCastExpr:     destroy<Syntax::ImplicitCastExpr>();     break;
            case ASTNodeKind::QualifierTypeExpr:    destroy<Syntax::QualifierTypeExpr>();    break;
            case ASTNodeKind::PointerTypeExpr:      destroy<Syntax::PointerTypeExpr>();      break;
            case ASTNodeKind::ReferenceTypeExpr:    destroy<Syntax::ReferenceTypeExpr>();    break;
            case ASTNodeKind::ArrayTypeExpr:        destroy<Syntax::ArrayTypeExpr>();        break;
            case ASTNodeKind::NamedTypeExpr:        destroy<Syntax::NamedTypeExpr>();        break;
            case ASTNodeKind::Invalid: [[fallthrough]];
            default: break;
        }
    }

    ASTNodeKind get_kind() const noexcept
    {
        return kind_;
    }

    template <typename T>
    [[nodiscard]] const T& as() const
    {
        if (kind_ != ast_node_kind_v<T>)
            error_exit("types don't match");

        return *std::launder(reinterpret_cast<const T*>(data_));
    }

    template <typename T>
    [[nodiscard]] T& as()
    {
        if (kind_ != ast_node_kind_v<T>)
            error_exit("types don't match");

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