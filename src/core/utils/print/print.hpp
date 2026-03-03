#pragma once

#include <tuple>

#include "../../ast/ast.hpp"
#include "../../sema/sematree.hpp"

void print(const AST& ast);
void print(const Sema::SemaTree& sema_tree, const Sema::SemaContext& sema_ctx);

std::string type_to_str(const Sema::TypePool& type_pool, TypeRef type_ref);
std::string ast_node_to_str(const AST& sema_tree, ASTNodeRef ref, std::string indent);
std::string sema_node_to_str(const Sema::SemaContext& sema_ctx, const Sema::SemaTree& sema_tree, const SemaNodeRef ref, std::string indent);

inline std::tuple<const std::string&, const TypeRef> query_symbol(const Sema::SemaContext& sema_ctx, SymbolRef ref)
{
    const auto& name = sema_ctx.symbol_table_->symbol_pool_[ref].identifier_;

    const auto type = sema_ctx.symbol_table_->symbol_pool_[ref].type_;
    
    return {name, type};
}