#pragma once

#include "IR.hpp"
#include "utils/casting.hpp"

struct IRBuilder
{
    Program* program_;
    Function* current_function_ = nullptr;
    BasicBlock* current_block_ = nullptr;

    IRBuilder(Program* program) : 
        program_{ program } {}

    template <DerivedFromValue T, typename... Args>
    T* create(Args&&... args)
    {
        T* value = new T{ std::forward<Args>(args)... };

        if (!value) 
            return nullptr;

        if constexpr (std::same_as<T, Function>) {
            return program_ ? program_->insert(value) : value;
        } else if constexpr (std::same_as<T, BasicBlock> || std::same_as<T, Argument>) {
            return current_function_ ? current_function_->insert(value) : value;
        } else {
            return current_block_ ? current_block_->insert(value) : value;
        }
    }

    Const* get_or_create_literal(auto literal)
    {
        auto [it, _] = program_->constant_pool_.try_insert(literal); 
        return it->second.get();
    }
};