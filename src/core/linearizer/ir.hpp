#include <vector>

#include "../context/context.hpp"
#include "../utils/alias.hpp"

enum class Op : uint32_t
{
    Alloca,
    Load,
    Store,
    Phi,
    Add,
    Sub,
    Mult,
    Div,
};

// t = op arg1 ar2

struct Instruction
{
    Op op;
    SymbolID dst, src1, src2;

    Instruction() = default;
};

struct Program
{
    std::vector<Instruction> instructions;
};

struct BasicBlock
{
    std::vector<InstructionID> instructions; // pointers into the IR instructions, if instruction count is small -> vec else -> linked list for block modification and instruction shuffling
    std::vector<BasicBlockID> predecessors, successors; // what about a block terminator?
};

struct CFG
{
    std::vector<BasicBlock> blocks;
};

// start and exit nodes in CFG

struct Function
{
    CFG cfg;
};


class Linearizer
{
public:
    Linearizer(const ModuleContext& ctx, const SemaTree& tree) :
        ctx_{ ctx }, 
        tree_{ tree } {}

    Program run()
    {
        linearize(tree_.root());
        return std::move(program_);
    }

private:
    Program program_;
    const ModuleContext& ctx_;
    const SemaTree& tree_;

    void linearize(SemaNodeID node_id)
    {
        
    }
};




// also explore pass manager (passing in objects that inherit from 'pass')
