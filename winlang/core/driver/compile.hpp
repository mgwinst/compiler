#pragma once

#include "context/context.hpp"
#include "frontend/ast/ast.hpp"
#include "frontend/sema/sematree.hpp"
#include "middleend/ir/IR.hpp"

class Compiler
{
public:
    Compiler(CompilerContext context) :
        context_{ std::move(context) } {}

    void compile_modules();

private:
    const CompilerContext context_;

    void compile(const Module& module);
    void debug_compile(const Module& module); // -debug=<#>

    AST parse(const Module& module);
    SemaTree analyze(ModuleContext& ctx, AST& ast);
    Program lower(ModuleContext& ctx, const SemaTree& tree);
    void optimize(Program& program);
};