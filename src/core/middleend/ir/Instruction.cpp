#include "IR.hpp"

Instruction::Instruction(ValueKind kind, std::vector<Value*> operands, Type* type, std::string_view name) : 
    Value{kind, type, name}
{
    operands_ = operands;
    for (auto* arg : operands_) {
        arg->add_use(this);
    }
}

Instruction::~Instruction()
{
    for (auto* operand : operands_) {
        if (operand) {
            if (auto it = ranges::find(operand->users_, this); it != operand->users_.end()) {
                swap_pop(operand->users_, it);
            }
        }
    }
}

Alloca::Alloca(Type* type, std::string_view name) :
    Instruction{ValueKind::Alloca, {}, type, name} {}

Load::Load(Type* type, Value* ptr) :
    Instruction{ValueKind::Load, {ptr}, type} {}

Load::Load(Value* ptr) :
    Instruction{ValueKind::Load, {ptr}} {}

Store::Store(Value* dst, Value* src) :
    Instruction{ValueKind::Store, {dst, src}} {}

Add::Add(Value* src1, Value* src2) :
    Instruction{ValueKind::Add, {src1, src2}} {}

Sub::Sub(Value* src1, Value* src2) :
    Instruction{ValueKind::Sub, {src1, src2}} {}

Mul::Mul(Value* src1, Value* src2) :
    Instruction{ValueKind::Mul, {src1, src2}} {}

Div::Div(Value* src1, Value* src2) :
    Instruction{ValueKind::Div, {src1, src2}} {}

Eq::Eq(Value* src1, Value* src2) :
    Instruction{ValueKind::Eq, {src1, src2}} {}

Ne::Ne(Value* src1, Value* src2) :
    Instruction{ValueKind::Ne, {src1, src2}} {}

Slt::Slt(Value* src1, Value* src2) :
    Instruction{ValueKind::Slt, {src1, src2}} {}

Call::Call(Function* callee, std::vector<Value*> args) :
    Instruction{ValueKind::Call, prepend(static_cast<Value*>(callee), std::move(args))} {}

Return::Return(Value* value) :
    Instruction{ValueKind::Return, {value}} {}

// don't need to type this instruction,
// it is just an opaque ptr, we know what it means from itself and first operand
PtrAdd::PtrAdd(Value* ptr, Value* offset) :
    Instruction{ValueKind::PtrAdd, {ptr, offset}} {}

Branch::Branch(BasicBlock* target) : 
    Instruction{ValueKind::Branch, {static_cast<Value*>(target)}},
    branch_kind_{BranchKind::Unconditional} {}

Branch::Branch(Value* cond, BasicBlock* bb_true, BasicBlock* bb_false) :
    Instruction{ValueKind::Branch, {cond, static_cast<Value*>(bb_true), static_cast<Value*>(bb_false)}},
    branch_kind_{BranchKind::Conditional} {}

bool Branch::is_conditional() const
{
    return branch_kind_ == BranchKind::Conditional;
}

Value* Branch::condition()
{
    assert(is_conditional());

    return operands_[0];
}

std::span<Value*> Branch::targets()
{
    return is_conditional() ? std::span{operands_}.subspan(1) : std::span{operands_};
}