#pragma once

#include "Value.hpp"

using namespace IR;

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

class IRBuilder
{
public:
    Function* create_function(std::string_view name = "");
    BasicBlock* create_basic_block(std::string_view name = "");
    Argument* create_arg(TypeID type_id, std::string_view name = "");
    Literal* create_literal(auto literal);
    AllocaInst* create_alloca(TypeID type_id, std::string_view name);
    LoadInst* create_load(Value* ptr);
    LoadInst* create_load(TypeID target_type_id, Value* ptr);
    StoreInst* create_store(Value* dst, Value* src);
    AddInst* create_add(Value* src1, Value* src2);
    SubInst* create_sub(Value* src1, Value* src2);
    MulInst* create_mul(Value* src1, Value* src2);
    DivInst* create_div(Value* src1, Value* src2);
    EqInst* create_eq(Value* src1, Value* src2);
    NeInst* create_ne(Value* src1, Value* src2);
    SltInst* create_slt(Value* src1, Value* src2);
    Terminator* create_ret(Value* src);
    Terminator* create_br(BasicBlock* target);
    Terminator* create_condbr(Value* cond, BasicBlock* target1, BasicBlock* target2);
    PtrAdd* create_ptradd(Value* base_ptr, Value* index);

    // inst->set_name(std::to_string(++value_count_));

private:
    // assert these are initialized
    Program* current_program_ = nullptr;
    Function* current_function_ = nullptr;
    BasicBlock* current_block_ = nullptr;

    template <DerivedFromValue T, typename... Args>
    T* create(Args&&... args)
    {
        auto* value = new T{ std::forward<Args>(args)... };

        if (!value) return nullptr;

        if constexpr (std::same_as<T, Function>)
            return current_program_ ? current_program_->insert(value) : value;
        else if constexpr (std::same_as<T, BasicBlock> || std::same_as<T, Argument>)
            return current_function_ ? current_function_->insert(value) : value;
        else
            return current_block_ ? current_block_->insert(inst) : inst;
    }

};

Literal* intern_literal(auto literal)
{
    auto [it, inserted] = program_.constants().try_insert(literal); 
    return it->second.get();
}

Literal* IRBuilder::create_literal(auto literal)
{
    return intern_literal(literal);
}
