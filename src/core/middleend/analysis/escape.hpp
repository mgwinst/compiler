#pragma once

#include "middleend/ir/IR.hpp"
#include "utils/casting.hpp"

bool escapes_via(Value* value, Value* target, std::unordered_set<Value*>& visited);
bool escapes(Alloca* alloca);
std::unordered_set<Alloca*> non_escaping_allocas(const std::unique_ptr<Function>& function);