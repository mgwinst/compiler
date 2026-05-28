#pragma once

#include "IR.hpp"
#include "../../utils/casting.hpp"

struct IRBuilder
{
public:
    Program* program_;
    Function* current_function_ = nullptr;
    BasicBlock* current_block_ = nullptr;

    IRBuilder(Program* program) : 
        program_{ program } {}

    template <DerivedFromValue T, typename... Args>
    T* create(Args&&... args)
    {
        program_->values_.push_back(std::make_unique<T>(std::forward<Args>(args)...));

        T* value = dyn_cast<T>(program_->values_.back().get());

        if constexpr (std::same_as<T, Function>) {
            return program_ ? program_->insert(value) : value;
        } else if constexpr (std::same_as<T, BasicBlock> || std::same_as<T, Argument>) {
            return current_function_ ? current_function_->insert(value) : value;
        } else {
            return current_block_ ? current_block_->insert(value) : value;
        }
    }

    Literal* get_or_create_literal(auto literal)
    {
        auto [it, _] = program_->constant_pool_.try_insert(literal); 
        return it->second.get();
    }
};