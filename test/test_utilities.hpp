#include "doctest/doctest.h"

#include "context/context.hpp"
#include "frontend/parser/parser.hpp"
#include "frontend/sema/analyzer.hpp"
#include "middleend/lowering/LoweringEngine.hpp"
#include "middleend/transforms/TransformPassManager.hpp"
#include "utils/utils.hpp"
#include "utils/casting.hpp"

// check source location for one of the examples

inline AST parse(const std::string& source_file)
{
    Module module = get_module(source_file);
    Parser parser{module};
    AST ast = parser.run();
    return ast;
}

inline std::pair<ModuleContext, SemaTree> analyze(const std::string& source_file)
{
    ModuleContext ctx;   
    AST ast = parse(source_file);
    SemaTree sema_tree = SemanticAnalyzer{ctx, ast, false}.run();
    return {std::move(ctx), std::move(sema_tree)};
}

inline Program lower(const std::string& source)
{
    auto [ctx, sema_tree] = analyze(source);
    return LoweringEngine{ctx, sema_tree}.run();
}