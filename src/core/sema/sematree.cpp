#include "sematree.hpp"
#include "sema_node.hpp"

#define NODE_LIMIT (1 << 18)

namespace Sema
{
    SemaTree::SemaTree() noexcept
    {
        nodes_.reserve(NODE_LIMIT);
    }

    SemaNodeRef SemaTree::root() const noexcept {
        return 0;
    }


    SemaNodeRef SemaTree::build_sema_node(SemaContext& sema_ctx, const AST& ast, ASTNodeRef ast_node_ref) noexcept
    {
        const auto& ast_node = ast.nodes_[ast_node_ref];

        switch (ast_node.get_kind()) {
            case ASTNodeKind::CompilationUnitDecl: {
                SemaNodeRef sema_root = sema_ctx.sema_tree_->emplace<CompilationUnitDecl>(std::string{ "source file" });

                sema_ctx.symbol_table_->enter_scope();

                const auto& comp_unit = ast_node.as<Syntax::CompilationUnitDecl>();
                for (auto decl : comp_unit.decls_) {
                    auto sema_node = build_sema_node(sema_ctx, ast, decl);                   
                    sema_ctx.sema_tree_->nodes_[sema_root].as<CompilationUnitDecl>().decls_.push_back(sema_node);
                }

                sema_ctx.symbol_table_->exit_scope();

                return sema_root;
            }

            case ASTNodeKind::VarDecl: {
                const auto& var = ast_node.as<Syntax::VarDecl>();

                // focus here
                auto type = sema_ctx.type_pool_->resolve_type(var.type_expr_, ast);

                if (sema_ctx.symbol_table_->exists_in_scope(var.name_)) {
                    // redefining identifier in same scope
                    // if types match (plain redefinition error)
                    // types don't match (redefinition error 'with different type')
                    error_exit("variable redefinition");
                }
                
                auto symbol = sema_ctx.symbol_table_->insert(var, type);

                if (var.init_) {
                    SemaNodeRef init = build_sema_node(sema_ctx, ast, *var.init_);
                    return sema_ctx.sema_tree_->emplace<VarDecl>(symbol, type, init);
                }

                return sema_ctx.sema_tree_->emplace<VarDecl>(symbol, type);
            }

            case ASTNodeKind::ParamDecl: {
                const auto& param = ast_node.as<Syntax::ParamDecl>();
                
                auto type = sema_ctx.type_pool_->resolve_type(param.type_expr_, ast);

                if (sema_ctx.symbol_table_->exists_in_scope(param.name_)) {
                    error_exit("parameter redefinition");
                }

                auto symbol = sema_ctx.symbol_table_->insert(param, type);
                
                return sema_ctx.sema_tree_->emplace<ParamDecl>(symbol, type);
            }

            case ASTNodeKind::FuncDecl: {
                const auto& func = ast_node.as<Syntax::FuncDecl>();

                auto func_type = sema_ctx.type_pool_->resolve_type(ast_node_ref, ast);

                if (sema_ctx.symbol_table_->exists_in_scope(func.name_)) {
                    error_exit("function redefinition")
                }
                
                auto func_symbol = sema_ctx.symbol_table_->insert(func, func_type);

                sema_ctx.symbol_table_->enter_scope();

                std::vector<SemaNodeRef> params;
                for (auto p : func.params_)
                    params.push_back(build_sema_node(sema_ctx, ast, p));

                auto body = build_sema_node(sema_ctx, ast, func.body_);

                sema_ctx.symbol_table_->exit_scope();

                return sema_ctx.sema_tree_->emplace<FuncDecl>(func_symbol, func_type, std::move(params), body);
            }

            case ASTNodeKind::RecordDecl: {
                const auto& rec = ast_node.as<Syntax::RecordDecl>();

                auto rec_type = sema_ctx.type_pool_->resolve_type(ast_node_ref, ast);
                
                if (sema_ctx.symbol_table_->exists_in_scope(rec.name_)) {
                    error_exit("struct redefinition")
                }

                auto rec_symbol = sema_ctx.symbol_table_->insert(rec, rec_type);

                sema_ctx.symbol_table_->enter_scope();               

                std::vector<SemaNodeRef> fields;
                for (auto p : rec.fields_)
                    fields.push_back(build_sema_node(sema_ctx, ast, p));

                sema_ctx.symbol_table_->exit_scope();

                return sema_ctx.sema_tree_->emplace<RecordDecl>(rec_symbol, rec_type, std::move(fields));
            }

            case ASTNodeKind::CompoundStmt: {
                const auto& cmpd_stmt = ast_node.as<Syntax::CompoundStmt>();
                
                std::vector<SemaNodeRef> children;
                for (auto c : cmpd_stmt.children_)
                    children.push_back(build_sema_node(sema_ctx, ast, c));

                if (cmpd_stmt.return_stmt_)
                    return sema_ctx.sema_tree_->emplace<CompoundStmt>(std::move(children), build_sema_node(sema_ctx, ast, *cmpd_stmt.return_stmt_));

                return sema_ctx.sema_tree_->emplace<CompoundStmt>(std::move(children));
            }

            case ASTNodeKind::ReturnStmt: {
                const auto& ret = ast_node.as<ReturnStmt>();
                return sema_ctx.sema_tree_->emplace<ReturnStmt>(ret.value_);
            }

            // fix empty else case -1 (check -1 everywhere shift?)
            case ASTNodeKind::IfStmt: {
                const auto& if_stmt = ast_node.as<Syntax::IfStmt>();

                sema_ctx.symbol_table_->enter_scope();
                
                auto cond = build_sema_node(sema_ctx, ast, if_stmt.cond_);
                auto then_stmt = build_sema_node(sema_ctx, ast, if_stmt.then_stmt_);

                auto else_stmt = if_stmt.else_stmt_ ? build_sema_node(sema_ctx, ast, *if_stmt.else_stmt_) : -1;

                sema_ctx.symbol_table_->exit_scope();

                if (else_stmt == -1)
                    return sema_ctx.sema_tree_->emplace<IfStmt>(cond, then_stmt);
                else
                    return sema_ctx.sema_tree_->emplace<IfStmt>(cond, then_stmt, else_stmt);
            }

            case ASTNodeKind::WhileStmt: {
                const auto& while_stmt = ast_node.as<Syntax::WhileStmt>();
                
                sema_ctx.symbol_table_->enter_scope();

                auto cond = build_sema_node(sema_ctx, ast, while_stmt.cond_);
                auto body = build_sema_node(sema_ctx, ast, while_stmt.body_);

                sema_ctx.symbol_table_->exit_scope();
                
                return sema_ctx.sema_tree_->emplace<WhileStmt>(cond, body);
            }

            case ASTNodeKind::ForStmt: {
                const auto& for_stmt = ast_node.as<Syntax::ForStmt>();
                
                sema_ctx.symbol_table_->enter_scope();

                auto init   = build_sema_node(sema_ctx, ast, for_stmt.init_);
                auto cond   = build_sema_node(sema_ctx, ast, for_stmt.cond_);
                auto update = build_sema_node(sema_ctx, ast, for_stmt.update_);
                auto body   = build_sema_node(sema_ctx, ast, for_stmt.body_);

                sema_ctx.symbol_table_->exit_scope();
                
                return sema_ctx.sema_tree_->emplace<ForStmt>(init, cond, update, body);
            }

            case ASTNodeKind::IntegerLiteralExpr: {
                const auto& int_lit = ast_node.as<Syntax::IntegerLiteralExpr>();
                return sema_ctx.sema_tree_->emplace<IntegerLiteralExpr>(int_lit.value_);
            }

            case ASTNodeKind::FloatLiteralExpr: {
                const auto& float_lit = ast_node.as<Syntax::FloatLiteralExpr>();
                return sema_ctx.sema_tree_->emplace<FloatLiteralExpr>(float_lit.value_);
            }

            case ASTNodeKind::CharLiteralExpr: {
                const auto& char_lit = ast_node.as<Syntax::CharLiteralExpr>();
                return sema_ctx.sema_tree_->emplace<CharLiteralExpr>(char_lit.value_);
            }

            case ASTNodeKind::StringLiteralExpr: {
                const auto& str_lit = ast_node.as<Syntax::StringLiteralExpr>();
                return sema_ctx.sema_tree_->emplace<StringLiteralExpr>(str_lit.value_);
            }

            case ASTNodeKind::BooleanLiteralExpr: {
                const auto& bool_lit = ast_node.as<Syntax::BooleanLiteralExpr>();
                return sema_ctx.sema_tree_->emplace<BooleanLiteralExpr>(bool_lit.value_);
            }

            case ASTNodeKind::UnaryExpr: {
                const auto& ue = ast_node.as<Syntax::UnaryExpr>();
                return sema_ctx.sema_tree_->emplace<UnaryExpr>(std::move(ue.op_), build_sema_node(sema_ctx, ast, ue.operand_), ue.is_postfix_);
            }

            case ASTNodeKind::BinaryExpr: {
                const auto& be = ast_node.as<Syntax::BinaryExpr>();
                auto left = build_sema_node(sema_ctx, ast, be.left_);
                auto right = build_sema_node(sema_ctx, ast, be.right_);
                return sema_ctx.sema_tree_->emplace<BinaryExpr>(be.op_, left, right);
            }

            case ASTNodeKind::ReferenceExpr: {
                const auto& ref = ast_node.as<Syntax::ReferenceExpr>();

                auto target_symbol = sema_ctx.symbol_table_->lookup(ref.name_);
                if (target_symbol == -1)
                    error_exit("use of undeclared identifier");

                auto target_type = sema_ctx.symbol_table_->symbol_pool_[target_symbol].type_;
                auto target_ident = sema_ctx.symbol_table_->symbol_pool_[target_symbol].identifier_;

                return sema_ctx.sema_tree_->emplace<ReferenceExpr>(target_symbol, target_type, std::move(target_ident));
            }

            case ASTNodeKind::CallExpr: {
                const auto& call = ast_node.as<Syntax::CallExpr>();
                auto callee = build_sema_node(sema_ctx, ast, call.callee_);
                
                std::vector<SemaNodeRef> args;
                for (auto arg : call.args_)
                    args.push_back(build_sema_node(sema_ctx, ast, arg));
                
                return sema_ctx.sema_tree_->emplace<CallExpr>(callee, std::move(args));
            }

            case ASTNodeKind::MemberExpr: {
                const auto& member = ast_node.as<Syntax::MemberExpr>();
                auto base = build_sema_node(sema_ctx, ast, member.base_);
                
                return sema_ctx.sema_tree_->emplace<MemberExpr>(base, std::move(member.member_), member.is_arrow_);
            }

            case ASTNodeKind::ArraySubscriptExpr: {
                const auto& arr_sub = ast_node.as<Syntax::ArraySubscriptExpr>();

                auto base = build_sema_node(sema_ctx, ast, arr_sub.base_);
                auto index = build_sema_node(sema_ctx, ast, arr_sub.index_);

                return sema_ctx.sema_tree_->emplace<ArraySubscriptExpr>(base, index);
            }

            case ASTNodeKind::InitListExpr: {
                const auto& init_list = ast_node.as<Syntax::InitListExpr>();
    
                std::vector<SemaNodeRef> values;
                for (auto v : init_list.init_values_)
                    values.push_back(build_sema_node(sema_ctx, ast, v));

                return sema_ctx.sema_tree_->emplace<InitListExpr>(std::move(values));
            }

            case ASTNodeKind::ExplicitCastExpr:
            case ASTNodeKind::ImplicitCastExpr:

            default:
                std::println("{}", (int)ast_node.get_kind());
                error_exit("build_sema_node()");
        }
    }

} // namespace Sema