#pragma once

#include "../context/context.hpp"
#include "../frontend/ast/ast.hpp"
#include "../frontend/sema/sematree.hpp"
#include "../lowering/ir.hpp"

AST parse(SourceFile& source_file);
SemaTree decorate(ModuleContext& ctx, const AST& ast);
void type_check(ModuleContext& ctx, const SemaTree& tree);
void desugar(ModuleContext& ctx, SemaTree& tree);
IR::Program lower(const ModuleContext& ctx, const SemaTree& tree);
