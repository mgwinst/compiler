#include "pipeline.hpp"
#include "../frontend/error/diagnostics.hpp"
#include "../frontend/parser/parser.hpp"
#include "../frontend/sema/sema_tree_builder.hpp"
#include "../frontend/sema/desugar.hpp"
#include "../frontend/sema/types/type_checker.hpp"
#include "../lowering/LoweringEngine.hpp"
#include "../lowering/ir.hpp"

inline void report(const Diagnostics& diag)
{
    if (diag.contains_warnings()) {
        diag.dump_warnings();
    }

    if (diag.contains_errors()) {
        diag.dump_errors();
        exit(1);
    }
}

AST parse(SourceFile& source_file)
{
    Parser parser{ source_file };
    auto ast = parser.run();

    report(parser.diagnostics_);

    return ast;
}

SemaTree decorate(ModuleContext& ctx, const AST& ast)
{
    auto sema_tree = SemaTreeBuilder{ctx, ast}.run();
    report(ctx.diagnostics_);
    return sema_tree;
}

void type_check(ModuleContext& ctx, const SemaTree& tree)
{
    TypeChecker{ctx, tree}.run();
    report(ctx.diagnostics_);
}

void desugar(ModuleContext& ctx, SemaTree& tree)
{
    TreeDesugarer(ctx, tree).run();
}

IR::Program lower(const ModuleContext& ctx, const SemaTree& tree)
{
    return LoweringEngine{ctx, tree}.run();
}