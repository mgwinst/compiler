#pragma once

#include <concepts>
#include <cstdint>
#include <list>
#include <vector>
#include <string>

#include "../utils/alias.hpp"

namespace IR {

enum class ValueKind
{
    InstructionVal, // types of instructions also? Or should be an Op?
    ArgumentVal,
    BasicBlockVal,
    FunctionVal,
    ConstantIntVal,
    ConstantFloatVal,
    TerminatorVal
};

enum class Op : uint32_t
{
    Alloca,
    Load,
    Store,
    Phi,
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Shl,
    Shr,
};

struct Value
{
    ValueKind kind_;
    std::vector<Value*> use_list_;
    std::string name_;
    Value* parent_;

    Value(ValueKind kind) :
        kind_{ kind },
        use_list_{ },
        name_{ },
        parent_{ nullptr } {}
    
    void set_parent(Value* parent)
    {
        parent_ = parent;
    }

    void add_use(Value* value) 
    {
        use_list_.push_back(value);
    }
};

template <typename T>
concept DerivedFromValue = std::derived_from<T, Value>;

// arguments are at same level at bb, therefore parent is function not BB
struct Argument : Value
{
    Argument() :
        Value{ ValueKind::ArgumentVal } {}
};

struct BasicBlock : Value
{
    std::list<Value*> instructions_;

    BasicBlock() :
        Value{ ValueKind::BasicBlockVal } {}

    template <DerivedFromValue T>
    Value* insert(T* val)
    {
        instructions_.push_back(static_cast<Value*>(val));
        return val;
    }
};

// use list is updated during call instruction
struct Function : Value
{
    std::vector<Value*> args_;
    std::vector<Value*> blocks_;

    Function() :
        Value{ ValueKind::FunctionVal } {}

    void add_argument(Argument* arg)
    {
        args_.push_back(arg);
        arg->set_parent(this);
    }

    void add_block(BasicBlock* block)
    {
        blocks_.push_back(block);
        block->set_parent(this);
    }
};

// instruction values always wire operands' use list with this instruction object
struct Instruction : Value
{
    Op op_;
    std::vector<Value*> operands_;

    template <std::same_as<Value*>... Ts>
    Instruction(Op op, Ts... operands) :
        Value{ ValueKind::InstructionVal },
        op_{ op },
        operands_{ operands... } 
    {
        for (auto* operand : operands_)
            operand->add_use(this);
    }
};

struct Program
{
    void append(IR::Value* value) // right now only functions (value = function)
    {
        values_.push_back(value);
    }

    const auto& functions() const
    {
        return values_;
    }

    std::vector<Value*> values_;
};
    
} // namespace IR
