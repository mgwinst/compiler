
/*


#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <ranges>

#include "EarlyOptimizer.hpp"
#include "../../utils/casting.hpp"

std::vector<BasicBlock*> post_order(const std::unique_ptr<Function>& function, bool reverse_order = false)
{
    auto& entry = function->blocks_.front();

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

    if (reverse_order)
        std::ranges::reverse(post_order);

    return post_order;
}

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
    for (auto& function : program_.functions_) {
        bool changed = true;
        while (changed) {
            changed = false;
            for (auto& block : function->blocks_) {
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
concept LoadOrStore = std::same_as<T, Load> || std::same_as<T, Store>;

bool targets_same_alloca(LoadOrStore auto* inst1, LoadOrStore auto* inst2)
{
    return inst1->operands_[0] == inst2->operands_[0];
}

Alloca* get_alloca_operand(LoadOrStore auto* inst)
{
    if (inst) {
        return dyn_cast<Alloca>(inst->operands_[0]);
    }

    return nullptr;
}

void EarlyOptimizer::remove_dead_stores()
{   
    for (auto& function : program_.functions_) {
        auto alloca_set = non_escaping_allocas(function);
        for (auto& block : function->blocks_) {
            std::unordered_map<Alloca*, Store*> last_store;
            std::vector<Store*> to_delete;
            for (auto& inst : std::views::reverse(block->instructions_)) {
                if (auto* store = dyn_cast<Store>(inst)) {
                    auto* a = get_alloca_operand(store);
                    if (alloca_set.contains(a)) {
                        if (last_store.contains(a)) {
                            to_delete.push_back(store);
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

            block->instructions_.remove_if([&to_delete](std::unique_ptr<Instruction>& inst) {
                return std::ranges::contains(to_delete, inst.get());
            });
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

void collapsible(BasicBlock* block1, BasicBlock* block2)
{

}

void combine_blocks()
{
    
}

// instructions new parent is the collapsed block

void EarlyOptimizer::merge_blocks()
{
    for (auto& function : program_.functions_) {

        auto& blocks = function->blocks_;

        for (auto it = blocks.begin(); it != blocks.end(); ) {
            if ((*it)->instructions_.empty()) {
                it = function->blocks_.erase(it); // pred.terminator.branch.operand[...] = nullptr for pred in block->preds()
            } else {
                if (double_branch(*it)) {
                    (*it)->instructions_.pop_back();
                }
                ++it;
            }
        }
        
        bool changed = true;
        while (changed) {
            changed = false;
            for (auto it = blocks.begin(); it != blocks.end(); ) {
                /*
                
                if collapsible(it, it->succ())
                    it.merge(it->succ())

                

                if ((*it)->successors().size() == 1) {
                    auto* succ = (*it)->successors()[0];
                    if (succ->predecessors().size() == 1) {

                        (*it)->instructions_.splice((*it)->instructions_.end(), succ->instructions_);



                        changed = true;
                    }
                    
                    
                    
                }               

                // if block.succ() == 1 && succ().pred() == 1
                //     block.remove(branch)
                //     block.append(succ.instructions)
            }

            // post order rewiring?
        }
        
    }
}


// if empty block : remove block
// if block as one succ and that succ has one pred : delete terminator and insert range pred into end of this block
// if 

*/