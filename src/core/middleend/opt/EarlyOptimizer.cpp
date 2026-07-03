#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <ranges>
#include <stack>

#include "EarlyOptimizer.hpp"
#include "utils/casting.hpp"
#include "middleend/analysis/escape.hpp"

bool may_have_side_effect(const std::unique_ptr<Value>& inst)
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
    std::vector<Value*> dead;

    for (auto& function : program_.functions_) {
        for (auto& block : function->blocks_) {
            for (auto it = block->instructions_.rbegin(); it != block->instructions_.rend(); ++it) {
                if ((*it)->users_.empty() && !may_have_side_effect(*it)) {
                    dead.push_back(it->get());
                }
            }

            block->instructions_.remove_if([&dead](std::unique_ptr<Value>& inst) {
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
        auto non_escaping = non_escaping_allocas(function.get());
        for (auto& block : function->blocks_) {
            for (auto& inst : std::views::reverse(block->instructions_)) {
                if (auto* store = dyn_cast<Store>(inst)) {
                    auto* a = get_alloca_operand(store);
                    if (non_escaping.contains(a)) {
                        if (last_store.contains(a)) {
                            dead.push_back(store);
                        }
                        last_store[a] = store;
                    }
                } else if (auto* load = dyn_cast<Load>(inst)) {
                    auto* a = get_alloca_operand(load);
                    if (non_escaping.contains(a)) {
                        last_store.erase(a);
                    }
                }
            }

            block->instructions_.remove_if([&dead](std::unique_ptr<Value>& inst) {
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
























// mem2reg


#include <queue>
#include "middleend/analysis/dominator.hpp"





/* 

handle trivial cases before full mem2reg

1. an alloca with a single store and multiple loads
2. an alloca whose loads are all in the same block as and dominated by the stores feeding them

*/

// also must run dead store elimination in case of immediately overwritten phi via store in block

// also must run dce after to clean up dead phi's b/c cheaper to over generate and cleanup

std::queue<BasicBlock*> def_blocks(Function* function, Alloca* alloca)
{
    std::queue<BasicBlock*> def_blocks;   

    for (auto& block : function->blocks_) {
        for (auto& inst : block->instructions_) {
            if (auto* store = dyn_cast<Store>(inst)) {
                if (get_alloca_operand(store) == alloca) {
                    def_blocks.push(block.get());
                    break;
                }
            }
        }
    }

    return def_blocks;
}

void rename(BasicBlock* block, std::unordered_map<Alloca*, std::stack<Value*>> def_stack, DominatorTree& dt) 
{
    std::unordered_map<Alloca*, std::size_t> sizes;
    for (auto [alloca, stack] : def_stack) {
        sizes[alloca] = stack.size();
    }
    
    std::vector<Value*> to_remove;

    for (auto& inst : block->instructions_) 
    {
        if (auto* phi = dyn_cast<Phi>(inst)) 
        {
            def_stack[phi->alloca_].push(phi);
        } 
        else if (auto* load = dyn_cast<Load>(inst)) 
        {
            auto* v = def_stack[get_alloca_operand(load)].top();
            inst->replace_uses_with(v);
            to_remove.push_back(load);
        } 
        else if (auto* store = dyn_cast<Store>(inst)) 
        {
            def_stack[get_alloca_operand(store)].push(store->operands_[1]);
            to_remove.push_back(store);
        }
    }

    block->instructions_.remove_if([&to_remove](auto& inst) { 
        return ranges::contains(to_remove, inst.get());
    });

    for (auto* succ : block->successors()) {
        for (auto& inst : succ->instructions_) {
            if (auto* phi = dyn_cast<Phi>(inst)) {
                phi->operands_.emplace_back(def_stack[phi->alloca_].top(), block);
            }
        }
    }

    for (auto* child : dt[block]) {
        rename(child, def_stack, dt);
    }

    for (auto [alloca, size] : sizes) {
        for (auto i = 0uz; i < size; ++i) {
            def_stack[alloca].pop();
        }
    } 
}

void promote_memory_to_register(Function* function, IRBuilder& builder) {   

    auto dt = DominatorTree{function};
    auto df = DominanceFrontier{function};

    auto promotable_allocas = non_escaping_allocas(function) | ranges::to<std::vector>();

    for (auto* alloca : promotable_allocas) {
        std::queue worklist = def_blocks(function, alloca);
        std::unordered_set<BasicBlock*> phi_blocks;

        while (!worklist.empty()) {
            auto* block = worklist.front();
            worklist.pop();
            for (auto* d : df[block]) {
                if (!phi_blocks.contains(d)) {
                    phi_blocks.insert(d);
                    worklist.push(d);
                }
            }
        }

        for (auto* block : phi_blocks) {
            auto* phi = builder.create<Phi>(alloca);
            block->instructions_.emplace_front(phi);
        }
    }

    std::unordered_map<Alloca*, std::stack<Value*>> def_stack;
    for (auto* alloca : promotable_allocas) {
        def_stack.emplace(alloca, std::stack<Value*>{ });
    }
    
    rename(function->get_entry_block(), def_stack, dt);

    // delete all promotable allocas after the rename walk
    function->get_entry_block()->instructions_.remove_if([&promotable_allocas](auto& inst) {
        return ranges::contains(promotable_allocas, inst.get());
    });
}

void EarlyOptimizer::mem2reg()
{
    for (auto& function : program_.functions_) {
        promote_memory_to_register(function.get(), builder_);
    }
}