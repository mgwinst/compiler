#include "escape.hpp"

bool escapes_via(Value* value, Value* target, std::unordered_set<Value*>& visited)
{
    if (visited.contains(value)) {
        return false;
    }

    visited.insert(value);

    switch (value->kind_) {
        case ValueKind::Store: {
            auto* store = cast<Store>(value);
            return store->operands_[1] == target; // are we storing this alloca ptr, (since we are storing, we lost value flow because it has left ssa world)
        }

        case ValueKind::Return: {
            auto* ret = cast<Return>(value);
            return ret->operands_[0] == target; // this is okay because this case is checking for returning the alloca itself, not a load of hte alloca, otherwise this operand would be a load instruction, not the alloca. the lowering engine will handle this case
        }

        case ValueKind::PtrAdd: {
            auto* ptradd = cast<PtrAdd>(value);
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

std::unordered_set<Alloca*> non_escaping_allocas(Function* function)
{
    std::unordered_set<Alloca*> allocas;

    for (auto& block : function->blocks_) {
        for (auto& inst : block->instructions_) {
            if (auto* alloca = dyn_cast<Alloca>(inst)) {
                if (!escapes(alloca)) {
                    allocas.insert(alloca);
                }
            }
        }
    }

    return allocas;
}