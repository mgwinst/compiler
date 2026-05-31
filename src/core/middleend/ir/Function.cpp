#include "IR.hpp"

Argument::Argument(TypeID type_id, std::string_view name) :
    Value{ValueKind::Argument, type_id, name} {}

Function::Function(std::string_view name) :
    Value{ValueKind::Function, none, name} {}

Function::~Function()
{
    for (Instruction* user : static_cast_view<Instruction>(users_)) {
        for (auto* operand : user->operands_) {
            if (operand == this) {
                operand = nullptr;
            }
        }
    }
}

BasicBlock* Function::insert(BasicBlock* block)
{
    block->parent_ = this;
    blocks_.push_back(std::unique_ptr<BasicBlock>{ block });
    return block;
}

Argument* Function::insert(Argument* arg)
{
    arg->parent_ = this;
    args_.push_back(std::unique_ptr<Argument>{ arg });
    return arg;
}

void Function::initialize_return(Value* return_value, BasicBlock* return_block)
{
    return_value_ = return_value;
    return_block_ = return_block;
}