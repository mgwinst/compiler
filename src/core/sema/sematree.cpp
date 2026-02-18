#include "sematree.hpp"
#include "sema_node.hpp"
#include "types.hpp"

namespace Sema
{
    SemaTree::SemaTree(SemaContext& sema_ctx, const AST& ast) noexcept
    {
        build_sema_node(sema_ctx, ast, ast.root());
    }

    SemaNodeRef SemaTree::build_sema_node(SemaContext& sema_ctx, const AST& ast, ASTNodeRef ast_node_ref) noexcept
    {
        const auto& ast_node = ast.nodes_[ast_node_ref];

        switch (ast_node.get_kind()) {
            case ASTNodeKind::CompilationUnitDecl: {
                SemaNodeRef sema_root = sema_ctx.sem_tree.emplace<Sema::CompilationUnitDecl>(std::string{ "source file" });

                const auto& comp_unit = ast_node.as<SyntaxTree::CompilationUnitDecl>();
                for (auto decl : comp_unit.decls_) {
                    auto sema_node = build_sema_node(sema_ctx, ast, decl);                   
                    sema_ctx.sem_tree.nodes_[sema_root].as<Sema::CompilationUnitDecl>().decls_.push_back(sema_node);
                }

                return sema_root;
            }

            case ASTNodeKind::VarDecl: {
                const auto& var = ast_node.as<SyntaxTree::VarDecl>();

                auto type = sema_ctx.type_pool_->get_type(var.type_);

                // if symbol_table.lookup(var.name) exists with same name, then error (can't have two vars with same ident in same scope)
                // can handle error by printing if it was redefined with different type (float x; int x) or just plain redefinition (int x; int x)
                // symbol_table.insert(var)

                auto symbol = sema_ctx.symbol_table_->insert(sema_ctx, var);

                if (var.init_) {
                    SemaNodeRef init = build_sema_node(sema_ctx, ast, *var.init_);
                    return sema_ctx.sem_tree.emplace<Sema::VarDecl>(type, symbol, init);
                }

                return sema_ctx.sem_tree.emplace<Sema::VarDecl>(type, symbol);
            }

            case ASTNodeKind::FuncDecl:
            case ASTNodeKind::StructDecl:

            default:
                break;
        }

        return 1;
    }

} // namespace Sema