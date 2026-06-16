#pragma once

#include "middleend/ir/IR.hpp"

class EarlyOptimizer
{
public:
    EarlyOptimizer(Program& program) :
        program_{ program } {}

    void run()
    {
        trivial_dce();
        remove_dead_stores();
        cleanup_cfg(); // error with bool literal in condition of loop might stem from this
    }

private:
    Program& program_;

    void trivial_dce(); // run once before opts and once after

    void remove_dead_stores();

    void cleanup_cfg();
 

};