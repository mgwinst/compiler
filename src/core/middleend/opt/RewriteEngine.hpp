#pragma once

#include <unordered_set>
#include <algorithm>

#include "middleend/ir/IR.hpp"
#include "middleend/ir/IRBuilder.hpp"
#include "utils/casting.hpp"

// store-to-load forwarding
// copy propagation
// local CSE
// const prop and folding

// there must be a pass that removes all loads of literals
// the reason we need this is because when we do folding and value->replace_uses_with(folded_value),
// then there could be a final instruction that was a load, and we are now loading a const,

class RewriteEngine
{
public:
    RewriteEngine(Program& program) : 
        program_{ program }, 
        builder_{ &program } {}

    void run()
    {
        for (auto& function : program_.functions_) {
            for (auto& block : function->blocks_) {
                constant_folding(block);
                strength_reduction(block);
            }
        }
    }

private:
    Program& program_;
    IRBuilder builder_;

    void constant_folding(std::unique_ptr<BasicBlock>& block);
    void strength_reduction(std::unique_ptr<BasicBlock>& block);
};
