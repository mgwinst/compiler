#pragma once

#include "middleend/ir/IR.hpp"
#include "casting.hpp"

inline int value_count = 0;

inline bool is_nameable(Value* value)
{
    return value->kind_ >= ValueKind::Load && value->kind_ <= ValueKind::PtrAdd;
}

inline void name_values(Program& program)
{
    for (auto& function : program.functions_) {
        value_count = 0;
        for (auto& block : function->blocks_) {
            for (auto& inst : block->instructions_) {
                if (is_nameable(inst.get())) {
                    inst->name_ = std::to_string(value_count++);
                }
            }
        }
    }
}