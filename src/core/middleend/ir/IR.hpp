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
#include <utility>
#include <cassert>

#include "boost/container/small_vector.hpp"
#include "boost/intrusive/list.hpp"

#include "../../frontend/sema/types/types.hpp"
#include "../../utils/alias.hpp"
#include "../../utils/utils.hpp"
#include "../../utils/enums.hpp"
#include "../../utils/cast_range.hpp"
#include <algorithm>

using boost::container::small_vector;
using boost::intrusive::list;

namespace ranges = std::ranges;
namespace views = std::views;

inline constexpr TypeID none = -1;

enum class ValueKind
{
    Invalid,
    Function,
    BasicBlock,
    Argument,
    Alloca,
    Load,
    Store,
    Add,
    Sub,
    Mul,
    Div,
    Eq,
    Ne,
    Slt,
    Call,
    PtrAdd,
    Return,
    Branch,
    Phi,
    Const,
};

struct Value : boost::intrusive::list_base_hook<>
{
    ValueKind kind_;
    TypeID type_id_;
    std::string name_;
    small_vector<Value*, 4> users_;
    Value* parent_;

    Value(ValueKind kind, TypeID type_id = none, std::string_view name = "", Value* parent = nullptr) :
        kind_{ kind },
        type_id_{ type_id },
        name_{ name },
        users_{ },
        parent_{ parent } {}

    virtual ~Value() = default;

    void add_use(Value* value)
    {
        if (value)
            users_.push_back(value);
    }
};

template <typename T>
concept DerivedFromValue = std::derived_from<T, Value>;

struct Instruction : Value
{
    small_vector<Value*, 2> operands_;

    Instruction(ValueKind kind, std::initializer_list<Value*> operands, TypeID type_id = none, std::string_view name = "") : 
    Value{kind, type_id, name}
    {
        operands_ = operands;
        for (auto* arg : operands_) {
            arg->add_use(this);
        }
    }

    ~Instruction() override
    {
        for (auto* arg : operands_) {
            if (arg) {
                auto it = ranges::find(arg->users_, this);
                swap_pop(arg->users_, it);
            }
        }
    }
};

template <typename T>
concept DerivedFromInstruction = std::derived_from<T, Instruction>;

struct Alloca : Instruction
{
    Alloca(TypeID type_id, std::string_view name) :
        Instruction{ValueKind::Alloca, {}, type_id, name} {}
};

struct Load : Instruction
{
    Load(TypeID type_id, Value* ptr) :
        Instruction{ValueKind::Load, {ptr}, type_id} {}

    Load(Value* ptr) :
        Instruction{ValueKind::Load, {ptr}} {}
};

struct Store : Instruction
{
    Store(Value* dst, Value* src) :
        Instruction{ValueKind::Store, {dst, src}} {}
};

struct Add : Instruction
{
    Add(Value* src1, Value* src2) :
        Instruction{ValueKind::Add, {src1, src2}} {}
};

struct Sub : Instruction
{
    Sub(Value* src1, Value* src2) :
        Instruction{ValueKind::Sub, {src1, src2}} {}
};

struct Mul : Instruction
{
    Mul(Value* src1, Value* src2) :
        Instruction{ValueKind::Mul, {src1, src2}} {}
};

struct Div : Instruction
{
    Div(Value* src1, Value* src2) :
        Instruction{ValueKind::Div, {src1, src2}} {}
};

struct Eq : Instruction
{
    Eq(Value* src1, Value* src2) :
        Instruction{ValueKind::Eq, {src1, src2}} {}
};

struct Ne : Instruction
{
    Ne(Value* src1, Value* src2) :
        Instruction{ValueKind::Ne, {src1, src2}} {}
};

struct Slt : Instruction
{
    Slt(Value* src1, Value* src2) :
        Instruction{ValueKind::Slt, {src1, src2}} {}
};

struct Call : Instruction
{
    // Call(Function* callee, Value* )
};

struct Return : Instruction
{
    Return(Value* value) :
        Instruction{ValueKind::Return, {value}} {}
};

// don't need to type this instruction,
// it is just an opaque ptr, we know what it means from itself and first operand
struct PtrAdd : Instruction
{
    PtrAdd(Value* ptr, Value* offset) :
        Instruction{ValueKind::PtrAdd, {ptr, offset}} {}
};

struct BasicBlock;

struct Branch : Instruction
{      
    // will any DerivedFromValue* bind to the block pointer parameters? 
    Branch(BasicBlock* target);
    Branch(Value* cond, BasicBlock* bb_true, BasicBlock* bb_false);

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

struct Phi : Value
{
    // std::initializer_list<std::pair<Value*, BasicBlock*>> [value, block], [value, block], ... 
};

// types are wrong here
struct Literal : Value
{
    int64_t data_;

    // this will cause error mismatch between accepted literal and internal type
    Literal(int64_t data) : 
        Value{ValueKind::Const, INT32, std::to_string(data)+"L"},
        data_{ data } {}
};

struct BasicBlock : Value
{
    list<Instruction> instructions_; // ordered

    BasicBlock(std::string_view name = "") :
        Value{ValueKind::BasicBlock, none, name} {}
    
    ~BasicBlock() override
    {
        // set all branches that point to this block to nullptr

        for (Branch* branch : static_cast_view<Branch>(users_)) {
            for (auto* target : branch->targets()) {
                if (target == this) {
                    target = nullptr;
                }
            }
        }

        /* doesn't actually destroy IR values, just removes from intrusive list owned by this block, so we can just clear
        while (!instructions_.empty()) {
            instructions_.pop_back();
        }
        */

        instructions_.clear();
    }

    template <DerivedFromInstruction T>
    T* insert(T* value)
    {
        value->parent_ = this;
        instructions_.push_back(*value);
        return value;
    }

    Instruction* terminator()
    {
        if (instructions_.empty())
            return nullptr;

        assert(instructions_.back().kind_ == ValueKind::Return ||
               instructions_.back().kind_ == ValueKind::Branch);

        return &instructions_.back();
    }

    auto successors();

    auto predecessors()
    {
        return users_ | std::views::transform([](Value* value) { return static_cast<BasicBlock*>(value->parent_); });
    }
};

// some out of line definitions (circular deps)

inline Branch::Branch(BasicBlock* target) : 
    Instruction{ValueKind::Branch, {static_cast<Value*>(target)}},
    branch_kind_{BranchKind::Unconditional} {}

inline Branch::Branch(Value* cond, BasicBlock* bb_true, BasicBlock* bb_false) :
    Instruction{ValueKind::Branch, {cond, static_cast<Value*>(bb_true), static_cast<Value*>(bb_false)}},
    branch_kind_{BranchKind::Conditional} {}

inline auto BasicBlock::successors()
{
    small_vector<BasicBlock*, 2> result{};   

    auto* term = terminator();
    if (!term)
        return result;

    if (term->kind_ != ValueKind::Branch)
        return result;

    auto* branch = static_cast<Branch*>(term);

    for (auto* target : static_cast_view<BasicBlock>(branch->targets()))
        result.push_back(target);

    return result;
}

struct Argument : Value
{
    Argument(TypeID type_id = none, std::string_view name = "") :
        Value{ValueKind::Argument, type_id, name} {}
};

struct Function : Value
{
    list<Argument> args_;
    list<BasicBlock> blocks_; // unordered
    
    // memory leak
    Value* return_value_ = nullptr;
    BasicBlock* return_block_ = nullptr;

    Function(std::string_view name = "") :
        Value{ValueKind::Function, none, name} {}

    ~Function() override
    {
        for (Instruction* user : static_cast_view<Instruction>(users_)) {
            for (auto* operand : user->operands_) {
                if (operand == this) {
                    operand = nullptr;
                }
            }
        }
    }

    BasicBlock* insert(BasicBlock* block)
    {
        block->parent_ = this;
        blocks_.push_back(*block);
        return block;
    }

    Argument* insert(Argument* arg)
    {
        arg->parent_ = this;
        args_.push_back(*arg);
        return arg;
    }

    void initialize_return(Value* return_value, BasicBlock* return_block)
    {
        return_value_ = return_value;
        return_block_ = return_block;
    }
};

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
            static_assert(always_false_v<T>, "T is not an internable type");
    }
};

struct Program
{
    std::vector<std::unique_ptr<Value>> values_;

    ConstantPool constant_pool_;
    list<Function> functions_;

    Function* insert(Function* function)
    {
        functions_.push_back(*function);
        return function;
    }
};









