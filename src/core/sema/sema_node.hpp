#pragma once

#include "symbol.hpp"

namespace Sema
{
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

        FuncDecl(SymbolRef symbol, TypeRef type, Contiguous auto&& params, ASTNodeRef body) noexcept :
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

} // namespace Sema

enum class SemaNodeKind : uint8_t 
{
    CompilationUnitDecl,
    VarDecl,
    ParamDecl,
    FuncDecl,
    RecordDecl,

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
inline constexpr SemaNodeKind sema_node_kind_v = SemaNodeKind::Invalid;

template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::CompilationUnitDecl>   = SemaNodeKind::CompilationUnitDecl;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::VarDecl>               = SemaNodeKind::VarDecl;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::ParamDecl>             = SemaNodeKind::ParamDecl;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::FuncDecl>              = SemaNodeKind::FuncDecl;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::RecordDecl>            = SemaNodeKind::RecordDecl;

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
            case SemaNodeKind::Invalid: [[fallthrough]];
            default: break;
        }
    }

    SemaNodeKind get_kind() const noexcept
    {
        return kind_;
    }

    // these should return T* so you can return optional reference
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
