#include <algorithm>
#include <unordered_set>

#include "sema_pass.hpp"

/*
    dusugar

    for -> while
    a[i] -> *(a + i)
    x += 3 -> x = x + 3
    3 + 5 -> 8, (simple constant folding)
*/

namespace Sema
{
    SemaNodeRef for_to_while(SemaContext& ctx, const ForStmt& for_stmt)
    {
        ctx.sema_tree_->nodes_[for_stmt.body_].as<CompoundStmt>().children_.push_back(for_stmt.update_);

        auto while_stmt = ctx.sema_tree_->emplace<WhileStmt>(for_stmt.cond_, for_stmt.body_);

        std::vector<SemaNodeRef> children;
        children.push_back(for_stmt.init_);
        children.push_back(while_stmt);

        return ctx.sema_tree_->emplace<CompoundStmt>(std::move(children));
    }

    SemaNodeRef arr_idx_to_ptr_arithmetic(SemaContext& ctx, const ArraySubscriptExpr& expr) 
    {
        auto bin_op = ctx.sema_tree_->emplace<BinaryExpr>("+", expr.base_, expr.index_);
        return ctx.sema_tree_->emplace<UnaryExpr>("*", bin_op);
    }

    bool is_compound_assign(std::string_view op)
    {
        static const std::unordered_set<std::string_view> compound_ops { "+=", "-=", "*=", "/=", "%=" };
        return compound_ops.contains(op);
    }

    SemaNodeRef expand_compound_assignment(SemaContext& ctx, const BinaryExpr& expr)
    {
        auto op = expr.op_[0];
        auto right = ctx.sema_tree_->emplace<BinaryExpr>(std::string{ op }, expr.left_, expr.right_);
        return ctx.sema_tree_->emplace<BinaryExpr>("=", expr.left_, right);
    }

    void desugar(SemaContext& ctx, SemaNodeRef ref)
    {
        auto& node = ctx.sema_tree_->nodes_[ref];

        switch (node.get_kind()) {
            case SemaNodeKind::CompilationUnitDecl: {
                const auto& comp_unit = node.as<Sema::CompilationUnitDecl>();
                for (auto decl : comp_unit.decls_)
                    desugar(ctx, decl);
                break;
            }

            case SemaNodeKind::FuncDecl: {
                const auto& func = node.as<Sema::FuncDecl>();
                desugar(ctx, func.body_);
                break;
            }

            case SemaNodeKind::CompoundStmt: {
                const auto& cmpd_stmt = node.as<Sema::CompoundStmt>();
                for (auto c : cmpd_stmt.children_)
                    desugar(ctx, c);
                break;
            }

            case SemaNodeKind::ReturnStmt: {
                const auto& ret = node.as<Sema::ReturnStmt>();
                desugar(ctx, ret.value_);
                break;
            }

            case SemaNodeKind::IfStmt: {
                const auto& if_stmt = node.as<Sema::IfStmt>();
                desugar(ctx, if_stmt.cond_);
                desugar(ctx, if_stmt.then_stmt_);
                if (if_stmt.else_stmt_.has_value())
                    desugar(ctx, *if_stmt.else_stmt_);
                break;
            }

            case SemaNodeKind::WhileStmt: {
                const auto& while_stmt = node.as<Sema::WhileStmt>();
                desugar(ctx, while_stmt.cond_);
                desugar(ctx, while_stmt.body_);
                break;
            }

            case SemaNodeKind::ForStmt: {
                const auto& for_stmt = node.as<Sema::ForStmt>();

                desugar(ctx, for_stmt.body_);

                auto while_stmt = for_to_while(ctx, for_stmt);
                std::swap(ctx.sema_tree_->nodes_[ref], ctx.sema_tree_->nodes_[while_stmt]);
                ctx.sema_tree_->nodes_.pop_back();

                break;
            }
            
            // ++a -> a = a + 1
            case SemaNodeKind::UnaryExpr: {
                const auto& ue = node.as<Sema::UnaryExpr>();

                if (ue.op_ == "++" || ue.op_ == "--") {
                    auto op = ue.op_[0];

                    auto literal_one = ctx.sema_tree_->emplace<IntegerLiteralExpr>(1);
                    auto right = ctx.sema_tree_->emplace<BinaryExpr>(std::string{ op }, ue.operand_, literal_one);
                    auto expanded = ctx.sema_tree_->emplace<BinaryExpr>("=", ue.operand_, right);

                    std::swap(ctx.sema_tree_->nodes_[ref], ctx.sema_tree_->nodes_[expanded]);
                    ctx.sema_tree_->nodes_.pop_back();
                }
                    
                break;
            }

            case SemaNodeKind::BinaryExpr: {
                const auto& be = node.as<Sema::BinaryExpr>();

                desugar(ctx, be.left_);
                desugar(ctx, be.right_);

                if (is_compound_assign(be.op_)) {
                    auto new_expr = expand_compound_assignment(ctx, be);
                    std::swap(ctx.sema_tree_->nodes_[ref], ctx.sema_tree_->nodes_[new_expr]);
                    ctx.sema_tree_->nodes_.pop_back();
                }

                break;
            }

            // a[i] -> *(a + i)
            case SemaNodeKind::ArraySubscriptExpr: {
                const auto& expr = node.as<Sema::ArraySubscriptExpr>();
                desugar(ctx, expr.index_);
                auto new_expr = arr_idx_to_ptr_arithmetic(ctx, expr);

                std::swap(ctx.sema_tree_->nodes_[ref], ctx.sema_tree_->nodes_[new_expr]);
                ctx.sema_tree_->nodes_.pop_back();
                
                break;
            }

            case SemaNodeKind::InitListExpr: {
                const auto& init_list = node.as<Sema::InitListExpr>();
                for (auto v : init_list.init_values_)
                    desugar(ctx, v);
                break;
            }

            default:
                break;
        }
    }

    void desugar(SemaContext& ctx)
    {
        desugar(ctx, ctx.sema_tree_->root());
    }

} // namespace Sema