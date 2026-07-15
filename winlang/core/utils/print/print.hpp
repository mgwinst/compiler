#pragma once

#include "frontend/sema/sematree.hpp"
#include "middleend/ir/IR.hpp"

std::string type_to_str(Type* type);
std::string node_to_str(Sema::SemaNode* node, std::string indent);
void print(SemaTree& tree);
void print(Program& program);