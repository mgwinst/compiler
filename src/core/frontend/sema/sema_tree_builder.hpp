#pragma once

#include "../../context/context.hpp"
#include "../sema/sematree.hpp"

class SemaTreeBuilder
{
public:
    SemaTreeBuilder(ModuleContext& ctx, const AST& ast) :
        ctx_{ ctx },
        ast_{ ast } {}

    SemaTree run();

private:
    ModuleContext& ctx_;
    const AST& ast_;
    SemaTree sema_tree_;

    SemaNodeID build_sema_node(ASTNodeID node_id);
};