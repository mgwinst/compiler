#pragma once

#include "../ir/IR.hpp"

using namespace IR;

class EarlyOptimizer
{
public:
    EarlyOptimizer(Program& program) :
        program_{ program } {}

    void run()
    {
        trivial_dce();
        remove_dead_stores();
    }

private:
    Program& program_;

    void trivial_dce(); // run once before opts

    void remove_dead_stores();
    // void fold_instructions();
    // void merge_blocks(); // handle dead blocks also
    


    void trivial_dce(); // run once after opts
};
