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

#include "boost/container/small_vector.hpp"

#include "../../frontend/sema/types/types.hpp"
#include "../../utils/alias.hpp"
#include "../../utils/utils.hpp"
#include "../../utils/enums.hpp"
#include "../../utils/cast_range.hpp"

using boost::container::small_vector;

inline constexpr TypeID no_type = -1;

namespace IR {

enum class ValueKind : uint32_t
{
    Invalid,
    FunctionVal,
    BasicBlockVal,
    ArgumentVal,
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
    CallInstVal,
    PtrAddVal,
    RetInstVal,
    BranchInstVal,
    PhiInstVal,
    IntLiteralVal,
    FloatLiteralVal,
};


// ***************** VALUE *****************


struct Value
{
    ValueKind kind_;
    TypeID type_id_;
    std::string name_;
    std::vector<Value*> users_;
    Value* parent_;

    Value(ValueKind kind, TypeID type_id = no_type, std::string_view name = "") :
        kind_{ kind },
        type_id_{ type_id },
        name_{ name },
        users_{ },
        parent_{ nullptr } {}

    virtual ~Value() = default;

    void set_parent(Value* parent)
    {
        parent_ = parent;
    }

    void add_use(Value* value) 
    {
        users_.push_back(value);
    }
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
    small_vector<Value*, 2> operands_;

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
                std::erase(operand->users_, this);
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
    LoadInst(Value* ptr) :
        Instruction{ValueKind::LoadInstVal, {ptr}, ptr->type_id_} {}

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

struct CallInst : Instruction
{

};

struct RetInst : Instruction
{
    RetInst(Value* value) :
        Instruction{ValueKind::RetInstVal, { value }} {}
};

struct BasicBlock;

struct BranchInst : Instruction
{      
    // will any DerivedFromValue* bind to the block pointer parameters? 
    BranchInst(BasicBlock* target);
    BranchInst(Value* cond, BasicBlock* bb_true, BasicBlock* bb_false);

    bool is_conditional() const
    {
        return branch_kind_ == BranchKind::Conditional;
    }

    auto condition()
    {
        assert(is_conditional());

        return operands_[0];
    }

    auto targets()
    {
        return is_conditional() ? std::span{operands_}.subspan(1) : std::span{operands_};
    }

private:
    BranchKind branch_kind_;
};


// don't need to type this instruction,
// it is just an opaque ptr, we know what it means from itself and first operand
// essentially just index / address calculation

struct PtrAdd : Instruction
{
    PtrAdd(Value* ptr, Value* offset) :
        Instruction{ValueKind::PtrAddVal, {ptr, offset}} {}
};

// should inherit from value instead of instruction? because of the std::pair<value, block> operands?
// struct Phi : Value
struct PhiInst : Instruction
{
    // std::initializer_list<std::pair<Value*, BasicBlock*>> [value, block], [value, block], ... 
};


// ***************** BASIC BLOCK *****************


struct BasicBlock : Value
{
    std::list<std::unique_ptr<Instruction>> instructions_;

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

    Instruction* terminator()
    {
        if (instructions_.empty())
            return nullptr;

        assert(instructions_.back()->kind_ == ValueKind::RetInstVal ||
               instructions_.back()->kind_ == ValueKind::BranchInstVal);

        return instructions_.back().get();
    }

    auto successors();

    auto predecessors()
    {
        return users_ | std::views::transform([](Value* value) { return static_cast<BasicBlock*>(value->parent_); });
    }
};


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
    
    // memory leak
    Value* return_value_ = nullptr;
    BasicBlock* return_block_ = nullptr;

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


class ConstantPool
{
public:
    template <typename T>
    auto try_insert(T literal)
    {
        return get_container<T>().try_emplace(literal, std::make_unique<Literal>(literal));
    }

private:
    std::unordered_map<int64_t, std::unique_ptr<Literal>> integer_pool_;

    template <typename T>
    auto& get_container()
    {
        if constexpr (std::same_as<T, int64_t>)
            return integer_pool_;
        else
            static_assert(always_false_v<T>, "T is not an internable type_id");
    }
};

struct Program
{
    ConstantPool constant_pool_;
    std::list<std::unique_ptr<Function>> functions_;

    Function* insert(Function* function)
    {
        functions_.push_back(std::unique_ptr<Function>(function));
        return function;
    }
};














// some out of line definitions (circular deps)


inline BranchInst::BranchInst(BasicBlock* target) : 
    Instruction{ValueKind::BranchInstVal, {static_cast<Value*>(target)}},
    branch_kind_{BranchKind::Unconditional} {}

inline BranchInst::BranchInst(Value* cond, BasicBlock* bb_true, BasicBlock* bb_false) :
    Instruction{ValueKind::BranchInstVal, {cond, static_cast<Value*>(bb_true), static_cast<Value*>(bb_false)}},
    branch_kind_{BranchKind::Conditional} {}

inline auto BasicBlock::successors()
{
    small_vector<BasicBlock*, 2> result{};   

    auto* term = terminator();
    if (!term)
        return result;

    if (term->kind_ != ValueKind::BranchInstVal)
        return result;

    auto* branch = static_cast<BranchInst*>(term);

    for (auto target : static_cast_view<BasicBlock>(branch->targets()))
        result.push_back(target);

    return result;
}

} // namespace IR