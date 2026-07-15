#include "test/test_utilities.hpp"

TEST_CASE("Instruction creation updates def-use chain")
{
    IRBuilder builder{};

    auto* type1 = new IntegerType{32, true};
    auto* alloca1 = builder.create<Alloca>(type1, "a");

    auto* type2 = new IntegerType{32, true};
    auto* alloca2 = builder.create<Alloca>(type2, "b");

    auto* add = builder.create<Add>(alloca1, alloca2);

    CHECK(alloca1->users_.size() == 1);
    CHECK(alloca1->users_[0] == add);

    CHECK(alloca2->users_.size() == 1);
    CHECK(alloca2->users_[0] == add);

    CHECK(add->users_.empty());
    CHECK(add->operands_.size() == 2);
    CHECK(add->operands_[0] == alloca1);
    CHECK(add->operands_[1] == alloca2);
}

TEST_CASE("replace_uses_with() updates all uses with new value")
{
    IRBuilder builder{};

    auto* type1 = new IntegerType{32, true};
    auto* alloca1 = builder.create<Alloca>(type1, "a");

    auto* type2 = new IntegerType{32, true};
    auto* alloca2 = builder.create<Alloca>(type2, "b");

    auto* add = builder.create<Add>(alloca1, alloca2);

    CHECK(add->operands_[0] == alloca1);

    auto* const_value_to_replace_alloca1 = builder.create<Const>(42);

    alloca1->replace_uses_with(const_value_to_replace_alloca1);

    CHECK(add->operands_[0] == const_value_to_replace_alloca1);
}