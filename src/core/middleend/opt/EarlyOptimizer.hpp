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
        cleanup_cfg();
    }

private:
    Program& program_;

    void trivial_dce(); // run once before opts and once after

    void remove_dead_stores();

    void cleanup_cfg();
 
        


};
