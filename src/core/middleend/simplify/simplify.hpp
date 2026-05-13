#include <algorithm>
#include <ranges>
#include <boost/dynamic_bitset/dynamic_bitset.hpp>

#include "../ir/Value.hpp"
#include "../../utils/casting.hpp"

using namespace IR;

inline std::vector<BasicBlock*> reverse_post_order(const std::unique_ptr<Function>& f)
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

inline bool escapes_via(Value* value, Value* target, std::unordered_set<Value*>& visited)
{
    if (visited.contains(value)) {
        return false;
    }

    visited.insert(value);

    switch (value->get_kind()) {
        case ValueKind::StoreInstVal: {
            auto* store = static_cast<StoreInst*>(value);
            return store->operands_[1] == target; // are we storing this alloca ptr, (since we are storing, we lost value flow because it has left ssa world)
        }

        case ValueKind::TerminatorVal: { // ex. of terminator being clunky (fix)
            auto* term = static_cast<Terminator*>(value);
            if (term->is_return()) {
                return term->operands_[0] == target; // this is okay because this case is checking for returning the alloca itself, not a load of hte alloca, otherwise this operand would be a load instruction, not the alloca. the lowering engine will handle this case
            }
        }

        case ValueKind::PtrAddVal: {
            auto* ptradd = static_cast<PtrAdd*>(value);
            if (ptradd->operands_[0] == target) {
                for (auto* use : ptradd->users()) {
                    if (escapes_via(use, ptradd, visited)) {
                        return true;
                    }
                }
            }
        }

        // call(&alloca) ... 

        default: {
            return false;
        }
    }
}

inline bool escapes(AllocaInst* alloca)
{
    std::unordered_set<Value*> v;

    for (auto* use : alloca->users()) {
        if (escapes_via(use, alloca, v)) {
            return true;
        }
        v.clear();
    }

    return false;
}

inline std::unordered_set<AllocaInst*> non_escaping_allocas(const std::unique_ptr<Function>& f)
{
    std::unordered_set<AllocaInst*> allocas;
    for (auto* block : reverse_post_order(f)) {
        for (auto& inst : block->instruction_list()) {
            if (auto* alloca = dyn_cast<AllocaInst>(inst)) {
                if (escapes(alloca) == false) {
                    allocas.insert(alloca);
                }
            }
        }
    }
    return allocas;
}

// what instructions are obviously side effect free with little to no analysis?

// branch
// return
// call

inline bool may_have_side_effect(std::unique_ptr<Instruction>& inst)
{
    switch (inst->get_kind()) {
        case ValueKind::StoreInstVal: // mutating memory state is a side effect
        case ValueKind::CallInstVal:
        case ValueKind::TerminatorVal:
            return true;
        default:
            return false;
    }
}

inline void trivial_dce(Program& program)
{
    for (auto& f : program.functions()) {
        bool changed = true;
        while (changed) {
            changed = false;
            for (auto& block : f->blocks_) {
                for (auto it = block->instruction_list().begin(); it != block->instruction_list().end(); ) {
                    if ((*it)->no_users() && !may_have_side_effect(*it)) {
                        it = block->instruction_list().erase(it);
                        changed = true;
                    } else {
                        ++it;
                    }
                }
            }
        }
    }
}

// pre-ssa simplification and cleanup
inline void simplify(Program& program)
{
    trivial_dce(program);
}