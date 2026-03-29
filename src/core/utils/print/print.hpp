#pragma once

#include "../../frontend/ast/ast.hpp"
#include "../../frontend/sema/sematree.hpp"
#include "../../context/context.hpp"

void print(const AST& ast);
void print(const Sema::SemaTree& sema_tree, const Sema::SemaContext& ctx);

std::string type_to_str(const Sema::SemaContext& ctx, TypeId type_ref);
std::string sema_node_to_str(const Sema::SemaContext& ctx, const Sema::SemaTree& sema_tree, const SemaNodeId ref, std::string indent);

void print_sema_tree(const Sema::SemaContext& ctx);
void print_symbol_table(const Sema::SemaContext& ctx);
void print_type_pool(const Sema::SemaContext& ctx);