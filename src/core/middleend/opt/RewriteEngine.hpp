#pragma once

#include <unordered_set>
#include <algorithm>

#include "middleend/ir/IR.hpp"
#include "middleend/ir/IRBuilder.hpp"
#include "utils/casting.hpp"

// local CSE
// copy propagation
// store-to-load forwarding

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
