#pragma once

#include <ranges>
#include <cassert>

#include "frontend/ast/ast.hpp"
#include "frontend/sema/types/types.hpp"
#include "frontend/sema/sematree.hpp"
#include "middleend/ir/IR.hpp"

using Syntax::ASTNode;
using Sema::SemaNode;

// if we want to upcast to base pointer should we just static_assert or build mechanism for that in cast<T>


template <DerivedFromASTNode Derived>
bool isa(ASTNode* ptr)
{
    return ptr && ptr->kind_ == ast_node_kind_v<Derived>;
}

template <AbstractNode T>
bool category(auto* ptr)
{
    if constexpr (std::same_as<T, Sema::Decl>)
        return ptr->kind_ >= SemaNodeKind::ModuleDecl && ptr->kind_ <= SemaNodeKind::RecordDecl;
    else if constexpr (std::same_as<T, Sema::Stmt>)
        return ptr->kind_ >= SemaNodeKind::CompoundStmt && ptr->kind_ <= SemaNodeKind::ForStmt;
    else
        return ptr->kind_ >= SemaNodeKind::IntegerLiteralExpr && ptr->kind_ <= SemaNodeKind::ImplicitCastExpr;
}

template <DerivedFromSemaNode Derived>
bool isa(SemaNode* ptr)
{
    if constexpr (AbstractNode<Derived>)
        return category<Derived>(ptr);

    return ptr && ptr->kind_ == sema_node_kind_v<Derived>;
}

template <DerivedFromType Derived>
bool isa(Type* ptr)
{
    return ptr && ptr->kind_ == type_kind_v<Derived>;
}

template <typename T, DerivedFromValue V>
bool isa(const std::unique_ptr<V>& value)
{
    if (!value)
        return false;

    return value->kind_ == value_kind_v<T>;
}

template <typename T, DerivedFromValue V>
bool isa(V* value)
{
    if (!value)
        return false;

    return value->kind_ == value_kind_v<T>;
}

template <typename T, typename U>
T* cast(U* ptr)
{
    assert(isa<T>(ptr));
    return static_cast<T*>(ptr);
}

template <typename T, typename U>
T* dyn_cast(U* ptr)
{
    return isa<T>(ptr) ? static_cast<T*>(ptr) : nullptr;
}

template <typename T, typename U>
T* dyn_cast(const std::unique_ptr<U>& ptr)
{
    return isa<T>(ptr.get()) ? static_cast<T*>(ptr.get()) : nullptr;
}

// range casting

template <typename T>
auto static_cast_view(auto&& range)
{
    return std::forward<decltype(range)>(range)
        | std::views::filter([](auto* p) { return p != nullptr; })
        | std::views::transform([](auto* p) { return static_cast<T*>(p); });
}