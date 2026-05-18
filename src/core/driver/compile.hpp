#pragma once

#include "../frontend/error/diagnostics.hpp"
#include "../frontend/parser/parser.hpp"
#include "../frontend/sema/sema_tree_builder.hpp"
#include "../frontend/sema/desugar.hpp"
#include "../frontend/sema/types/type_checker.hpp"
#include "../middleend/lowering/LoweringEngine.hpp"
#include "../middleend/opt/EarlyOptimizer.hpp"
#include "../context/context.hpp"
#include "../utils/utils.hpp"

class Compiler
{
public:
    Compiler(CompilerContext context) :
        context_{ std::move(context) } {}

    void compile_modules();

private:
    const CompilerContext context_;

    void compile(const Module& module);
    void debug_compile(const Module& module);

    AST parse(const Module& module);
    SemaTree decorate(ModuleContext& ctx, const AST& ast);
    void type_check(ModuleContext& ctx, const SemaTree& tree);
    void desugar(ModuleContext& ctx, SemaTree& tree);
    IR::Program lower(const ModuleContext& ctx, const SemaTree& tree);
    void early_optimize(IR::Program& program);
};

