#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <ranges>

#include "EarlyOptimizer.hpp"
#include "utils/casting.hpp"
#include "middleend/analysis/escape.hpp"

bool may_have_side_effect(const std::unique_ptr<Instruction>& inst)
{
    switch (inst->kind_) {
        case ValueKind::Store: // mutating memory state is a side effect
        case ValueKind::Call:
        case ValueKind::Return:
        case ValueKind::Branch:
            return true;
        default:
            return false;
    }
}

void EarlyOptimizer::trivial_dce()
{
    std::vector<Instruction*> dead;

    for (auto& function : program_.functions_) {
        for (auto& block : function->blocks_) {
            for (auto it = block->instructions_.rbegin(); it != block->instructions_.rend(); ++it) {
                if ((*it)->users_.empty() && !may_have_side_effect(*it)) {
                    dead.push_back(it->get());
                }
            }

            block->instructions_.remove_if([&dead](std::unique_ptr<Instruction>& inst) {
                return std::ranges::contains(dead, inst.get());
            });

            dead.clear();
        }
    }
}

template <typename T>
concept LoadOrStore = std::same_as<T, Load> || std::same_as<T, Store>;

Alloca* get_alloca_operand(LoadOrStore auto* inst)
{
    if (inst) {
        return dyn_cast<Alloca>(inst->operands_[0]);
    }

    return nullptr;
}

void EarlyOptimizer::remove_dead_stores()
{   
    std::unordered_map<Alloca*, Store*> last_store;
    std::vector<Store*> dead;

    for (auto& function : program_.functions_) {
        auto alloca_set = non_escaping_allocas(function);
        for (auto& block : function->blocks_) {
            for (auto& inst : std::views::reverse(block->instructions_)) {
                if (auto* store = dyn_cast<Store>(inst)) {
                    auto* a = get_alloca_operand(store);
                    if (alloca_set.contains(a)) {
                        if (last_store.contains(a)) {
                            dead.push_back(store);
                        }
                        last_store[a] = store;
                    }
                } else if (auto* load = dyn_cast<Load>(inst)) {
                    auto* a = get_alloca_operand(load);
                    if (alloca_set.contains(a)) {
                        last_store.erase(a);
                    }
                }
            }

            block->instructions_.remove_if([&dead](std::unique_ptr<Instruction>& inst) {
                return std::ranges::contains(dead, inst.get());
            });

            last_store.clear();
            dead.clear();
        }
    }
}

void remove_unreachable_blocks(std::unique_ptr<Function>& function, bool& changed)
{
    auto it = function->blocks_.begin();
    auto end = function->blocks_.end();

    while (it != end) {
        auto* block = it->get();

        if (block->predecessors().empty() && block != function->get_entry_block()) {
            it = function->blocks_.erase(it);
            changed = true;
        }

        ++it;
    }
}

void remove_redundant_branches(std::unique_ptr<Function>& function, bool& changed)
{
    for (auto& block : function->blocks_) {
        auto& insts = block->instructions_;

        if (insts.size() >= 2) {
            auto a = std::prev(insts.end(), 2);
            auto b = std::prev(insts.end());

            if (isa<Branch>(*a) && isa<Branch>(*b)) {
                insts.pop_back();
                changed = true;
            }
        }
    }
}

bool collapsible(BasicBlock* block)
{
    if (block->successors().size() == 1) {
        auto successor = block->successors()[0];
        if (successor->predecessors().size() == 1) {
            assert(block == successor->predecessors()[0]);
            return true;
        }
    }

    return false;
}

// return the successor so we can remove from function at the call site (hack b/c no intrusive list)
BasicBlock* combine_with_successor(BasicBlock* block)
{
    auto* successor = block->successors()[0];

    for (auto& inst : successor->instructions_) {
        inst->parent_ = block;
    }

    // remove the terminator branch block1(..., -> [br] <-) + block2(inst, inst, ...) before merging
    block->instructions_.pop_back();

    block->instructions_.splice(block->instructions_.end(), successor->instructions_);

    // this is just an empty block now, so return it and remove from function that owns it
    return successor;
}

void merge_linear_blocks(std::unique_ptr<Function>& function, bool& changed)
{
    std::unordered_set<BasicBlock*> dead;

    auto it = function->blocks_.begin();
    auto end = function->blocks_.end();

    while (it != end) {
        auto* block = it->get();

        if (dead.contains(block)) {
            ++it;
            continue;
        }
        
        if (collapsible(block)) {
            auto* dead_block = combine_with_successor(block);
            dead.insert(dead_block);
            changed = true; 
        }

        ++it;
    }

    function->blocks_.remove_if([&](auto& b) {
        return dead.contains(b.get());
    });
}

void EarlyOptimizer::cleanup_cfg()
{
    for (auto& function : program_.functions_) {

        bool changed = true;

        while (changed) {
            changed = false;

            remove_redundant_branches(function, changed);
            remove_unreachable_blocks(function, changed);
            merge_linear_blocks(function, changed);
        }
    }
}

