#pragma once

#include "../../../context/context.hpp"
#include "../sematree.hpp"

class TypeChecker
{
public:
    TypeChecker(ModuleContext& ctx, const SemaTree& tree) :
        ctx_{ ctx },
        tree_{ tree } {}
    
    void run();

private:
    ModuleContext& ctx_;
    const SemaTree& tree_;

    std::optional<TypeID> check_type(SemaNodeID node_id);
};