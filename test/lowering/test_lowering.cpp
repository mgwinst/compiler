#include <doctest/doctest.h>

#include "driver/compile.hpp"
#include "frontend/parser/parser.hpp"
#include "frontend/sema/analyzer.hpp"
#include "middleend/lowering/LoweringEngine.hpp"
#include "utils/casting.hpp"

Program lower(const std::string& source)
{
    Module module = get_module(source);

    ModuleContext ctx;

    Parser parser{module};

    AST ast = parser.run();
    parser.diagnostics_.report();

    SemaTree sema_tree = SemanticAnalyzer{ctx, ast}.run();
    ctx.diagnostics_.report();

    return LoweringEngine{ctx, sema_tree}.run();
}

TEST_CASE("add")
{
    auto program = lower("lowering/src/add.w");

    /*
    REQUIRE(program.functions_.size() == 1);

    auto* function = std::next(program.functions_.begin(), 0)->get();

    REQUIRE(function->blocks_.size() == 2);

    
    auto* block = std::next(function->blocks_.begin(), 0)->get();

    auto instructions = block->instructions_.begin();

    CHECK(isa<Alloca>(std::next(instructions, 0)->get()));
    CHECK(isa<Alloca>(std::next(instructions, 1)->get()));
    CHECK(isa<Alloca>(std::next(instructions, 2)->get()));
    CHECK(isa<Load>(std::next(instructions, 3)->get()));
    CHECK(isa<Load>(std::next(instructions, 4)->get()));
    CHECK(isa<Add>(std::next(instructions, 5)->get()));
    CHECK(isa<Store>(std::next(instructions, 6)->get()));
    CHECK(isa<Branch>(std::next(instructions, 7)->get()));

    auto* branch = cast<Branch>(std::next(instructions, 7)->get());
    
    auto* return_block = cast<BasicBlock>(branch->targets()[0]);

    CHECK(return_block->instructions_.size() == 1);

    CHECK(isa<Return>(std::next(return_block->instructions_.begin(), 0)->get()));
    */
}



