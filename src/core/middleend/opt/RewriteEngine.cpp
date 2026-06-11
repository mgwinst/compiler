#include "RewriteEngine.hpp"

Value* fold(std::unique_ptr<Instruction>& inst, IRBuilder& builder)
{
    auto a = static_cast<Const*>(inst->operands_[0])->data_;
    auto b = static_cast<Const*>(inst->operands_[1])->data_;

    switch (inst->kind_) {
        case ValueKind::Add: {
            return builder.get_or_create_literal(a + b);
        }

        case ValueKind::Sub: {
            return builder.get_or_create_literal(a - b);
        }

        case ValueKind::Mul: {
            return builder.get_or_create_literal(a * b);
        }

        case ValueKind::Div: {
            if (b != 0)
                return builder.get_or_create_literal(a / b);
        }

        default:
            return nullptr;
    }
}

void RewriteEngine::constant_folding(std::unique_ptr<BasicBlock>& block)
{
    auto math_op = [](auto& inst) -> bool {
        return inst->kind_ >= ValueKind::Add && inst->kind_ <= ValueKind::Slt;
    };

    auto foldable = [&](auto& inst) -> bool { 
        return math_op(inst) && isa<Const>(inst->operands_[0]) && isa<Const>(inst->operands_[1]);
    };

    for (auto it = block->instructions_.begin(); it != block->instructions_.end(); ) {
        auto& inst = *it;
        if (foldable(inst)) {
            if (auto* fold_value = fold(inst, builder_))
                (*it)->replace_uses_with(fold_value);
            it = block->instructions_.erase(it);
        } else {
            ++it;
        }
    }
}

void RewriteEngine::strength_reduction(std::unique_ptr<BasicBlock>& block)
{

}