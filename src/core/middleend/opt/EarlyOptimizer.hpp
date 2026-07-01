#pragma once

#include "middleend/ir/IR.hpp"
#include "middleend/ir/IRBuilder.hpp"

class EarlyOptimizer
{
public:
    EarlyOptimizer(Program& program) :
        program_{ program },
        builder_{ &program } {}

    void run()
    {
        // inline_functions()
        // sroa();

        trivial_dce();
        remove_dead_stores();
        cleanup_cfg();
        trivial_dce();
        mem2reg();
    }

private:
    Program& program_;
    IRBuilder builder_;

    // void inline_functions();
    // void sroa();

    void trivial_dce();
    void remove_dead_stores();

    // instruction simplify
    // void constant_folding();
    // void algebra();

    void cleanup_cfg();
    void mem2reg();
};

// need to implement
// const prop -> const fold -> algebra