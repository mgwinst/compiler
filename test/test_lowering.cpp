#include <doctest/doctest.h>

#include "test/test_utilities.hpp"

TEST_CASE("variable declaration -> alloca")
{
    auto program = lower("srctest/lowering/variable.w");

    auto& function = program.functions_.front();
    auto& block = function->blocks_.front();

    auto it = block->instructions_.begin();

    CHECK(isa<Alloca>(*it));
}

TEST_CASE("assignment -> store") 
{
    auto program = lower("srctest/lowering/assign.w");

    auto& function = program.functions_.front();
    auto& block = function->blocks_.front();

    auto it = block->instructions_.begin();

    CHECK(isa<Alloca>(*it++));
    CHECK(isa<Store>(*it));
}

TEST_CASE("integer addition")
{
    auto program = lower("srctest/lowering/add.w");

    auto& function = program.functions_.front();
    auto& block = function->blocks_.front();

    auto it = block->instructions_.begin();

    CHECK(isa<Alloca>(*it++));
    CHECK(isa<Add>(*it++));
    CHECK(isa<Store>(*it));
}

TEST_CASE("variable reference") 
{
    auto program = lower("srctest/lowering/reference.w");

    auto& function = program.functions_.front();
    auto& block = function->blocks_.front();

    auto it = block->instructions_.begin();
    
    CHECK(isa<Alloca>(*it++));
    CHECK(isa<Store>(*it++));
    CHECK(isa<Alloca>(*it++));
    CHECK(isa<Load>(*it++));
    CHECK(isa<Store>(*it));
}

TEST_CASE("== -> eq") 
{
    auto program = lower("srctest/lowering/equal.w");

    auto& function = program.functions_.front();
    auto& block = function->blocks_.front();

    auto it = block->instructions_.begin();
    
    CHECK(isa<Alloca>(*it++));
    CHECK(isa<Store>(*it++));
    CHECK(isa<Alloca>(*it++));
    CHECK(isa<Store>(*it++));
    CHECK(isa<Load>(*it++));
    CHECK(isa<Load>(*it++));
    CHECK(isa<Eq>(*it));
}

TEST_CASE("!= -> ne") 
{
    auto program = lower("srctest/lowering/not_equal.w");

    auto& function = program.functions_.front();
    auto& block = function->blocks_.front();

    auto it = block->instructions_.begin();
    
    CHECK(isa<Alloca>(*it++));
    CHECK(isa<Store>(*it++));
    CHECK(isa<Alloca>(*it++));
    CHECK(isa<Store>(*it++));
    CHECK(isa<Load>(*it++));
    CHECK(isa<Load>(*it++));
    CHECK(isa<Ne>(*it));
}

TEST_CASE("!x -> not x") 
{
    auto program = lower("srctest/lowering/logical_not.w");

    auto& function = program.functions_.front();
    auto& block = function->blocks_.front();

    auto it = block->instructions_.begin();
    
    CHECK(isa<Alloca>(*it++));
    CHECK(isa<Store>(*it++));
    CHECK(isa<Load>(*it++));
    CHECK(isa<Not>(*it));
}

TEST_CASE("(int < int) -> slt") {}
TEST_CASE("(int <= int) -> sle") {}
TEST_CASE("(uint < uint) -> ult") {}
TEST_CASE("(uint <= uint) -> ule") {}

TEST_CASE("<< -> shl") 
{
    auto program = lower("srctest/lowering/shift_left.w");

    auto& function = program.functions_.front();
    auto& block = function->blocks_.front();

    auto it = block->instructions_.begin();
    
    CHECK(isa<Alloca>(*it++));
    CHECK(isa<Store>(*it++));
    CHECK(isa<Load>(*it++));
    CHECK(isa<Shl>(*it));
}

TEST_CASE("signed >> -> ashr") 
{
    auto program = lower("srctest/lowering/arith_shift_right.w");

    auto& function = program.functions_.front();
    auto& block = function->blocks_.front();

    auto it = block->instructions_.begin();
    
    CHECK(isa<Alloca>(*it++));
    CHECK(isa<Store>(*it++));
    CHECK(isa<Load>(*it++));
    CHECK(isa<Ashr>(*it));
}

TEST_CASE("unsigned >> -> lshr") 
{
    auto program = lower("srctest/lowering/logical_shift_right.w");

    auto& function = program.functions_.front();
    auto& block = function->blocks_.front();

    auto it = block->instructions_.begin();
    
    CHECK(isa<Alloca>(*it++));
    CHECK(isa<Store>(*it++));
    CHECK(isa<Load>(*it++));
    CHECK(isa<Lshr>(*it));
}

TEST_CASE("^ -> xor") {}
TEST_CASE("| -> or") {}
TEST_CASE("& -> and") {}
TEST_CASE("array indexing -> ptradd") {}
TEST_CASE("struct member access -> ptradd") {}
TEST_CASE("&&") {}
TEST_CASE("!=") {}
TEST_CASE("function arguments spill to stack slot") {}
TEST_CASE("functions always branch to return basic block with ret instruction ") {}
TEST_CASE("pointer dereference -> double load") {}
TEST_CASE("if statement -> br label if.then, label if.end") {}
TEST_CASE("if-else statement -> br label if.then, label if.else") {}
TEST_CASE("if (x) -> ne x, 0") {}