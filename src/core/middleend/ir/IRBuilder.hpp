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

struct IRBuilder
{
public:
    Program& program_;
    Function* current_function_ = nullptr;
    BasicBlock* current_block_ = nullptr;
    ConstantPool constant_pool_;

    IRBuilder(Program& program) : 
        program_{ program } {}

    // inst->set_name(std::to_string(++value_count_));

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

    Literal* get_or_create_literal(auto literal)
    {
        auto [it, _] = program_.constants().try_insert(literal); 
        return it->second.get();
    }
};