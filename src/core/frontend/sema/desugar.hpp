#pragma once

#include "../../context/context.hpp"
#include "sematree.hpp"

class TreeDesugarer
{
public:
    TreeDesugarer(ModuleContext& ctx, SemaTree& tree) :
        ctx_{ ctx },
        tree_{ tree } {}

    void run();

private:
    ModuleContext& ctx_;
    SemaTree& tree_;

    void desugar_node(SemaNodeID node_id);
};