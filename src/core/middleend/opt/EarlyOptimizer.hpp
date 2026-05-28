#pragma once

#include "../ir/IR.hpp"

class EarlyOptimizer
{
public:
    EarlyOptimizer(Program& program) :
        program_{ program } {}

    void run()
    {
        trivial_dce();
        remove_dead_stores();
        // merge_blocks();
    }

private:
    Program& program_;

    void trivial_dce(); // run once before opts

    void remove_dead_stores();
    void merge_blocks(); // handle dead blocks also, do this early so less blocks to traverse in graph
    // void fold_instructions();
 
        


};
