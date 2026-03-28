#include "sema_pass.hpp"
#include "helpers.hpp"
#include "../../utils/print/print.hpp"

#include <ranges>

// after sema tree has been built, we should interface 
// with the tree via symbols. The symbols contain all the info
// we need about each node

namespace Sema
{
    // implicit conversion should not change the symbol or type, it just inserts an implicit cast node to target type

    std::optional<TypeId> check_type(SemaContext& ctx, SemaNodeId node_id)
    {
        // bool ok; // maybe we set this, then at end of expression type check, check the value and if !ok, return ERROR_TYPE 

        auto& node = ctx.sema_tree_->nodes_[node_id];
        
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
                    auto init_type_id = check_type(ctx, *var.init_);

                    if (*init_type_id == ERROR_TYPE)
                        return ERROR_TYPE;

                    if (var.type_id_ != *init_type_id) {
                        auto err = TypeError{std::format("type mismatch ({} and {})", type_to_str(ctx, var.type_id_), type_to_str(ctx, *init_type_id)), var.source_loc_};
                        ctx.diagnostics_->register_error(err);
                        return ERROR_TYPE;
                    }

                    return init_type_id;
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
                auto ret_type_id = check_type(ctx, ret.value_);
                
                if (*ret_type_id == ERROR_TYPE)
                    return ERROR_TYPE;

                return ret_type_id;
            }

            case SemaNodeKind::IfStmt: {
                auto& ifstmt = node.as<IfStmt>();
                
                auto cond_type_id = check_type(ctx, ifstmt.cond_);
                if (cond_type_id == ERROR_TYPE) return ERROR_TYPE;

                if (!convertible_to_boolean(ctx.type_pool_->get_type(*cond_type_id))) {
                    auto err = TypeError{std::format("condition expression must be of boolean or scalar type")};
                    ctx.diagnostics_->register_error(err);
                    return ERROR_TYPE;
                }

                check_type(ctx, ifstmt.then_stmt_); // check types in body, then_stmt -> if body

                if (ifstmt.else_stmt_) {
                    auto else_cond_type_id = check_type(ctx, *ifstmt.else_stmt_);
                    if (else_cond_type_id == ERROR_TYPE) return ERROR_TYPE;
                
                    if (!convertible_to_boolean(ctx.type_pool_->get_type(*else_cond_type_id))) {
                        auto err = TypeError{std::format("condition expression must be of boolean or scalar type")};
                        ctx.diagnostics_->register_error(err);
                        return ERROR_TYPE;
                    }
                }

                return std::nullopt;
            }

            case SemaNodeKind::WhileStmt: {
                auto& while_stmt = node.as<WhileStmt>();

                auto cond_type_id = check_type(ctx, while_stmt.cond_);
                if (cond_type_id == ERROR_TYPE) return ERROR_TYPE;
                
                if (!convertible_to_boolean(ctx.type_pool_->get_type(*cond_type_id))) {
                    auto err = TypeError{std::format("condition expression must be of boolean or scalar type")};
                    ctx.diagnostics_->register_error(err);
                    return ERROR_TYPE;
                }

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
                auto& unary = node.as<UnaryExpr>();
                
                auto operand_type_id = check_type(ctx, unary.operand_);
                if (*operand_type_id == ERROR_TYPE) return ERROR_TYPE;

                const Type& operand_type = ctx.type_pool_->get_type(*operand_type_id);

                if (unary.op_ == "&") {
                    return ctx.type_pool_->get_or_create<PointerType>(*operand_type_id);
                } else if (unary.op_ == "*") {
                    if (operand_type.get_kind() != TypeKind::Pointer) {
                        auto err = TypeError{std::format("dereferencing non-pointer type {}", type_to_str(ctx, *operand_type_id)), unary.source_loc_};
                        ctx.diagnostics_->register_error(err);
                        return ERROR_TYPE;
                    }
                } else if (unary.op_ == "!") {
                    if (!convertible_to_boolean(operand_type)) {
                        auto err = TypeError{std::format("invalid argument type '{}' to unary expression '!'", type_to_str(ctx, *operand_type_id)), unary.source_loc_};
                        ctx.diagnostics_->register_error(err);
                        return ERROR_TYPE;
                    }
                    return BOOL;
                } else if (unary.op_ == "~") {
                    if (operand_type.get_kind() != TypeKind::Integer) {
                        auto err = TypeError{std::format("non-integral type '{}' to unary expression '~'", type_to_str(ctx, *operand_type_id)), unary.source_loc_};
                        ctx.diagnostics_->register_error(err);
                        return ERROR_TYPE;
                    }
                } else if (unary.op_ == "++" || unary.op_ == "--") {
                    if (!is_int_or_ptr(operand_type) || is_const(operand_type)) {
                        auto err = TypeError{std::format("can't '{}' value of type ({})", unary.op_, type_to_str(ctx, *operand_type_id)), unary.source_loc_};
                        ctx.diagnostics_->register_error(err);
                        return ERROR_TYPE;
                    }
                }

                return operand_type_id;
            }

            // FIX BIN OP RETURN TYPES (probably need a table for pattern matching...)
            case SemaNodeKind::BinaryExpr: {
                auto& binary = node.as<BinaryExpr>();

                auto left_type = check_type(ctx, binary.left_);
                auto right_type = check_type(ctx, binary.right_);
                
                if (*left_type == ERROR_TYPE || *right_type == ERROR_TYPE)
                    return ERROR_TYPE;

                if (*left_type != *right_type) {
                    auto err = TypeError{std::format("type mismatch ({} and {})", type_to_str(ctx, *left_type), type_to_str(ctx, *right_type)), binary.source_loc_};
                    ctx.diagnostics_->register_error(err);
                    return ERROR_TYPE;
                }

                /*
                if (is_bitwise_op(binary.op_)) {
                    if (!is_integral(*left_type) || !is_integral(*right_type)) {
                        auto err = TypeError{std::format("bitwise ops require integral type operands", type_to_str(ctx, *left_type), type_to_str(ctx, *right_type)), binary.source_loc_};
                        ctx.diagnostics_->register_error(err);
                        return ERROR_TYPE;
                    }
                } else if (is_logical_op(binary.op_)) {
                    return BOOL;
                } else if (is_relational_op(binary.op_)) {
                    
                }
                */

                return left_type;
            }

            case SemaNodeKind::ReferenceExpr: {
                const auto& ref = ctx.sema_tree_->nodes_[node_id].as<ReferenceExpr>();
                return ref.target_type_id_;
            }

            case SemaNodeKind::CallExpr: {
                auto& call = node.as<CallExpr>();

                auto call_ref = ctx.sema_tree_->nodes_[call.callee_].as<ReferenceExpr>();
                auto& func_type = ctx.type_pool_->get_type(call_ref.target_type_id_).as<FunctionType>();
                
                if (func_type.params_.size() != call.args_.size()) {
                    auto err = InvalidArguments{std::format("expected '{}' arguments in call expression", func_type.params_.size()), call.source_loc_};
                    ctx.diagnostics_->register_error(err);
                    return ERROR_TYPE;
                }

                for (auto [param_type, arg] : std::views::zip(func_type.params_, call.args_)) {
                    auto arg_type = check_type(ctx, arg);
                    if (!arg_type) return ERROR_TYPE;

                    if (param_type != *arg_type) {
                        auto err = TypeError{std::format("passing '{}' to parameter of incompatible type '{}'", type_to_str(ctx, *arg_type), type_to_str(ctx, param_type)), call.source_loc_};
                        ctx.diagnostics_->register_error(err);
                        return ERROR_TYPE;
                    }
                }

                return func_type.return_type_;
            }

            case SemaNodeKind::MemberExpr: {
                auto& expr = node.as<MemberExpr>();
                auto& rec_ref = ctx.sema_tree_->nodes_[expr.base_].as<ReferenceExpr>();
                auto& rec_type = ctx.type_pool_->get_type(rec_ref.target_type_id_).as<RecordType>();
                
                if (Field* field = rec_type.lookup_field(expr.member_); !field) {
                    auto err = TypeError{std::format("no member named '{}' in '{}'", expr.member_, type_to_str(ctx, rec_ref.target_type_id_)), expr.source_loc_};
                    ctx.diagnostics_->register_error(err);
                    return ERROR_TYPE;
                } else {
                    return field->type_;
                }
            }

            case SemaNodeKind::ArraySubscriptExpr: {
                auto& expr = node.as<ArraySubscriptExpr>();
                auto& arr_ref = ctx.sema_tree_->nodes_[expr.base_].as<ReferenceExpr>();
                auto& arr_symbol = ctx.symbol_table_->get_symbol(arr_ref.target_symbol_);
                auto& arr_type = ctx.type_pool_->get_type(arr_symbol.type_id_).as<ArrayType>();

                return arr_type.inner_type_;
            }

            case SemaNodeKind::InitListExpr: {
                return std::nullopt;
            }

            default:
                return std::nullopt;
        }
    }

    void check_types(SemaContext& ctx)
    {
        check_type(ctx, ctx.sema_tree_->root());
    }

} // Sema