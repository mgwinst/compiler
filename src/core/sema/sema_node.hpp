#pragma once

#include "symbol.hpp"
#include "../error/source_location.hpp"
#include "../utils/macros.hpp"
#include "../utils/enums.hpp"

namespace Sema
{
    // ************** DECLARATIONS **************

    struct CompilationUnitDecl
    {
        std::string name_;
        std::vector<SemaNodeRef> decls_;
        SymbolRef symbol_;
        TypeRef type_;

        CompilationUnitDecl(std::string name) noexcept :
            name_{ std::move(name) } {}
    };

    struct VarDecl
    {
        SymbolRef symbol_;
        TypeRef type_;
        std::optional<SemaNodeRef> init_; // single expr or init_list
        SourceLoc source_loc_;
    };

    struct ParamDecl
    {
        SymbolRef symbol_;
        TypeRef type_;
        //std::optional<SemaNodeRef> init_; // single expr or init_list
        SourceLoc source_loc_;

        ParamDecl(SymbolRef symbol, TypeRef type, SourceLoc source_loc) noexcept :
            symbol_{ symbol },
            type_{ type },
            source_loc_{ source_loc } {}
    };

    struct FuncDecl
    {
        SymbolRef symbol_;
        TypeRef type_;
        std::vector<SemaNodeRef> params_;
        SemaNodeRef body_;
        SourceLoc source_loc_;

        FuncDecl(SymbolRef symbol, TypeRef type, std::vector<SemaNodeRef> params, SemaNodeRef body, SourceLoc source_loc) noexcept :
            symbol_{ symbol },
            type_{ type },
            params_{ std::move(params) },
            body_{ body },
            source_loc_{ source_loc } {}
    };

    struct RecordDecl 
    {
        SymbolRef symbol_;
        TypeRef type_;
        std::vector<SemaNodeRef> fields_;
        SourceLoc source_loc_;
        
        RecordDecl(SymbolRef symbol, TypeRef type, std::vector<SemaNodeRef> fields, SourceLoc source_loc) :
            symbol_{ symbol }, 
            type_{ type }, 
            fields_{ std::move(fields) },
            source_loc_{ source_loc } {}
    };

    // ************** EXPRESSIONS **************

    struct CompoundStmt
    {
        std::vector<SemaNodeRef> children_; // both expr/decls
        std::optional<SemaNodeRef> return_stmt_;

        CompoundStmt(std::vector<SemaNodeRef> children, std::optional<SemaNodeRef> return_stmt = std::nullopt) noexcept :
            children_{ std::move(children) },
            return_stmt_{ return_stmt } {}
    };

    struct ReturnStmt
    {
        SemaNodeRef value_;

        ReturnStmt(SemaNodeRef value) noexcept :
            value_{ value } {}
    };

    // nest IfStmt nodes for elif chains (no need for dedicated else-if nodes)
    struct IfStmt {
        SemaNodeRef cond_;
        SemaNodeRef then_stmt_; 
        std::optional<SemaNodeRef> else_stmt_;
    };

    struct WhileStmt
    {
        SemaNodeRef cond_;
        SemaNodeRef body_;

        WhileStmt(SemaNodeRef cond, SemaNodeRef body) noexcept :
            cond_{ cond },
            body_{ body } {}
    };

    struct ForStmt
    {
        SemaNodeRef init_, cond_, update_;
        SemaNodeRef body_;

        ForStmt(SemaNodeRef init, SemaNodeRef cond, SemaNodeRef update, SemaNodeRef body) noexcept :
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
        SemaNodeRef operand_;
        bool is_postfix_;

        UnaryExpr(std::string op, SemaNodeRef operand, bool is_postfix = false) noexcept :
            op_{ std::move(op) },
            operand_{ operand },
            is_postfix_{ is_postfix } {}
    };

    struct BinaryExpr
    {
        std::string op_;
        SemaNodeRef left_, right_;

        BinaryExpr(std::string op, SemaNodeRef left, SemaNodeRef right) noexcept :
            op_{ std::move(op) },
            left_{ left },
            right_{ right } {}
    };

    // invalid state to continue sema passes?
    struct ReferenceExpr
    {
        SymbolRef target_symbol_;
        TypeRef target_type_;
        std::string name_;
        SourceLoc source_loc_;
        bool is_valid_;

        ReferenceExpr(SymbolRef target_symbol, TypeRef target_type, std::string name, SourceLoc source_loc) noexcept : 
            target_symbol_{ target_symbol },
            target_type_{ target_type },
            name_{ std::move(name) },
            source_loc_{ source_loc } {}
    };

    struct CallExpr
    {
        SemaNodeRef callee_; // reference expr
        std::vector<SemaNodeRef> args_;

        CallExpr(SemaNodeRef callee, std::vector<SemaNodeRef> args) noexcept :
            callee_{ callee },
            args_{ std::move(args) } {}
    };

    struct MemberExpr
    {
        SemaNodeRef base_;
        std::string member_;
        bool is_arrow_ = false;
        
        MemberExpr(SemaNodeRef base, std::string member, bool is_arrow = false) :
            base_{ base },
            member_{ std::move(member) },
            is_arrow_{ is_arrow } {}
    };

    struct ArraySubscriptExpr
    {
        SemaNodeRef base_;
        SemaNodeRef index_;

        ArraySubscriptExpr(SemaNodeRef base, SemaNodeRef index) noexcept :
            base_{ base },
            index_{ index } {}
    };

    struct InitListExpr
    {
        std::vector<SemaNodeRef> init_values_;
        
        InitListExpr(std::vector<SemaNodeRef> init_values) :
            init_values_{ std::move(init_values) } {}
    };

    struct ExplicitCastExpr
    {

    };

    struct ImplicitCastExpr
    {

    };

} // namespace Sema

template <typename T>
inline constexpr SemaNodeKind sema_node_kind_v = SemaNodeKind::Invalid;

template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::CompilationUnitDecl>   = SemaNodeKind::CompilationUnitDecl;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::VarDecl>               = SemaNodeKind::VarDecl;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::ParamDecl>             = SemaNodeKind::ParamDecl;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::FuncDecl>              = SemaNodeKind::FuncDecl;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::RecordDecl>            = SemaNodeKind::RecordDecl;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::CompoundStmt>          = SemaNodeKind::CompoundStmt;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::ReturnStmt>            = SemaNodeKind::ReturnStmt;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::IfStmt>                = SemaNodeKind::IfStmt;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::WhileStmt>             = SemaNodeKind::WhileStmt;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::ForStmt>               = SemaNodeKind::ForStmt;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::IntegerLiteralExpr>    = SemaNodeKind::IntegerLiteralExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::FloatLiteralExpr>      = SemaNodeKind::FloatLiteralExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::CharLiteralExpr>       = SemaNodeKind::CharLiteralExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::StringLiteralExpr>     = SemaNodeKind::StringLiteralExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::BooleanLiteralExpr>    = SemaNodeKind::BooleanLiteralExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::UnaryExpr>             = SemaNodeKind::UnaryExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::BinaryExpr>            = SemaNodeKind::BinaryExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::ReferenceExpr>         = SemaNodeKind::ReferenceExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::CallExpr>              = SemaNodeKind::CallExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::MemberExpr>            = SemaNodeKind::MemberExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::ArraySubscriptExpr>    = SemaNodeKind::ArraySubscriptExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::InitListExpr>          = SemaNodeKind::InitListExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::ExplicitCastExpr>      = SemaNodeKind::ExplicitCastExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::ImplicitCastExpr>      = SemaNodeKind::ImplicitCastExpr;

class SemaNode
{
public:
    template <typename T>
        requires (sema_node_kind_v<T> != SemaNodeKind::Invalid && sizeof(T) < 127)
    SemaNode(std::in_place_type_t<T>, auto&&... args) : kind_{ sema_node_kind_v<T> }
    {
        ::new (data_) T{ std::forward<decltype(args)>(args)... };
    }

    ~SemaNode()
    {
        switch (kind_) {
            case SemaNodeKind::CompilationUnitDecl:      destroy<Sema::CompilationUnitDecl>();  break;
            case SemaNodeKind::VarDecl:                  destroy<Sema::VarDecl>();              break;
            case SemaNodeKind::ParamDecl:                destroy<Sema::ParamDecl>();            break;
            case SemaNodeKind::FuncDecl:                 destroy<Sema::FuncDecl>();             break;
            case SemaNodeKind::RecordDecl:               destroy<Sema::RecordDecl>();           break;
            case SemaNodeKind::CompoundStmt:             destroy<Sema::CompoundStmt>();         break;
            case SemaNodeKind::ReturnStmt:               destroy<Sema::ReturnStmt>();           break;
            case SemaNodeKind::IfStmt:                   destroy<Sema::IfStmt>();               break;
            case SemaNodeKind::WhileStmt:                destroy<Sema::WhileStmt>();            break;
            case SemaNodeKind::ForStmt:                  destroy<Sema::ForStmt>();              break;
            case SemaNodeKind::IntegerLiteralExpr:       destroy<Sema::IntegerLiteralExpr>();   break;
            case SemaNodeKind::FloatLiteralExpr:         destroy<Sema::FloatLiteralExpr>();     break;
            case SemaNodeKind::CharLiteralExpr:          destroy<Sema::CharLiteralExpr>();      break;
            case SemaNodeKind::StringLiteralExpr:        destroy<Sema::StringLiteralExpr>();    break;
            case SemaNodeKind::BooleanLiteralExpr:       destroy<Sema::BooleanLiteralExpr>();   break;
            case SemaNodeKind::UnaryExpr:                destroy<Sema::UnaryExpr>();            break;
            case SemaNodeKind::BinaryExpr:               destroy<Sema::BinaryExpr>();           break;
            case SemaNodeKind::ReferenceExpr:            destroy<Sema::ReferenceExpr>();        break;
            case SemaNodeKind::CallExpr:                 destroy<Sema::CallExpr>();             break;
            case SemaNodeKind::MemberExpr:               destroy<Sema::MemberExpr>();           break;
            case SemaNodeKind::ArraySubscriptExpr:       destroy<Sema::ArraySubscriptExpr>();   break;
            case SemaNodeKind::InitListExpr:             destroy<Sema::InitListExpr>();         break;
            case SemaNodeKind::ExplicitCastExpr:         destroy<Sema::ExplicitCastExpr>();     break;
            case SemaNodeKind::ImplicitCastExpr:         destroy<Sema::ImplicitCastExpr>();     break;
            case SemaNodeKind::Invalid: [[fallthrough]];
            default: break;
        }
    }

    SemaNodeKind get_kind() const noexcept
    {
        return kind_;
    }

    template <typename T>
    [[nodiscard]] const T& as() const
    {
        if (kind_ != sema_node_kind_v<T>)
            error_exit("types don't match");

        return *std::launder(reinterpret_cast<const T*>(data_));

    }

    template <typename T>
    [[nodiscard]] T& as()
    {
        if (kind_ != sema_node_kind_v<T>)
            error_exit("types don't match");

        return *std::launder(reinterpret_cast<T*>(data_));
    }

private:
    alignas(64) std::byte data_[127];
    SemaNodeKind kind_;

    template <typename T>
    void destroy()
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
            std::launder(reinterpret_cast<T*>(data_))->~T();
    }
};
