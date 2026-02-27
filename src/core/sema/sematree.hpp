#pragma once

#include <memory>

#include "../ast/ast.hpp"
#include "symbol.hpp"
#include "types/type_pool.hpp"
#include "sema_node.hpp"
#include "../utils/alias.hpp"

// should type/symbol tables be in a global compilation context object?
// should the subsystems query this context? What are the consequences of such design?

// think about the appropriate relationship between those things and the SemaTree itself... SemaContext object maybe for smaller granularity?

namespace Sema
{
    struct SemaContext;

    struct SemaTree
    {   
        std::vector<SemaNode> nodes_;

        SemaTree() noexcept;

        template <typename T, typename... Args>
            requires std::is_constructible_v<T, Args...>
        [[nodiscard]] auto emplace(Args&&... args) noexcept
        {
            nodes_.emplace_back(std::in_place_type<T>, std::forward<Args>(args)...);
            return nodes_.size() - 1;
        }

        SemaNodeRef root() const noexcept;

        SemaNodeRef build_sema_node(SemaContext& sema, const AST& ast, ASTNodeRef ast_node_ref) noexcept;
    };

    struct SemaContext
    {
        std::shared_ptr<SemaTree> sema_tree_;
        std::shared_ptr<TypePool> type_pool_;
        std::shared_ptr<SymbolTable> symbol_table_;

        SemaContext() : 
            sema_tree_{ std::make_shared<SemaTree>() },
            type_pool_{ std::make_shared<TypePool>() },
            symbol_table_{ std::make_shared<SymbolTable>() } {}

        void build_sema_tree(const AST& ast) noexcept
        {
            sema_tree_->build_sema_node(*this, ast, ast.root());
        }
    };

} // namespace Sema