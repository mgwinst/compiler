#pragma once

#include <ranges>
#include <cassert>

#include "frontend/ast/ast.hpp"

using Syntax::ASTNode;

// if we want to upcast to base pointer should we just static_assert or build mechanism for that in cast<T>
// this is important

template <DerivedFromASTNode Derived>
bool isa(ASTNode* ptr)
{
    return ptr && ptr->kind_ == ast_node_kind_v<Derived>;
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
