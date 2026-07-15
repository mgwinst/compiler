#include "doctest/doctest.h"

#include "context/context.hpp"
#include "frontend/parser/parser.hpp"
#include "frontend/sema/analyzer.hpp"
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

inline std::pair<SemaTree, ModuleContext> analyze(const std::string& source_file)
{
    ModuleContext ctx;   
    Module module = get_module(source_file);
    Parser parser{module};
    AST ast = parser.run();
    SemaTree sema_tree = SemanticAnalyzer{ctx, ast, false}.run();

    return {std::move(sema_tree), std::move(ctx)};
}