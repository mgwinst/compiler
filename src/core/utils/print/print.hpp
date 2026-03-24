#pragma once

#include "../../ast/ast.hpp"
#include "../../sema/sematree.hpp"
#include "../../sema/types/type_pool.hpp"
#include "../../context/context.hpp"

void print(const AST& ast);
void print(const Sema::SemaTree& sema_tree, const Sema::SemaContext& ctx);

std::string type_to_str(const Sema::TypePool& type_pool, TypeId type_ref);
std::string sema_node_to_str(const Sema::SemaContext& ctx, const Sema::SemaTree& sema_tree, const SemaNodeId ref, std::string indent);

void print_sema_tree(const Sema::SemaContext& ctx);
void print_symbol_table(const Sema::SemaContext& ctx);
void print_type_pool(const Sema::SemaContext& ctx);