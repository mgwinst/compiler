#include "desugar.hpp"
#include <algorithm>

#include "sematree.hpp"

using namespace Sema;

SemaNodeID for_to_while(SemaTree& tree, const ForStmt& for_stmt)
{
    tree.nodes_[for_stmt.body_].as<CompoundStmt>().children_.push_back(for_stmt.update_);

    auto while_stmt = tree.emplace<WhileStmt>(for_stmt.cond_, for_stmt.body_);

    std::vector<SemaNodeID> children;
    children.push_back(for_stmt.init_);
    children.push_back(while_stmt);

    return tree.emplace<CompoundStmt>(std::move(children));
}

SemaNodeID arr_idx_to_ptr_arithmetic(SemaTree& tree, const ArraySubscriptExpr& expr) 
{
    auto bin_op = tree.emplace<BinaryExpr>("+", expr.base_, expr.index_, SourceLoc{});
    return tree.emplace<UnaryExpr>("*", bin_op, false, SourceLoc{});
}

bool is_compound_assign(std::string_view op)
{
    static const std::unordered_set<std::string_view> compound_ops { "+=", "-=", "*=", "/=", "%=" };
    return compound_ops.contains(op);
}

SemaNodeID expand_compound_assignment(SemaTree& tree, const BinaryExpr& expr)
{
    auto op = expr.op_[0];
    auto right = tree.emplace<BinaryExpr>(std::string{ op }, expr.left_, expr.right_, SourceLoc{});
    return tree.emplace<BinaryExpr>("=", expr.left_, right, SourceLoc{});
}

void TreeDesugarer::run()
{
    desugar_node(tree_.root());
}

void TreeDesugarer::desugar_node(SemaNodeID node_id)
{
    const auto& node = tree_.nodes_[node_id];

    switch (node.get_kind()) {
        case SemaNodeKind::ModuleDecl: {
            const auto& module = node.as<ModuleDecl>();
            for (auto decl : module.decls_)
                desugar_node(decl);
            break;
        }

        case SemaNodeKind::FuncDecl: {
            const auto& func = node.as<FuncDecl>();
            desugar_node(func.body_);
            break;
        }

        case SemaNodeKind::CompoundStmt: {
            const auto& cmpd_stmt = node.as<CompoundStmt>();
            for (auto c : cmpd_stmt.children_)
                desugar_node(c);
            break;
        }

        case SemaNodeKind::ReturnStmt: {
            const auto& ret = node.as<ReturnStmt>();
            desugar_node(ret.value_);
            break;
        }

        case SemaNodeKind::IfStmt: {
            const auto& if_stmt = node.as<IfStmt>();
            desugar_node(if_stmt.cond_);
            desugar_node(if_stmt.then_stmt_);
            if (if_stmt.else_stmt_.has_value())
                desugar_node(*if_stmt.else_stmt_);
            break;
        }

        case SemaNodeKind::WhileStmt: {
            const auto& while_stmt = node.as<WhileStmt>();
            desugar_node(while_stmt.cond_);
            desugar_node(while_stmt.body_);
            break;
        }

        // for -> while
        case SemaNodeKind::ForStmt: {
            const auto& for_stmt = node.as<ForStmt>();

            desugar_node(for_stmt.body_);

            auto while_stmt = for_to_while(tree_, for_stmt);
            std::swap(tree_.nodes_[node_id], tree_.nodes_[while_stmt]);
            tree_.nodes_.pop_back();

            break;
        }
        
        // ++a -> a = a + 1
        case SemaNodeKind::UnaryExpr: {
            const auto& unary = node.as<UnaryExpr>();

            if (unary.op_ == "++" || unary.op_ == "--") {
                auto op = unary.op_[0];

                auto literal_one = tree_.emplace<IntegerLiteralExpr>(1);
                auto right = tree_.emplace<BinaryExpr>(std::string{ op }, unary.operand_, literal_one, SourceLoc{});
                auto expanded = tree_.emplace<BinaryExpr>("=", unary.operand_, right, SourceLoc{});

                std::swap(tree_.nodes_[node_id], tree_.nodes_[expanded]);
                tree_.nodes_.pop_back();
            }
                
            break;
        }

        // a += b -> a = a + b
        case SemaNodeKind::BinaryExpr: {
            const auto& be = node.as<BinaryExpr>();

            desugar_node(be.left_);
            desugar_node(be.right_);

            if (is_compound_assign(be.op_)) {
                auto new_expr = expand_compound_assignment(tree_, be);
                std::swap(tree_.nodes_[node_id], tree_.nodes_[new_expr]);
                tree_.nodes_.pop_back();
            }

            break;
        }

        // a[i] -> *(a + i)
        case SemaNodeKind::ArraySubscriptExpr: {
            const auto& expr = node.as<ArraySubscriptExpr>();

            desugar_node(expr.index_);

            auto new_expr = arr_idx_to_ptr_arithmetic(tree_, expr);

            std::swap(tree_.nodes_[node_id], tree_.nodes_[new_expr]);
            tree_.nodes_.pop_back();
            
            break;
        }

        case SemaNodeKind::InitListExpr: {
            const auto& init_list = node.as<InitListExpr>();
            for (auto v : init_list.init_values_)
                desugar_node(v);
            break;
        }

        default:
            break;
    }
}