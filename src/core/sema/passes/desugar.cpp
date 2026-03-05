#include "sema_pass.hpp"

/*
    dusugar

    for -> while
    a[i] -> *(a + i)
    x += 3 -> x = x + 3
    3 + 5 -> 8, (simple constant folding)
*/

void desugar_tree(Sema::SemaContext& ctx, SemaNodeRef ref)
{
}