#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <ranges>

#include "EarlyOptimizer.hpp"
#include "../../utils/casting.hpp"

using namespace IR;

std::vector<BasicBlock*> reverse_post_order(const std::unique_ptr<Function>& f)
{
    auto& entry = f->blocks_.front();

    std::unordered_set<BasicBlock*> visited;   
    std::vector<BasicBlock*> post_order;

    auto dfs = [&](this auto& self, BasicBlock* block) {
        if (visited.contains(block)) 
            return;
        visited.insert(block);
        for (auto* succ : block->successors())
            self(succ);
        post_order.push_back(block);
    };

    dfs(entry.get());
    std::ranges::reverse(post_order);
    return post_order;
}

std::vector<BasicBlock*> post_order(const std::unique_ptr<Function>& f)
{
    auto& entry = f->blocks_.front();

    std::unordered_set<BasicBlock*> visited;   
    std::vector<BasicBlock*> post_order;

    auto dfs = [&](this auto& self, BasicBlock* block) {
        if (visited.contains(block)) 
            return;
        visited.insert(block);
        for (auto* succ : block->successors())
            self(succ);
        post_order.push_back(block);
    };

    dfs(entry.get());
    return post_order;
}

bool escapes_via(Value* value, Value* target, std::unordered_set<Value*>& visited)
{
    if (visited.contains(value)) {
        return false;
    }

    visited.insert(value);

    switch (value->kind_) {
        case ValueKind::StoreInstVal: {
            auto* store = static_cast<StoreInst*>(value);
            return store->operands_[1] == target; // are we storing this alloca ptr, (since we are storing, we lost value flow because it has left ssa world)
        }

        case ValueKind::RetInstVal: {
            auto* ret = static_cast<RetInst*>(value);
            return ret->operands_[0] == target; // this is okay because this case is checking for returning the alloca itself, not a load of hte alloca, otherwise this operand would be a load instruction, not the alloca. the lowering engine will handle this case
        }

        case ValueKind::PtrAddVal: {
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

bool escapes(AllocaInst* alloca)
{
    std::unordered_set<Value*> v;

    for (auto* use : alloca->users_) {
        if (escapes_via(use, alloca, v)) {
            return true;
        }
    }

    return false;
}

std::unordered_set<AllocaInst*> non_escaping_allocas(const std::unique_ptr<Function>& f)
{
    std::unordered_set<AllocaInst*> allocas;
    for (auto& block : f->blocks_) {
        for (auto& inst : block->instructions_) {
            if (auto* alloca = dyn_cast<AllocaInst>(inst)) {
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
        case ValueKind::StoreInstVal: // mutating memory state is a side effect
        case ValueKind::CallInstVal:
        case ValueKind::RetInstVal:
        case ValueKind::BranchInstVal:
            return true;
        default:
            return false;
    }
}

void EarlyOptimizer::trivial_dce()
{
    for (auto& f : program_.functions_) {
        bool changed = true;
        while (changed) {
            changed = false;
            for (auto& block : f->blocks_) {
                for (auto it = block->instructions_.begin(); it != block->instructions_.end(); ) {
                    if ((*it)->users_.empty() && !may_have_side_effect(*it)) {
                        it = block->instructions_.erase(it);
                        changed = true;
                    } else {
                        ++it;
                    }
                }
            }
        }
    }
}

template <typename T>
concept LoadOrStore = std::same_as<T, LoadInst> || std::same_as<T, StoreInst>;

bool targets_same_alloca(LoadOrStore auto* inst1, LoadOrStore auto* inst2)
{
    return inst1->operands_[0] == inst2->operands_[0];
}

AllocaInst* get_alloca_operand(LoadOrStore auto* inst)
{
    if (inst) {
        return dyn_cast<AllocaInst>(inst->operands_[0]);
    }

    return nullptr;
}

void EarlyOptimizer::remove_dead_stores()
{   
    for (auto& f : program_.functions_) {
        auto alloca_set = non_escaping_allocas(f);
        for (auto& block : f->blocks_) {
            std::unordered_map<AllocaInst*, StoreInst*> last_store;
            std::vector<StoreInst*> to_delete;
            for (auto& inst : std::views::reverse(block->instructions_)) {
                if (auto* store = dyn_cast<StoreInst>(inst)) {
                    auto* a = get_alloca_operand(store);
                    if (alloca_set.contains(a)) {
                        if (last_store.contains(a)) {
                            to_delete.push_back(store);
                        }
                        last_store[a] = store;
                    }
                } else if (auto* load = dyn_cast<LoadInst>(inst)) {
                    auto* a = get_alloca_operand(load);
                    if (alloca_set.contains(a)) {
                        last_store.erase(a);
                    }
                }
            }

            block->instructions_.remove_if([&to_delete](std::unique_ptr<Instruction>& inst) {
                return std::ranges::contains(to_delete, inst.get());
            });
        }
    }
}
