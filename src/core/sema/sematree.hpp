#pragma once

#include "../ast/ast.hpp"
#include "sema_node.hpp"
#include <memory>

// should type/symbol tables be in a global compilation context object?
// should the subsystems query this context? What are the consequences of such design?

// think about the appropriate relationship between those things and the SemaTree itself... SemaContext object maybe for smaller granularity?

namespace Sema
{
    using SemaNodeRef = int64_t;

    struct SemaContext;

    struct SemaTree
    {   
        std::vector<SemaNode> nodes_;
        std::shared_ptr<SemaContext> sema_ctx_{ nullptr };

        SemaTree(SemaContext& sema_ctx, const AST& ast) noexcept;

        template <typename T, typename... Args>
            requires std::is_constructible_v<T, Args...>
        [[nodiscard]] auto emplace(Args&&... args) noexcept
        {
            nodes_.emplace_back(std::in_place_type<T>, std::forward<Args>(args)...);
            return ASTNodeRef{ nodes_.size() - 1 };
        }

        ASTNodeRef root() const noexcept;

        SemaNodeRef build_sema_node(SemaContext& sema_ctx, const AST& ast, ASTNodeRef ast_node_ref) noexcept;
    };

} // namespace Sema