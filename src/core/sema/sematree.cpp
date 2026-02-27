#include "sematree.hpp"
#include "sema_node.hpp"

#define NODE_LIMIT (1 << 18)

namespace Sema
{
    SemaTree::SemaTree() noexcept
    {
        nodes_.reserve(NODE_LIMIT);
    }

    SemaNodeRef SemaTree::build_sema_node(SemaContext& sema_ctx, const AST& ast, ASTNodeRef ast_node_ref) noexcept
    {
        const auto& ast_node = ast.nodes_[ast_node_ref];

        switch (ast_node.get_kind()) {
            case ASTNodeKind::CompilationUnitDecl: {
                SemaNodeRef sema_root = sema_ctx.sema_tree_->emplace<Sema::CompilationUnitDecl>(std::string{ "source file" });

                sema_ctx.symbol_table_->enter_scope();

                const auto& comp_unit = ast_node.as<SyntaxTree::CompilationUnitDecl>();
                for (auto decl : comp_unit.decls_) {
                    auto sema_node = build_sema_node(sema_ctx, ast, decl);                   
                    sema_ctx.sema_tree_->nodes_[sema_root].as<Sema::CompilationUnitDecl>().decls_.push_back(sema_node);
                }

                sema_ctx.symbol_table_->exit_scope();

                return sema_root;
            }

            case ASTNodeKind::VarDecl: {
                const auto& var = ast_node.as<SyntaxTree::VarDecl>();

                // focus here
                auto type = sema_ctx.type_pool_->resolve_type(var.type_expr_, ast);

                if (sema_ctx.symbol_table_->exists_in_scope(var.name_)) {
                    // redefining identifier in same scope
                    // if types match (plain redefinition error)
                    // types don't match (redefinition error 'with different type')
                }
                
                auto symbol = sema_ctx.symbol_table_->insert(var, type);

                if (var.init_) {
                    SemaNodeRef init = build_sema_node(sema_ctx, ast, *var.init_);
                    return sema_ctx.sema_tree_->emplace<Sema::VarDecl>(symbol, type, init);
                }

                return sema_ctx.sema_tree_->emplace<Sema::VarDecl>(symbol, type);
            }

            case ASTNodeKind::ParamDecl: {
                const auto& param = ast_node.as<SyntaxTree::ParamDecl>();
                
                auto type = sema_ctx.type_pool_->resolve_type(param.type_expr_, ast);

                if (sema_ctx.symbol_table_->exists_in_scope(param.name_)) {
                    // redefine error, param already exists in function scope
                }

                auto symbol = sema_ctx.symbol_table_->insert(param, type);
                
                return sema_ctx.sema_tree_->emplace<Sema::ParamDecl>(symbol, type);
            }

            case ASTNodeKind::FuncDecl: {
                const auto& func = ast_node.as<SyntaxTree::FuncDecl>();

                auto func_type = sema_ctx.type_pool_->resolve_type(ast_node_ref, ast);

                if (sema_ctx.symbol_table_->exists_in_scope(func.name_)) {
                    // function redefinition error
                }
                
                auto func_symbol = sema_ctx.symbol_table_->insert(func, func_type);

                sema_ctx.symbol_table_->enter_scope();

                std::vector<SemaNodeRef> params;
                for (auto p : func.params_) {
                    params.push_back(build_sema_node(sema_ctx, ast, p));
                }

                auto body = build_sema_node(sema_ctx, ast, func.body_);

                sema_ctx.symbol_table_->exit_scope();

                return sema_ctx.sema_tree_->emplace<Sema::FuncDecl>(func_symbol, func_type, std::move(params), body);
            }

            case ASTNodeKind::RecordDecl: {
                break;
            }

            default:
                break;
        }

        return 1;
    }

} // namespace Sema