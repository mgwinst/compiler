#pragma once

#include "ast_node.hpp"

#define NODE_LIMIT (1 << 18)

struct AST
{   
    std::vector<ASTNode> nodes_;

    AST() noexcept
    {
        nodes_.reserve(NODE_LIMIT);
    }

    template <typename T, typename... Args>
        requires std::is_constructible_v<T, Args...>
    [[nodiscard]] auto emplace(Args&&... args) noexcept
    {
        nodes_.emplace_back(std::in_place_type<T>, std::forward<Args>(args)...);
        return nodes_.size() - 1;
    }

    ASTNodeRef root() const noexcept
    {
        if (nodes_.empty())
            error_exit("AST is empty");
        
        return ASTNodeRef{ 0 };
    }
};
