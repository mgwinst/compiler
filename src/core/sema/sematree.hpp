#pragma once

#include "sema_node.hpp"

#define NODE_LIMIT (1 << 18)

namespace Sema
{
    struct SemaTree
    {   
        std::vector<SemaNode> nodes_;

        SemaTree() noexcept
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

        SemaNodeRef root() const noexcept 
        { 
            if (nodes_.empty())
                error_exit("Semantic Tree is empty");

            return SemaNodeRef{ 0 };
        }
    };

} // namespace Sema