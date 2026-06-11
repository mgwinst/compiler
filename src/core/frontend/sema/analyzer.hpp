#pragma once

#include "context/context.hpp"
#include "frontend/ast/ast.hpp"
#include "frontend/sema/sematree.hpp"

class SemanticAnalyzer
{
public:
    SemanticAnalyzer(ModuleContext& ctx, AST& ast) :
        ctx_{ ctx },
        ast_{ ast } {}

    SemaTree run();

private:
    ModuleContext& ctx_;
    AST& ast_;
    SemaTree sema_tree_;

    Sema::SemaNode* build_sema_node(Syntax::ASTNode* node);
    Type* check_type(Sema::SemaNode* node);
    void desugar(Sema::SemaNode*& node);
};

