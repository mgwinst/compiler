#pragma once

#include "../../frontend/sema/sematree.hpp"
#include "../../context/context.hpp"

std::string type_to_str(const ModuleContext&, TypeID type_ref);
std::string node_to_str(const ModuleContext& ctx, const SemaTree& tree, const SemaNodeID node_id, std::string indent);

// TODO: better pretty printer design
class PrettyPrinter
{
public:
    PrettyPrinter(ModuleContext& ctx) :
        ctx_{ ctx } {}  

    void print(SemaTree& tree) const
    {
        std::println("{}\n", node_to_str(ctx_, tree, tree.root(), ""));
    }

private:
    ModuleContext& ctx_;
};

