#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <ranges>

#include "EarlyOptimizer.hpp"
#include "../../utils/casting.hpp"

bool escapes_via(Value* value, Value* target, std::unordered_set<Value*>& visited)
{
    if (visited.contains(value)) {
        return false;
    }

    visited.insert(value);

    switch (value->kind_) {
        case ValueKind::Store: {
            auto* store = static_cast<Store*>(value);
            return store->operands_[1] == target; // are we storing this alloca ptr, (since we are storing, we lost value flow because it has left ssa world)
        }

        case ValueKind::Return: {
            auto* ret = static_cast<Return*>(value);
            return ret->operands_[0] == target; // this is okay because this case is checking for returning the alloca itself, not a load of hte alloca, otherwise this operand would be a load instruction, not the alloca. the lowering engine will handle this case
        }

        case ValueKind::PtrAdd: {
            auto* ptradd = static_cast<PtrAdd*>(value);
            if (ptradd->operands_[0] == target) {
                for (auto* use : ptradd->users_) {
                    if (escapes_via(use, ptradd, visited)) {
                        return true;
                    }
                }
            }
            return false;
        }

        // call(&alloca) ... 

        default: {
            return false;
        }
    }
}

bool escapes(Alloca* alloca)
{
    std::unordered_set<Value*> v;

    for (auto* use : alloca->users_) {
        if (escapes_via(use, alloca, v)) {
            return true;
        }
    }

    return false;
}

std::unordered_set<Alloca*> non_escaping_allocas(const std::unique_ptr<Function>& function)
{
    std::unordered_set<Alloca*> allocas;

    for (auto& block : function->blocks_) {
        for (auto& inst : block->instructions_) {
            if (auto* alloca = dyn_cast<Alloca>(inst)) {
                if (escapes(alloca) == false) {
                    allocas.insert(alloca);
                }
            }
        }
    }

    return allocas;
}

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

// remove double branch instructions to expose dead branches
// do this during the merge pass to expose dead blocks during scan
// what traversal will make this most efficient

bool double_branch(std::unique_ptr<BasicBlock>& block)
{
    auto& insts = block->instructions_;

    if (insts.size() >= 2) {
        auto a = std::prev(insts.end(), 2);
        auto b = std::prev(insts.end());
     
        return isa<Branch>(*a) && isa<Branch>(*b);
    }

    return false;
}

// invariance = any block that exists must mean that a branch instruction leads to it

// handle branch folding once we have handle booleans properly
void remove_unreachable_blocks(std::unique_ptr<Function>& function, bool& changed)
{
    bool local_change = true;
    while (local_change) {
        local_change = false;
        for (auto it = function->blocks_.begin(); it != function->blocks_.end(); ) {
            if ((*it)->predecessors().empty() && (*it)->name_ != "entry") {
                it = function->blocks_.erase(it);
                changed = local_change = true;
                continue;
            }

            if (double_branch(*it)) {
                (*it)->instructions_.pop_back();
                changed = local_change = true;
            }

            ++it;
        }
    }
}

void remove_empty_blocks(std::unique_ptr<Function>& function, bool& changed)
{
    auto block_has_single_branch = [](auto& block) -> bool {
        return block->instructions_.size() == 1 && isa<Branch>(block->instructions_.front());
    };

    for (auto it = function->blocks_.begin(); it != function->blocks_.end(); ) {
        if ((*it)->empty() || block_has_single_branch(*it)) {
            it = function->blocks_.erase(it);
            changed = true;
        } else {
            ++it;
        }
    }
}

bool collapsible(std::unique_ptr<BasicBlock>& block)
{
    if (block->successors().size() == 1) {
        auto successor = block->successors()[0];
        if (successor->predecessors().size() == 1) {
            assert(block.get() == successor->predecessors()[0]);
            return true;
        }
    }

    return false;
}

void combine_with_successor(std::unique_ptr<BasicBlock>& block)
{
    auto* successor = block->successors()[0];

    for (auto& inst : successor->instructions_) {
        inst->parent_ = block.get();
    }

    // we have to remove the branch before the merging (br, new, new, ...), now when we call successor on next pass, terminator() does not return br/ret instruction
    block->instructions_.pop_back();

    block->instructions_.splice(block->instructions_.end(), successor->instructions_);

    successor->remove_from_parent();
}

void merge_linear_blocks(std::unique_ptr<Function>& function, bool& changed)
{
    for (auto& block : function->blocks_) {
        if (collapsible(block)) {
            combine_with_successor(block);
            changed = true;
        }
    }
}

void EarlyOptimizer::cleanup_cfg()
{
    for (auto& function : program_.functions_) {

        bool changed = true;

        while (changed) {
            changed = false;

            remove_unreachable_blocks(function, changed);
            remove_empty_blocks(function, changed);
            merge_linear_blocks(function, changed);
        }
    }
}