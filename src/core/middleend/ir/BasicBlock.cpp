#include "IR.hpp"
#include "utils/utils.hpp"
#include "utils/casting.hpp"

BasicBlock::BasicBlock(std::string_view name) :
    Value{ValueKind::BasicBlock, nullptr, name} {}

BasicBlock::~BasicBlock()
{
    for (auto* branch : static_cast_view<Branch>(users_)) {
        if (branch->branch_kind_ == BranchKind::Conditional) {
            branch->operands_.erase(branch->operands_.begin()); // remove the condition value
            std::erase_if(branch->operands_, [this](auto* value) { return value == this; }); // remove the target (this block)
            branch->branch_kind_ = BranchKind::Unconditional;
        } else {
            auto* pred = cast<BasicBlock>(branch->parent_);
            assert(pred->instructions_.size() > 0);
            pred->instructions_.pop_back();
        }
    } 

    while (!instructions_.empty()) {
        instructions_.pop_back();
    }
}

Instruction* BasicBlock::terminator()
{
    if (instructions_.empty())
        return nullptr;

    if (isa<Return>(instructions_.back()) || isa<Branch>(instructions_.back()))
        return instructions_.back().get();
    
    return nullptr;
}

bool BasicBlock::empty()
{
    return instructions_.empty();
}

small_vector<BasicBlock*, 2> BasicBlock::successors()
{
    small_vector<BasicBlock*, 2> result{ };   

    auto* term = terminator();
    if (!term)
        return result;

    if (!isa<Branch>(term))
        return result;

    auto* branch = static_cast<Branch*>(term);

    for (auto* target : static_cast_view<BasicBlock>(branch->targets()))
        result.push_back(target);

    return result;
}