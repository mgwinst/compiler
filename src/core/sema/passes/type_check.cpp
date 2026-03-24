#include "sema_pass.hpp"

#include <cstdlib>

// after sema tree has been built, we should interface 
// with the tree via symbols. The symbols contain all the info
// we need about each node

namespace Sema
{
    TypeId get_type(const SemaContext& ctx, SemaNodeId ref)
    {
        const auto& node = ctx.sema_tree_->nodes_[ref];

        switch (node.get_kind()) {
            case SemaNodeKind::IntegerLiteralExpr: return INT32;
            case SemaNodeKind::FloatLiteralExpr:   return FLOAT32;
            case SemaNodeKind::CharLiteralExpr:    return CHAR;
            case SemaNodeKind::BooleanLiteralExpr: return BOOL;
            case SemaNodeKind::ReferenceExpr:      return node.as<ReferenceExpr>().target_type_id_;
            default: error_exit("get_type()");
        }
    }

    // implicit should not change the symbol or type, it just inserts an implicit cast node to target type

    std::optional<TypeId> check_type(SemaContext& ctx, SemaNodeId ref)
    {
        auto& node = ctx.sema_tree_->nodes_[ref];
        
        switch (node.get_kind()) {
            case SemaNodeKind::CompilationUnitDecl: {
                auto& comp_unit = node.as<CompilationUnitDecl>();
                for (auto decl : comp_unit.decls_)
                    check_type(ctx, decl);
                return std::nullopt;
            }

            case SemaNodeKind::VarDecl: {
                const auto& var = node.as<VarDecl>();
                
                if (var.init_) {
                    auto init_type = check_type(ctx, *var.init_);

                    if (*init_type == ERROR_TYPE)
                        return ERROR_TYPE;

                    if (var.type_id_ != *init_type) {
                        auto err = TypeMismatchError{std::format("type mismatch ({} and {})", var.type_id_, *init_type)};
                        ctx.diagnostics_->register_error(err);
                        return ERROR_TYPE;
                    }

                    return init_type;
                }

                return std::nullopt;
            }

            case SemaNodeKind::FuncDecl: {
                auto& func = node.as<FuncDecl>();
                check_type(ctx, func.body_);
                return std::nullopt;
            }

            case SemaNodeKind::CompoundStmt: {
                auto& compound = node.as<CompoundStmt>();
                for (auto c : compound.children_)
                    check_type(ctx, c);
                return std::nullopt;
            }

            case SemaNodeKind::ReturnStmt: {
                auto& ret = node.as<ReturnStmt>();
                auto ret_type = check_type(ctx, ret.value_);
                
                if (*ret_type == ERROR_TYPE)
                    return ERROR_TYPE;

                return ret_type;
            }

            case SemaNodeKind::IfStmt: {
                auto& ifstmt = node.as<IfStmt>();
                               

                return std::nullopt;
            }

            case SemaNodeKind::WhileStmt: {
                return std::nullopt;
            }

            case SemaNodeKind::IntegerLiteralExpr: {
                return INT32;
            }

            case SemaNodeKind::FloatLiteralExpr: {
                return FLOAT32;
            }

            case SemaNodeKind::CharLiteralExpr: {
                return CHAR;
            }

            case SemaNodeKind::BooleanLiteralExpr: {
                return INT32;
            }

            case SemaNodeKind::UnaryExpr: {
                return std::nullopt;
            }

            case SemaNodeKind::BinaryExpr: {
                auto& binary = node.as<BinaryExpr>();

                auto left_type = check_type(ctx, binary.left_);
                auto right_type = check_type(ctx, binary.right_);
                
                if (*left_type == ERROR_TYPE || *right_type == ERROR_TYPE)
                    return ERROR_TYPE;

                /*
                if (is_bitwise(binary.op_)) {
                    if (!is_integral(left_type) || !is_integral(right_type)) {
                        ...
                    }
                }
                */

                if (*left_type != *right_type) {
                    auto err = TypeMismatchError{std::format("type mismatch ({} and {})", *left_type, *right_type)};
                    ctx.diagnostics_->register_error(err);
                    return ERROR_TYPE;
                }

                return left_type;
            }

            case SemaNodeKind::ReferenceExpr: {
                return get_type(ctx, ref);
            }

            case SemaNodeKind::CallExpr: {
                auto& call = node.as<CallExpr>();

                auto ref = ctx.sema_tree_->nodes_[call.callee_].as<ReferenceExpr>();
                auto& func_type = ctx.type_pool_->get_type(ref.target_type_id_).as<FunctionType>();
                
                if (func_type.params_.size() != call.args_.size()) {
                    std::println("expected {} arguments", func_type.params_.size());
                    return ERROR_TYPE;
                }

                for (auto i = 0uz; i < func_type.params_.size(); i++) {
                    auto param_type = func_type.params_[i];
                    auto arg_type = check_type(ctx, call.args_[i]);
                    if (!arg_type) return ERROR_TYPE;

                    if (param_type != *arg_type) {
                        auto err = TypeMismatchError{std::format("type mismatch ({} and {})", param_type, *arg_type)};
                        ctx.diagnostics_->register_error(err);
                        return ERROR_TYPE;
                    }
                }

                return func_type.return_type_;
            }

            case SemaNodeKind::MemberExpr: {
                auto& expr = node.as<MemberExpr>();

                auto& ref = ctx.sema_tree_->nodes_[expr.base_].as<ReferenceExpr>();

                auto& obj_type = ctx.type_pool_->get_type(ref.target_type_id_).as<RecordType>();
                

                return std::nullopt;
            }

            case SemaNodeKind::ArraySubscriptExpr: {
                return std::nullopt;

            }

            case SemaNodeKind::InitListExpr: {
                return std::nullopt;
            }

            default:
                std::println("{}", (int)node.get_kind());
                return std::nullopt;
        }
    }

    void check_types(SemaContext& ctx)
    {
        check_type(ctx, ctx.sema_tree_->root());
    }

} // Sema