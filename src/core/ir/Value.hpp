#pragma once

#include <cstdint>
#include <memory>
#include <initializer_list>
#include <vector>
#include <string_view>
#include <concepts>
#include <list>
#include <string>
#include <ranges>

#include "../utils/alias.hpp"
#include "../utils/utils.hpp"

namespace IR {

enum class ValueKind : uint32_t
{
    Invalid,
    FunctionVal,
    BasicBlockVal,
    ArgumentVal,

    // Instruction, instead testing if value, is instruction should be an enum range check
    AllocaInstVal,
    StoreInstVal,
    LoadInstVal,

    AddInstVal,
    SubInstVal,
    MulInstVal,
    DivInstVal,

    EqInstVal,
    NeInstVal,
    SltInstVal,
    BranchInstVal,

    PhiInstVal,
    
    ConstantIntVal,
    ConstantFloatVal,

    TerminatorVal,

};


// ***************** VALUE *****************


// Everything in the IR is a "Value" (similar to LLVM architecture)

class Value
{
public:
    Value(ValueKind kind, std::string_view name = "") :
        kind_{ kind },
        name_{ name },
        use_list_{ },
        parent_{ nullptr } {}

    virtual ~Value() = default;

    void set_parent(Value* parent)
    {
        parent_ = parent;
    }

    void add_use(Value* value) 
    {
        use_list_.push_back(value);
    }

    void set_name(std::string_view name) 
    {
        name_ = name;
    }

    ValueKind get_kind() const 
    {
        return kind_;
    }

    std::string get_name()
    {
        return name_;
    }

    auto* get_parent()
    {
        return parent_;
    }

protected:
    ValueKind kind_;
    std::string name_;
    std::vector<Value*> use_list_;
    Value* parent_;
};

template <typename T>
concept DerivedFromValue = std::derived_from<T, Value>;


// ***************** CONSTANTS *****************


struct ConstantInt : Value
{

};

struct ConstantFloat : Value
{

};


// ***************** INSTRUCTIONS *****************


struct Instruction : public Value
{
    std::vector<Value*> operands_;

    Instruction(ValueKind kind,
                std::initializer_list<Value*> operands,
                std::string_view name = "")
        : Value{kind, name}
    {
        operands_ = operands;
        for (auto* operand : operands_)
            operand->add_use(this);
    }
};

struct AllocaInst : Instruction
{
    TypeID type_id_;

    AllocaInst(TypeID type_id, std::string_view name) :
        Instruction{ValueKind::AllocaInstVal, {}, name},
        type_id_{ type_id } {}
};

struct LoadInst : Instruction
{
    LoadInst(Value* ptr) :
        Instruction{ValueKind::LoadInstVal, {ptr}} {}
};

struct StoreInst : Instruction
{
    StoreInst(Value* dst, Value* src) :
        Instruction{ValueKind::StoreInstVal, {dst, src}} {}
};

struct AddInst : Instruction
{
    AddInst(Value* src1, Value* src2) :
        Instruction{ValueKind::AddInstVal, {src1, src2}} {}
};

struct SubInst : Instruction
{
    SubInst(Value* src1, Value* src2) :
        Instruction{ValueKind::SubInstVal, {src1, src2}} {}
};

struct MulInst : Instruction
{
    MulInst(Value* src1, Value* src2) :
        Instruction{ValueKind::MulInstVal, {src1, src2}} {}
};

struct DivInst : Instruction
{
    DivInst(Value* src1, Value* src2) :
        Instruction{ValueKind::DivInstVal, {src1, src2}} {}
};

struct EqInst : Instruction
{
    EqInst(Value* src1, Value* src2) :
        Instruction{ValueKind::EqInstVal, {src1, src2}} {}
};

struct NeInst : Instruction
{
    NeInst(Value* src1, Value* src2) :
        Instruction{ValueKind::NeInstVal, {src1, src2}} {}
};

struct SltInst : Instruction
{
    SltInst(Value* src1, Value* src2) :
        Instruction{ValueKind::SltInstVal, {src1, src2}} {}
};

// should inherit from value instead of instruction? because of the std::pair<value, block> operands?
struct PhiInst : Instruction
{
    // std::initializer_list<std::pair<Value*, BasicBlock*>> [value, block], [value, block], ...
};

// ***************** TERMINATOR *****************

struct BasicBlock;

template <typename T>
concept BasicBlock_t = std::same_as<T, BasicBlock>;


struct Terminator : Instruction
{
    TerminatorKind terminator_kind_;   

    template <BasicBlock_t... Ts>
    Terminator(TerminatorKind terminator_kind, Value* arg, Ts*... labels) : // this breaks on ret because no labels on ret terminator instruction
        Instruction{ValueKind::TerminatorVal, {arg, static_cast<Value*>(labels)...}},
        terminator_kind_{ terminator_kind } {}

    auto condition()
    {
        return operands_[0];
    }   

    // these getters are wrong because unconditional branch doesn't have cond in first slot

    auto targets()
    {
        return operands_ | std::views::drop(1);
    }
};


// ***************** BASIC BLOCK *****************


struct BasicBlock : Value
{
    BasicBlock(std::string_view name = "") :
        Value{ValueKind::BasicBlockVal, name} {}

    template <DerivedFromValue T>
    Value* insert(std::unique_ptr<T> value)
    {
        auto* ptr = value.get();
        value->set_parent(this);
        instructions_.push_back(std::move(value));
        return ptr;
    }

    auto& instruction_list()
    {
        return instructions_;
    }

    auto successors()
    {
        return get_terminator()->targets();
    }

    auto predecessors()
    {
        return use_list_ | std::views::transform([](Value* value) { return value->get_parent(); });
    }

private:
    std::list<std::unique_ptr<Value>> instructions_;

    Terminator* get_terminator()
    {
        return static_cast<Terminator*>(instructions_.back().get());
    }
};
 

// ***************** FUNCTIONS *****************


// parent -> function, not basic block
struct Argument : Value
{
    Argument() :
        Value{ ValueKind::ArgumentVal } {}
};

// function use list is updated during call
struct Function : Value
{
    std::vector<std::unique_ptr<Argument>> args_;
    std::vector<std::unique_ptr<BasicBlock>> blocks_;

    Function() :
        Value{ ValueKind::FunctionVal } {}

    void add_argument(std::unique_ptr<Argument> arg)
    {
        arg->set_parent(this);
        args_.push_back(std::move(arg));
    }

    void add_block(std::unique_ptr<BasicBlock> block)
    {
        block->set_parent(this);
        blocks_.push_back(std::move(block));
    }
};


// ***************** PROGRAM *****************


class Program
{
public:
    void insert(std::unique_ptr<Function> function) // right now only functions (value = function)
    {
        functions_.push_back(std::move(function));
    }

    const auto& functions() const
    {
        return functions_;
    }

private:
    std::vector<std::unique_ptr<Function>> functions_;
};


} // namespace IR
