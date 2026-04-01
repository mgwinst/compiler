#pragma once

#include "sema_node.hpp"

struct SemaTree
{   
    std::vector<SemaNode> nodes_;

    template <typename T, typename... Args>
        requires std::is_constructible_v<T, Args...>
    [[nodiscard]] auto emplace(Args&&... args) noexcept
    {
        nodes_.emplace_back(std::in_place_type<T>, std::forward<Args>(args)...);
        return nodes_.size() - 1;
    }

    SemaNodeID root() const noexcept 
    { 
        if (nodes_.empty())
            error_exit("Semantic Tree is empty");

        return static_cast<SemaNodeID>(0);
    }
};