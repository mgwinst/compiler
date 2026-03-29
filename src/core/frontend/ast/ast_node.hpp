#pragma once

#include <cstdint>
#include <vector>
#include <optional>
#include <utility>
#include <vector>
#include <new>

#include "../error/source_location.hpp"
#include "../../utils/macros.hpp"
#include "../../utils/alias.hpp"
#include "../../utils/enums.hpp"

// ************** DECLARATIONS **************

namespace Syntax
{
    struct CompilationUnitDecl
    {
        std::string name_;
        std::vector<ASTNodeId> decls_;

        CompilationUnitDecl(std::string name) noexcept :
            name_{ std::move(name) },
            decls_{} {}
    };

    struct VarDecl
    {
        std::string name_;
        ASTNodeId type_expr_;
        std::optional<ASTNodeId> init_{ std::nullopt }; // single expr or init_list_expr
        SourceLoc source_loc_;

        VarDecl(std::string name, ASTNodeId type_expr, std::optional<ASTNodeId> init, SourceLoc source_loc) noexcept :
            name_{ std::move(name) }, 
            type_expr_{ type_expr }, 
            init_{ std::move(init) },
            source_loc_{ std::move(source_loc) } {}
    };

    // init value (default value) ?
    struct ParamDecl
    {
        std::string name_;
        ASTNodeId type_expr_;
        SourceLoc source_loc_;

        ParamDecl(std::string name, ASTNodeId type_expr, SourceLoc source_loc) noexcept :
            name_{ std::move(name) },
            type_expr_{ type_expr },
            source_loc_{ std::move(source_loc) } {}
    };

    struct FuncDecl
    {
        std::string name_;
        std::vector<ASTNodeId> params_;
        ASTNodeId return_type_;
        ASTNodeId body_;
        SourceLoc source_loc_;

        FuncDecl(std::string name, std::vector<ASTNodeId> params, ASTNodeId return_type, ASTNodeId body, SourceLoc source_loc) noexcept :
            name_{ std::move(name) },
            params_{ std::move(params) },
            return_type_{ return_type },
            body_{ body },
            source_loc_{ std::move(source_loc) } {}
    };

    struct RecordDecl
    {
        RecordKind kind_;
        std::string name_;
        std::vector<ASTNodeId> fields_;
        SourceLoc source_loc_;

        RecordDecl(RecordKind kind, std::string name, std::vector<ASTNodeId> fields, SourceLoc source_loc) noexcept :
            kind_{ kind },
            name_{ std::move(name) },
            fields_{ std::move(fields) },
            source_loc_{ std::move(source_loc) } {}
    };

    // ************** EXPRESSIONS **************

    struct CompoundStmt
    {
        std::vector<ASTNodeId> children_; // exprs/decls/returns

        CompoundStmt(std::vector<ASTNodeId> children) noexcept :
            children_{ std::move(children) } {}
    };

    struct ReturnStmt
    {
        ASTNodeId value_;

        ReturnStmt(ASTNodeId value) noexcept :
            value_{ value } {}
    };

    struct BreakStmt
    {

    };

    struct ContinueStmt
    {

    };

    // nest IfStmt nodes for elif chains (no need for dedicated else-if nodes)
    struct IfStmt {
        ASTNodeId cond_;
        ASTNodeId then_stmt_; 
        std::optional<ASTNodeId> else_stmt_ = std::nullopt;

        IfStmt(ASTNodeId cond, ASTNodeId then_stmt, std::optional<ASTNodeId> else_stmt) :
            cond_{ cond },
            then_stmt_( then_stmt ),
            else_stmt_( else_stmt ) {}
    };

    struct WhileStmt
    {
        ASTNodeId cond_;
        ASTNodeId body_;

        WhileStmt(ASTNodeId cond, ASTNodeId body) :
            cond_{ cond },
            body_{ body } {}
    };

    struct ForStmt
    {
        ASTNodeId init_, cond_, update_;
        ASTNodeId body_;

        ForStmt(ASTNodeId init, ASTNodeId cond, ASTNodeId update, ASTNodeId body) noexcept :
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

        StringLiteralExpr(std::string value) :
            value_{ std::move(value) } {}
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
        ASTNodeId operand_;
        bool is_postfix_ = false;
        SourceLoc source_loc_;

        UnaryExpr(std::string op, ASTNodeId operand, bool is_postfix, SourceLoc source_loc) noexcept :
            op_{ std::move(op) },
            operand_{ operand },
            is_postfix_{ is_postfix },
            source_loc_{ source_loc } {}
    };

    struct BinaryExpr
    {
        std::string op_;
        ASTNodeId left_, right_;
        SourceLoc source_loc_;

        BinaryExpr(std::string op, ASTNodeId left, ASTNodeId right, SourceLoc source_loc) noexcept :
            op_{ std::move(op) },
            left_{ left },
            right_{ right },
            source_loc_{ source_loc } {}
    };

    struct ReferenceExpr
    {
        std::string name_;
        SourceLoc source_loc_;

        ReferenceExpr(std::string name, SourceLoc source_loc) noexcept : 
            name_{ std::move(name) },
            source_loc_{ source_loc } {}
    };

    struct CallExpr
    {
        ASTNodeId callee_; // callee is a reference expression ^
        std::vector<ASTNodeId> args_;
        SourceLoc source_loc_;

        CallExpr(ASTNodeId callee, std::vector<ASTNodeId> args, SourceLoc source_loc) noexcept :
            callee_{ callee },
            args_{ std::move(args) },
            source_loc_{ source_loc } {}
    };

    struct MemberExpr
    {
        ASTNodeId base_;
        std::string member_;
        bool is_arrow_ = false;
        SourceLoc source_loc_;
        
        MemberExpr(ASTNodeId base, std::string member, bool is_arrow, SourceLoc source_loc) :
            base_{ base },
            member_{ std::move(member) },
            is_arrow_{ is_arrow },
            source_loc_{ source_loc } {}
    };

    struct ArraySubscriptExpr
    {
        ASTNodeId base_;
        ASTNodeId index_;
        SourceLoc source_loc_;

        ArraySubscriptExpr(ASTNodeId base, ASTNodeId index, SourceLoc source_loc) noexcept :
            base_{ base },
            index_{ index },
            source_loc_( source_loc ) {}
    };

    struct InitListExpr
    {
        std::vector<ASTNodeId> init_values_;
        SourceLoc source_loc_;
        
        InitListExpr(std::vector<ASTNodeId> init_values, SourceLoc source_loc) :
            init_values_{ std::move(init_values) },
            source_loc_{ source_loc } {}
    };

    struct ExplicitCastExpr
    {

    };

    // ************** TYPE EXPRESSIONS **************

    struct QualifierTypeExpr
    {
        QualifierKind kind_;
        ASTNodeId inner_;

        QualifierTypeExpr(QualifierKind kind, ASTNodeId inner) :
            kind_{ kind }, inner_{ inner } {}
    };
    
    struct PointerTypeExpr
    {
        ASTNodeId inner_;
    };

    struct ReferenceTypeExpr
    {
        ASTNodeId inner_;
    };

    struct ArrayTypeExpr
    {
        ASTNodeId inner_;
        std::optional<ASTNodeId> size_;
    };

    struct NamedTypeExpr
    {
        std::string name_;

        NamedTypeExpr(std::string name) :
            name_{ std::move(name) } {}
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
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::BreakStmt>           = ASTNodeKind::BreakStmt;
template <> inline constexpr ASTNodeKind ast_node_kind_v<Syntax::ContinueStmt>        = ASTNodeKind::ContinueStmt;
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

    ASTNode(ASTNode&& other) noexcept : kind_{ other.kind_ }
    {
        move_construct_from(other);
        other.kind_ = ASTNodeKind::Invalid;
    }

    ASTNode& operator=(ASTNode&& other)
    {
        this->~ASTNode();
        kind_ = other.kind_;
        move_construct_from(other);
        other.kind_ = ASTNodeKind::Invalid;
        
        return *this;
    }

    ASTNode(ASTNode& other) = delete;
    ASTNode& operator=(ASTNode& other) = delete;

    ~ASTNode()
    {
        destroy_active();
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

    void move_construct_from(ASTNode& other)
    {
        switch (other.kind_) {
            case ASTNodeKind::CompilationUnitDecl: move_construct<Syntax::CompilationUnitDecl>(other); break;
            case ASTNodeKind::VarDecl:             move_construct<Syntax::VarDecl>(other);             break;
            case ASTNodeKind::ParamDecl:           move_construct<Syntax::ParamDecl>(other);           break;
            case ASTNodeKind::FuncDecl:            move_construct<Syntax::FuncDecl>(other);            break;
            case ASTNodeKind::RecordDecl:          move_construct<Syntax::RecordDecl>(other);          break;
            case ASTNodeKind::CompoundStmt:        move_construct<Syntax::CompoundStmt>(other);        break;
            case ASTNodeKind::ReturnStmt:          move_construct<Syntax::ReturnStmt>(other);          break;
            case ASTNodeKind::BreakStmt:           move_construct<Syntax::BreakStmt>(other);           break;
            case ASTNodeKind::ContinueStmt:        move_construct<Syntax::ContinueStmt>(other);        break;
            case ASTNodeKind::IfStmt:              move_construct<Syntax::IfStmt>(other);              break;
            case ASTNodeKind::WhileStmt:           move_construct<Syntax::WhileStmt>(other);           break;
            case ASTNodeKind::ForStmt:             move_construct<Syntax::ForStmt>(other);             break;
            case ASTNodeKind::IntegerLiteralExpr:  move_construct<Syntax::IntegerLiteralExpr>(other);  break;
            case ASTNodeKind::FloatLiteralExpr:    move_construct<Syntax::FloatLiteralExpr>(other);    break;
            case ASTNodeKind::CharLiteralExpr:     move_construct<Syntax::CharLiteralExpr>(other);     break;
            case ASTNodeKind::StringLiteralExpr:   move_construct<Syntax::StringLiteralExpr>(other);   break;
            case ASTNodeKind::BooleanLiteralExpr:  move_construct<Syntax::BooleanLiteralExpr>(other);  break;
            case ASTNodeKind::UnaryExpr:           move_construct<Syntax::UnaryExpr>(other);           break;
            case ASTNodeKind::BinaryExpr:          move_construct<Syntax::BinaryExpr>(other);          break;
            case ASTNodeKind::ReferenceExpr:       move_construct<Syntax::ReferenceExpr>(other);       break;
            case ASTNodeKind::CallExpr:            move_construct<Syntax::CallExpr>(other);            break;
            case ASTNodeKind::MemberExpr:          move_construct<Syntax::MemberExpr>(other);          break;
            case ASTNodeKind::ArraySubscriptExpr:  move_construct<Syntax::ArraySubscriptExpr>(other);  break;
            case ASTNodeKind::InitListExpr:        move_construct<Syntax::InitListExpr>(other);        break;
            case ASTNodeKind::ExplicitCastExpr:    move_construct<Syntax::ExplicitCastExpr>(other);    break;
            case ASTNodeKind::QualifierTypeExpr:   move_construct<Syntax::QualifierTypeExpr>(other);   break;
            case ASTNodeKind::PointerTypeExpr:     move_construct<Syntax::PointerTypeExpr>(other);     break;
            case ASTNodeKind::ReferenceTypeExpr:   move_construct<Syntax::ReferenceTypeExpr>(other);   break;
            case ASTNodeKind::ArrayTypeExpr:       move_construct<Syntax::ArrayTypeExpr>(other);       break;
            case ASTNodeKind::NamedTypeExpr:       move_construct<Syntax::NamedTypeExpr>(other);       break;
            case ASTNodeKind::Invalid:
            default:
                break;
        }
    }

    template <typename T>
    void move_construct(ASTNode& other) noexcept
    {
        new (data_) T{ std::move(other.as<T>()) };
    }

    void destroy_active() noexcept
    {
        switch (kind_) {
            case ASTNodeKind::CompilationUnitDecl:  destroy<Syntax::CompilationUnitDecl>();  break;
            case ASTNodeKind::VarDecl:              destroy<Syntax::VarDecl>();              break;
            case ASTNodeKind::ParamDecl:            destroy<Syntax::ParamDecl>();            break;
            case ASTNodeKind::FuncDecl:             destroy<Syntax::FuncDecl>();             break;
            case ASTNodeKind::RecordDecl:           destroy<Syntax::RecordDecl>();           break;
            case ASTNodeKind::CompoundStmt:         destroy<Syntax::CompoundStmt>();         break;
            case ASTNodeKind::ReturnStmt:           destroy<Syntax::ReturnStmt>();           break;
            case ASTNodeKind::BreakStmt:            destroy<Syntax::BreakStmt>();            break;
            case ASTNodeKind::ContinueStmt:         destroy<Syntax::ContinueStmt>();         break;
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
            case ASTNodeKind::QualifierTypeExpr:    destroy<Syntax::QualifierTypeExpr>();    break;
            case ASTNodeKind::PointerTypeExpr:      destroy<Syntax::PointerTypeExpr>();      break;
            case ASTNodeKind::ReferenceTypeExpr:    destroy<Syntax::ReferenceTypeExpr>();    break;
            case ASTNodeKind::ArrayTypeExpr:        destroy<Syntax::ArrayTypeExpr>();        break;
            case ASTNodeKind::NamedTypeExpr:        destroy<Syntax::NamedTypeExpr>();        break;
            case ASTNodeKind::Invalid: [[fallthrough]];
            default: break;
        }
    }

    template <typename T>
    void destroy()
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
            std::launder(reinterpret_cast<T*>(data_))->~T();
    }
};