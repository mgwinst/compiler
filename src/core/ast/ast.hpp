#pragma once

#include "ast_node.hpp"

struct AST
{   
    std::vector<ASTNode> nodes_;

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
