#pragma once

#include "symbol.hpp"

namespace Sema
{
    struct CompilationUnitDecl
    {
        std::string name_;
        std::vector<ASTNodeRef> decls_;
        SymbolRef symbol_;
        TypeRef type_;

        CompilationUnitDecl(StringLike auto&& name) noexcept :
            name_{ std::forward<decltype(name)>(name) } {}
    };

    struct VarDecl
    {
        SymbolRef symbol_;
        TypeRef type_;
        std::optional<ASTNodeRef> init_; // single expr or init_list
    };

} // namespace Sema

enum class SemaNodeKind : uint8_t 
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
inline constexpr SemaNodeKind sema_node_kind_v = SemaNodeKind::Invalid;

template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::CompilationUnitDecl>   = SemaNodeKind::CompilationUnitDecl;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::VarDecl>               = SemaNodeKind::VarDecl;

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
            case SemaNodeKind::CompilationUnitDecl:  destroy<Sema::CompilationUnitDecl>();  break;
            case SemaNodeKind::VarDecl:              destroy<Sema::VarDecl>();              break;
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
        if (kind_ == sema_node_kind_v<T>)
            return *std::launder(reinterpret_cast<const T*>(data_));
    }

    template <typename T>
    [[nodiscard]] T& as()
    {
        if (kind_ == sema_node_kind_v<T>)
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
