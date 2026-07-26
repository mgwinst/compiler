#include <thread>

#include "driver/compile.hpp"

#include "utils/utils.hpp"
#include "utils/temp_ir_naming.hpp"
#include "utils/print/print.hpp"
#include "frontend/error/diagnostics.hpp"
#include "frontend/parser/parser.hpp"
#include "frontend/sema/analyzer.hpp"
#include "middleend/lowering/LoweringEngine.hpp"
#include "middleend/transforms/TransformPassManager.hpp"

void Compiler::compile_modules()
{
    std::vector<std::jthread> threads;
    for (auto& module : context_.modules()) {
        threads.emplace_back([this, &module] { compile(module); });
    }
}

void Compiler::compile(const Module& module)
{
    ModuleContext ctx;   

    AST ast = parse(module);

    SemaTree sema_tree = analyze(ctx, ast);

    Program program = lower(ctx, sema_tree);

    name_values(program);

    if (context_.flags().contains("-ast")) {
        print(sema_tree);
    }

    if (context_.flags().contains("-ir")) {
        print(program);
    }

    optimize(program);

    name_values(program);

    if (context_.flags().contains("-opt")) {
        print(program);
    }
}

AST Compiler::parse(const Module& module)
{
    Parser parser{ module };
    AST ast = parser.run();

    parser.diagnostics_.report();

    return ast;
}

SemaTree Compiler::analyze(ModuleContext& ctx, AST& ast)
{
    SemaTree sema_tree = SemanticAnalyzer{ctx, ast}.run();

    ctx.diagnostics_.report();

    return sema_tree;
}

Program Compiler::lower(ModuleContext& ctx, const SemaTree& tree)
{
    return LoweringEngine{ctx, tree}.run();
}

void Compiler::optimize(Program& program)
{
    TransformPassManager{{
        Transforms::TRIVIAL_DCE,
        Transforms::DSE,
        Transforms::CFG_CLEANUP,
        Transforms::MEM2REG,
    }}.run(program);
}