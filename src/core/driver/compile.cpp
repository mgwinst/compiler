#include "compile.hpp"

#include "../utils/temp_ir_naming.hpp"

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
    SemaTree sema_tree = decorate(ctx, ast);
    type_check(ctx, sema_tree);
    desugar(ctx, sema_tree);

    Program program = lower(ctx, sema_tree);
    name_values(program); // temporary debug

    PrettyPrinter printer{ ctx };
    
    if (context_.flags().contains("-ast")) {
        printer.print(sema_tree);
    }

    if (context_.flags().contains("-ir")) {
        printer.print(program);
    }

    early_optimize(program);

    if (context_.flags().contains("-opt")) {
        printer.print(program);
    }
}

void report(const Diagnostics& diag)
{
    if (diag.contains_warnings()) {
        diag.dump_warnings();
    }

    if (diag.contains_errors()) {
        diag.dump_errors();
        exit(1);
    }
}

AST Compiler::parse(const Module& module)
{
    Parser parser{ module };
    auto ast = parser.run();

    report(parser.diagnostics_);

    return ast;
}

SemaTree Compiler::decorate(ModuleContext& ctx, const AST& ast)
{
    auto sema_tree = SemaTreeBuilder{ctx, ast}.run();
    report(ctx.diagnostics_);
    return sema_tree;
}

void Compiler::type_check(ModuleContext& ctx, const SemaTree& tree)
{
    TypeChecker{ctx, tree}.run();
    report(ctx.diagnostics_);
}

void Compiler::desugar(ModuleContext& ctx, SemaTree& tree)
{
    TreeDesugarer{ctx, tree}.run();
}

Program Compiler::lower(const ModuleContext& ctx, const SemaTree& tree)
{
    return LoweringEngine{ctx, tree}.run();
}

void Compiler::early_optimize(Program& program)
{
    EarlyOptimizer{program}.run();
}
