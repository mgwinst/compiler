#pragma once

#include "../../context/context.hpp"

namespace Sema
{
    SemaTree& build_sema_tree(SemaContext& ctx, const AST& ast);
    void check_types(SemaContext& ctx);
    void desugar_tree(Sema::SemaContext& ctx, SemaNodeRef ref);

} // namespace Sema

