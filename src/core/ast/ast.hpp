#pragma once

#include "ast_node.hpp"

struct AST
{   
    std::vector<ASTNode> nodes_;

    AST() noexcept;

    template <typename T, typename... Args>
        requires std::is_constructible_v<T, Args...>
    [[nodiscard]] auto emplace(Args&&... args) noexcept
    {
        nodes_.emplace_back(std::in_place_type<T>, std::forward<Args>(args)...);
        return ASTNodeRef{ nodes_.size() - 1 };
    }

    ASTNodeRef root() const noexcept;

    void print() const noexcept;
    std::string node_to_str(ASTNodeRef ref, std::string indent) const noexcept;
};