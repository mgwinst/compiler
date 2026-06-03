#include "IR.hpp"
#include "../../utils/utils.hpp"
#include "../../utils/casting.hpp"
#include "../../utils/cast_range.hpp"

BasicBlock::BasicBlock(std::string_view name) :
    Value{ValueKind::BasicBlock, none, name} {}

BasicBlock::~BasicBlock()
{
    // bug: should remain conditional but switch targets (emulate a fall through in the graph)

    for (Branch* branch : static_cast_view<Branch>(users_)) {
        if (branch->is_conditional()) {
            branch->branch_kind_ = BranchKind::Unconditional;
            auto it = ranges::find(branch->operands_, this);
            branch->operands_.erase(it);
        } else {
            assert(isa<BasicBlock>(branch->parent_));
            static_cast<BasicBlock*>(branch->parent_)->instructions_.pop_back();
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

void BasicBlock::remove_from_parent()
{
    assert(parent_ != nullptr);

    if (auto* func = dyn_cast<Function>(parent_)) {
        auto index = (reinterpret_cast<std::byte*>(this) - reinterpret_cast<std::byte*>(func->blocks_.data())) / sizeof(std::unique_ptr<BasicBlock>);
        swap_pop(func->blocks_, index);
    }
}