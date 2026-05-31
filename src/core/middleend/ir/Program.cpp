#include "IR.hpp"

Function* Program::insert(Function* function)
{
    functions_.push_back(std::unique_ptr<Function>(function));
    return function;
}