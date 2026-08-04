#pragma once

#include "middleend/ir/IR.hpp"
#include "utils/casting.hpp"

std::unordered_set<Alloca*> non_escaping_allocas(Function* function);