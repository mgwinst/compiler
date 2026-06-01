#pragma once

#include <initializer_list>
#include <unordered_set>
#include <string_view>
#include <cstdint>
#include <concepts>
#include <algorithm>
#include <memory>
#include <vector>
#include <list>
#include <string>
#include <ranges>
#include <utility>
#include <cassert>

#include "boost/container/small_vector.hpp"

#include "../../frontend/sema/types/types.hpp"
#include "../../utils/alias.hpp"
#include "../../utils/utils.hpp"
#include "../../utils/enums.hpp"
#include "../../utils/cast_range.hpp"

using boost::container::small_vector;

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

struct Value
{
    ValueKind kind_;
    TypeID type_id_;
    std::string name_;
    small_vector<Value*, 4> users_;
    Value* parent_;

    Value(ValueKind kind, TypeID type_id = none, std::string_view name = "", Value* parent = nullptr);

    virtual ~Value() = default;

    void add_use(Value* value);
};

template <typename T>
concept DerivedFromValue = std::derived_from<T, Value>;

struct Instruction : Value
{
    small_vector<Value*, 2> operands_;

    Instruction(ValueKind kind, std::initializer_list<Value*> operands, TypeID type_id = none, std::string_view name = "");

    ~Instruction() override;
};

template <typename T>
concept DerivedFromInstruction = std::derived_from<T, Instruction>;

struct Alloca : Instruction
{
    Alloca(TypeID type_id, std::string_view name);
};

struct Load : Instruction
{
    Load(TypeID type_id, Value* ptr);
    Load(Value* ptr);
};

struct Store : Instruction
{
    Store(Value* dst, Value* src);
};

struct Add : Instruction
{
    Add(Value* src1, Value* src2);
};

struct Sub : Instruction
{
    Sub(Value* src1, Value* src2);
};

struct Mul : Instruction
{
    Mul(Value* src1, Value* src2);
};

struct Div : Instruction
{
    Div(Value* src1, Value* src2);
};

struct Eq : Instruction
{
    Eq(Value* src1, Value* src2);
};

struct Ne : Instruction
{
    Ne(Value* src1, Value* src2);
};

struct Slt : Instruction
{
    Slt(Value* src1, Value* src2);
};

struct Call : Instruction
{
    // Call(Function* callee, Value* )
};

struct Return : Instruction
{
    Return(Value* value);
};

// don't need to type this instruction,
// it is just an opaque ptr, we know what it means from itself and first operand
struct PtrAdd : Instruction
{
    PtrAdd(Value* ptr, Value* offset);
};

struct BasicBlock;

struct Branch : Instruction
{      
    BranchKind branch_kind_;

    Branch(BasicBlock* target);
    Branch(Value* cond, BasicBlock* bb_true, BasicBlock* bb_false);

    bool is_conditional() const;
    Value* condition();
    std::span<Value*> targets();
};

struct Phi : Value
{
    // std::initializer_std::list<std::pair<Value*, BasicBlock*>> [value, block], [value, block], ... 
};

struct Literal : Value
{
    int64_t data_;

    // this will cause error mismatch between accepted literal and internal type
    Literal(int64_t data);
};

struct BasicBlock : Value
{
    std::list<std::unique_ptr<Instruction>> instructions_; // ordered

    BasicBlock(std::string_view name = "");
    
    ~BasicBlock() override;

    template <DerivedFromInstruction T>
    T* insert(T* value)
    {
        value->parent_ = this;
        instructions_.push_back(std::unique_ptr<Instruction>{ value });
        return value;
    }

    bool empty();
    Instruction* terminator();
    small_vector<BasicBlock*, 2> successors();
    auto predecessors() 
    { 
        return users_ | std::views::transform([](Value* value) { return static_cast<BasicBlock*>(value->parent_); });
    };
    void remove_from_parent();
};

struct Argument : Value
{
    Argument(TypeID type_id = none, std::string_view name = "");
};

struct Function : Value
{
    std::vector<std::unique_ptr<Argument>> args_;
    std::vector<std::unique_ptr<BasicBlock>> blocks_; // unordered
    
    // memory leak
    Value* return_value_ = nullptr;
    BasicBlock* return_block_ = nullptr;

    Function(std::string_view name = "");

    ~Function() override;

    BasicBlock* insert(BasicBlock* block);
    Argument* insert(Argument* arg);
    void initialize_return(Value* return_value, BasicBlock* return_block);
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
    ConstantPool constant_pool_;
    std::list<std::unique_ptr<Function>> functions_;

    Program() = default;
    Program(Program&&) noexcept = default;
    Program& operator=(Program&&) noexcept = default;
    Program(const Program&) = delete;
    Program& operator=(const Program&) = delete;
    ~Program() = default;

    Function* insert(Function* function);
};


inline std::vector<BasicBlock*> post_order(std::unique_ptr<Function>& function, bool reverse = false)
{
    if (function->blocks_.empty())
        return { };

    BasicBlock* entry = function->blocks_.front().get();

    std::unordered_set<BasicBlock*> visited;   
    std::vector<BasicBlock*> post_order;

    auto dfs = [&](this auto& self, BasicBlock* block) {
        if (visited.contains(block)) 
            return;
        visited.insert(block);
        for (auto* succ : block->successors())
            self(succ);
        post_order.push_back(block);
    };

    dfs(entry);

    if (reverse)
        std::ranges::reverse(post_order);

    return post_order;
}












