#pragma once

#include <initializer_list>
#include <string_view>
#include <cstdint>
#include <concepts>
#include <memory>
#include <vector>
#include <list>
#include <string>
#include <ranges>
#include <cassert>

#include "../../frontend/sema/types/types.hpp"
#include "../../utils/alias.hpp"
#include "../../utils/utils.hpp"
#include "../../utils/enums.hpp"

inline constexpr TypeID no_type = -1;

namespace IR {

enum class ValueKind : uint32_t
{
    Invalid,
    FunctionVal,
    BasicBlockVal,
    ArgumentVal,

    AllocaInstVal,
    LoadInstVal,
    StoreInstVal,
    AddInstVal,
    SubInstVal,
    MulInstVal,
    DivInstVal,
    EqInstVal,
    NeInstVal,
    SltInstVal,
    CallInstVal, // add struct
    PtrAddVal,
    PhiInstVal,
    TerminatorVal, // is instruction
    
    // LiteralVal
    IntLiteralVal,
    FloatLiteralVal,
};


// ***************** VALUE *****************


// Everything in the IR is a "Value" (similar to LLVM architecture)

class Value
{
public:
    Value(ValueKind kind, TypeID type_id = no_type, std::string_view name = "") :
        kind_{ kind },
        type_id_{ type_id },
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
        assert(kind_ != ValueKind::Invalid);
        return kind_;
    }

    TypeID get_type_id()
    {
        // assert(type_id_ != no_type);
        return type_id_;
    }

    std::string get_name() const
    {
        return name_;
    }

    auto* get_parent()
    {
        return parent_;
    }

    bool no_users() const
    {
        return use_list_.empty();
    }

protected:
    ValueKind kind_;
    TypeID type_id_;
    std::string name_;
    std::vector<Value*> use_list_;
    Value* parent_;
};

template <typename T>
concept DerivedFromValue = std::derived_from<T, Value>;


// ***************** LITERALS *****************


// types are wrong here
struct Literal : Value
{
    std::variant<int64_t, double> data_;

    // this will cause error mismatch between accepted literal and internal type
    Literal(int64_t v) : 
        Value{ValueKind::IntLiteralVal, INT32, std::to_string(v)+"L"},
        data_{ v } {}

    Literal(double v) : 
        Value{ValueKind::FloatLiteralVal, FLOAT32, std::to_string(v)+"L"},
        data_{ v } {}
};


// ***************** INSTRUCTIONS *****************


struct Instruction : public Value
{
    std::vector<Value*> operands_;

    Instruction(ValueKind kind,
                std::initializer_list<Value*> operands,
                TypeID type_id = no_type,
                std::string_view name = "")
        : Value{kind, type_id, name}
    {
        operands_ = operands;
        for (auto* operand : operands_) {
            operand->add_use(this);
        }
    }

    ~Instruction() override
    {
        for (auto* operand : operands_) {
            if (operand)
                std::erase(operand->users(), this);
        }
    }
};

template <typename T>
concept DerivedFromInstruction = std::derived_from<T, Instruction>;

struct AllocaInst : Instruction
{
    AllocaInst(TypeID type_id, std::string_view name) :
        Instruction{ValueKind::AllocaInstVal, {}, type_id, name} {}
};

struct LoadInst : Instruction
{
    LoadInst(TypeID type_id, Value* ptr) :
        Instruction{ValueKind::LoadInstVal, {ptr}, type_id} {}
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


// ***************** PTRADD *****************


// don't need to type this instruction,
// it is just an opaque ptr, we know what it means from itself and first operand

// essentially just index / address calculation

struct PtrAdd : Instruction
{
    PtrAdd(Value* ptr, Value* offset) :
        Instruction{ValueKind::PtrAddVal, {ptr, offset}} {}
};


// ***************** BASIC BLOCK *****************


struct Terminator;

struct BasicBlock : Value
{
    BasicBlock(std::string_view name = "") :
        Value{ValueKind::BasicBlockVal, no_type, name} {}
    
    // drop_all_references()
    ~BasicBlock() override
    {
        while (!instructions_.empty()) {
            instructions_.pop_back();
        }
    }

    template <DerivedFromInstruction T>
    T* insert(T* value)
    {
        value->set_parent(this);
        instructions_.push_back(std::unique_ptr<Instruction>{ value });
        return value;
    }

    auto& instruction_list()
    {
        return instructions_;
    }

    auto successors();

    // inheritance should be private so that you can't call users() on a basic block ptr
    auto predecessors()
    {
        return users() | std::views::transform([](Value* value) { return static_cast<BasicBlock*>(value->get_parent()); });
    }

    Terminator* get_terminator();

private:
    std::list<std::unique_ptr<Instruction>> instructions_;
};


// ***************** TERMINATOR *****************


struct Terminator : Instruction
{
    TerminatorKind terminator_kind_;

    // is Value* and BasicBlock* overload dangerous? Will Value* always be matched?
    // is this where a dyn_cast<T> is appropriate? would that work though with Terminator = T?
    // actual dynamic_cast?

    Terminator(Value* value) :
        Instruction{ValueKind::TerminatorVal, { value }},
        terminator_kind_{ TerminatorKind::Return } {}

    Terminator(BasicBlock* target) :
        Instruction{ValueKind::TerminatorVal, { static_cast<Value*>(target) }},
        terminator_kind_{ TerminatorKind::Branch } {}

    Terminator(Value* cond, BasicBlock* bb_true, BasicBlock* bb_false) :
        Instruction{ValueKind::TerminatorVal, {cond, static_cast<Value*>(bb_true), static_cast<Value*>(bb_false)}},
        terminator_kind_{ TerminatorKind::CondBranch } {}


    // this feels wrong to enforce correct polymorphic behavior with assert(tag)?

    auto* condition()
    {   
        assert(terminator_kind_ == TerminatorKind::CondBranch);

        return operands_[0];
    }   

    auto targets()
    {
        assert(terminator_kind_ != TerminatorKind::Return);
 
        auto operands = terminator_kind_ == TerminatorKind::CondBranch ?
            operands_ | std::views::drop(1) :
            operands_ | std::views::drop(0);

        return operands | std::views::transform([](Value* value) { return static_cast<BasicBlock*>(value); });
    }

    bool is_return() const 
    {
        return terminator_kind_ == TerminatorKind::Return;
    }
};

inline Terminator* BasicBlock::get_terminator()
{
    return static_cast<Terminator*>(instructions_.back().get());
}

inline auto BasicBlock::successors()
{
    auto t = get_terminator();

    if (t->terminator_kind_ == TerminatorKind::Return)
        return std::vector<BasicBlock*>{};
    else
        return t->targets() | std::ranges::to<std::vector<BasicBlock*>>();
}


// ***************** FUNCTIONS *****************


// parent -> function, not basic block
struct Argument : Value
{
    Argument(TypeID type_id = no_type, std::string_view name = "") :
        Value{ ValueKind::ArgumentVal, type_id, name} {}
};

// function use list is updated during call
struct Function : Value
{
    std::list<std::unique_ptr<Argument>> args_;
    std::list<std::unique_ptr<BasicBlock>> blocks_;
    IR::Value* return_value_ = nullptr;
    IR::BasicBlock* return_block_ = nullptr;

    Function(std::string_view name = "") :
        Value{ ValueKind::FunctionVal, no_type, name} {}

    BasicBlock* insert(BasicBlock* block)
    {
        block->set_parent(this);
        blocks_.push_back(std::unique_ptr<BasicBlock>{ block });
        return block;
    }

    Argument* insert(Argument* arg)
    {
        arg->set_parent(this);
        args_.push_back(std::unique_ptr<Argument>{ arg });
        return arg;
    }

    void initialize_return(IR::Value* return_value, IR::BasicBlock* return_block)
    {
        return_value_ = return_value;
        return_block_ = return_block;
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

    auto& functions()
    {
        return functions_;
    }

    auto& constants()
    {
        return constant_pool_;
    }

private:
    ConstantPool constant_pool_;
    std::list<std::unique_ptr<Function>> functions_;
};


} // namespace IR
