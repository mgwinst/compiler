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

    PhiInstVal,
    
    IntLiteralVal,
    FloatLiteralVal,

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

    auto& users()
    {
        return use_list_;
    }

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

    std::string get_name() const
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


// ***************** LITERALS *****************


struct Literal : Value
{
    std::variant<int64_t, double> data_;

    Literal(int64_t v) : 
        Value{ValueKind::IntLiteralVal, std::to_string(v)},
        data_{ v } {}

    Literal(double v) : 
        Value{ValueKind::FloatLiteralVal, std::to_string(v)},
        data_{ v } {}
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

template <typename T>
concept DerivedFromInstruction = std::derived_from<T, Instruction>;

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


// ***************** BASIC BLOCK *****************


struct Terminator;

struct BasicBlock : Value
{
    BasicBlock(std::string_view name = "") :
        Value{ValueKind::BasicBlockVal, name} {}

    template <DerivedFromInstruction T>
    Instruction* insert(std::unique_ptr<T> value)
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

    auto successors();

    auto predecessors()
    {
        return use_list_ | std::views::transform([](Value* value) { return value->get_parent(); });
    }

private:
    std::list<std::unique_ptr<Value>> instructions_;

    Terminator* get_terminator();
};


// ***************** TERMINATOR *****************


struct Terminator : Instruction
{
    TerminatorKind terminator_kind_;   

    Terminator(Value* value) :
        Instruction{ValueKind::TerminatorVal, { value }},
        terminator_kind_{ TerminatorKind::Return } {}

    Terminator(BasicBlock* target) :
        Instruction{ValueKind::TerminatorVal, { static_cast<Value*>(target) }},
        terminator_kind_{ TerminatorKind::Branch } {}

    Terminator(Value* cond, BasicBlock* bb_true, BasicBlock* bb_false) :
        Instruction{ValueKind::TerminatorVal, {cond, static_cast<Value*>(bb_true), static_cast<Value*>(bb_false)}},
        terminator_kind_{ TerminatorKind::CondBranch } {}

    auto* condition()
    {   
        assert(terminator_kind_ == TerminatorKind::CondBranch);
        return operands_[0];
    }   

    auto targets()
    {
        assert(terminator_kind_ != TerminatorKind::Return);
        return operands_ | std::views::transform([](Value* value) { return static_cast<BasicBlock*>(value); });
    }
};

inline Terminator* BasicBlock::get_terminator()
{
    return static_cast<Terminator*>(instructions_.back().get());
}

inline auto BasicBlock::successors()
{
    return get_terminator()->targets();
}


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

    BasicBlock* insert(std::unique_ptr<BasicBlock> block)
    {
        auto* ptr = block.get();
        block->set_parent(this);
        blocks_.push_back(std::move(block));
        return ptr;
    }

    Argument* insert(std::unique_ptr<Argument> arg)
    {
        auto* ptr = arg.get();
        arg->set_parent(this);
        args_.push_back(std::move(arg));
        return ptr;
    }
};


// ***************** PROGRAM *****************


class Program
{
public:
    Function* insert(std::unique_ptr<Function> function)
    {
        auto* ptr = function.get();
        functions_.push_back(std::move(function));
        return ptr;
    }

    const auto& functions() const
    {
        return functions_;
    }

private:
    std::vector<std::unique_ptr<Function>> functions_;
};


} // namespace IR
