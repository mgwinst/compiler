#pragma once

#include "symbol.hpp"
#include "../utils/concepts.hpp"
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

        CompilationUnitDecl(StringLike auto&& name) noexcept :
            name_{ std::forward<decltype(name)>(name) } {}
    };

    struct VarDecl
    {
        SymbolRef symbol_;
        TypeRef type_;
        std::optional<SemaNodeRef> init_; // single expr or init_list
    };

    struct ParamDecl
    {
        SymbolRef symbol_;
        TypeRef type_;
        //std::optional<SemaNodeRef> init_; // single expr or init_list
    };

    struct FuncDecl
    {
        SymbolRef symbol_;
        TypeRef type_;
        std::vector<SemaNodeRef> params_;
        SemaNodeRef body_;

        FuncDecl(SymbolRef symbol, TypeRef type, Contiguous auto&& params, SemaNodeRef body) noexcept :
            symbol_{ symbol },
            type_{ type },
            params_{ std::forward<decltype(params)>(params) },
            body_{ body } {}
    };

    struct RecordDecl 
    {
        SymbolRef symbol_;
        TypeRef type_;
        std::vector<SemaNodeRef> fields_;
        
        RecordDecl(SymbolRef symbol, TypeRef type, Contiguous auto&& fields) :
            symbol_{ symbol }, 
            type_{ type }, 
            fields_{ std::forward<decltype(fields)>(fields) } {}
    };

    // ************** EXPRESSIONS **************

    struct CompoundStmt
    {
        std::vector<SemaNodeRef> children_; // both expr/decls
        std::optional<SemaNodeRef> return_stmt_;

        CompoundStmt(Contiguous auto&& children, std::optional<SemaNodeRef> return_stmt = std::nullopt) noexcept :
            children_{ std::forward<decltype(children)>(children) },
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
        SemaNodeRef operand_;
        bool is_postfix_;

        UnaryExpr(StringLike auto&& op, SemaNodeRef operand, bool is_postfix = false) noexcept :
            op_{ std::forward<decltype(op)>(op) },
            operand_{ operand },
            is_postfix_{ is_postfix } {}
    };

    struct BinaryExpr
    {
        std::string op_;
        SemaNodeRef left_, right_;

        BinaryExpr(StringLike auto&& op, SemaNodeRef left, SemaNodeRef right) noexcept :
            op_{ std::forward<decltype(op)>(op) },
            left_{ left },
            right_{ right } {}
    };

    struct ReferenceExpr
    {
        SymbolRef target_symbol_;
        TypeRef target_type_;
        std::string name_;

        ReferenceExpr(SymbolRef target_symbol, TypeRef target_type, StringLike auto&& name) noexcept : 
            target_symbol_{ std::forward<decltype(target_symbol)>(target_symbol) },
            target_type_{ std::forward<decltype(target_type)>(target_type) },
            name_{ std::forward<decltype(name)>(name) } {}
    };

    struct CallExpr
    {
        SemaNodeRef callee_; // either callee is a reference expression, and if so that would hold a symbol pointer or 
        std::vector<SemaNodeRef> args_;

        CallExpr(SemaNodeRef callee, Contiguous auto&& args) noexcept :
            callee_{ callee },
            args_{ std::forward<decltype(args)>(args) } {}
    };

    struct MemberExpr
    {
        SemaNodeRef base_;
        std::string member_;
        bool is_arrow_ = false;
        
        MemberExpr(SemaNodeRef base, StringLike auto&& member, bool is_arrow = false) :
            base_{ base },
            member_{ std::forward<decltype(member)>(member) },
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
        
        InitListExpr(Contiguous auto&& init_values) :
            init_values_{ std::forward<decltype(init_values)>(init_values) } {}
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
