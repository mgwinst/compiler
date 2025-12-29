#pragma once

#include <unordered_map>

#include "ast/node.hpp"
#include "sema/types.hpp"

struct AST
{   
    std::vector<Node> nodes_;
    TypePool types_;

    AST() noexcept;

    template <typename T, typename... Args>
        requires std::is_constructible_v<T, Args...>
    [[nodiscard]] auto emplace(Args&&... args) noexcept
    {
        nodes_.emplace_back(std::in_place_type<T>, std::forward<Args>(args)...);
        return NodeRef{ nodes_.size() - 1 };
    }

    NodeRef root() const noexcept;
};



