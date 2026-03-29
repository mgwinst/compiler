#pragma once

#include "../../../context/context.hpp"

// namespace passes

namespace Sema
{
    void build_sema_tree(SemaContext& ctx, const AST& ast);
    void check_types(SemaContext& ctx);
    void desugar(SemaContext& ctx);

} // namespace Sema

