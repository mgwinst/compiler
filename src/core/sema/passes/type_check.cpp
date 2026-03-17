#include "sema_pass.hpp"
#include "../../utils/print/print.hpp"

namespace Sema
{
    TypeRef get_type(const SemaContext& ctx, SemaNodeRef ref)
    {
        const auto& node = ctx.sema_tree_->nodes_[ref];

        switch (node.get_kind()) {
            case SemaNodeKind::IntegerLiteralExpr: {
                return INT32_INDEX;
            }

            case SemaNodeKind::FloatLiteralExpr: {
                return FLOAT32_INDEX;
            }

            // byte for now...
            case SemaNodeKind::CharLiteralExpr: {
                return BYTE_INDEX;
            }

            case SemaNodeKind::BooleanLiteralExpr: {
                return BOOL_INDEX;
            }

            case SemaNodeKind::ReferenceExpr: {
                const auto& ref = node.as<ReferenceExpr>();
                return ref.target_type_;
            }

            default:
                error_exit("get_type()");
        }
    }

    void check_type(const Sema::SemaContext& ctx, SemaNodeRef ref)
    {
        const auto& node = ctx.sema_tree_->nodes_[ref];
        
        switch (node.get_kind()) {
            case SemaNodeKind::CompilationUnitDecl: {
                const auto& comp_unit = node.as<CompilationUnitDecl>();
                for (auto decl : comp_unit.decls_) {
                    check_type(ctx, decl);
                }
                break;
            }

            case SemaNodeKind::VarDecl: {
                const auto& var = node.as<VarDecl>();
                
                if (var.init_) {
                    auto init_type = get_type(ctx, *var.init_);
                    if (var.type_ != init_type) {
                        if (type_category[var.type_] != type_category[init_type]) {
                            auto type_error = TypeError{ std::format("cannot initialize a variable of type '{}' with '{}'",
                                type_to_str(*ctx.type_pool_, var.type_), type_to_str(*ctx.type_pool_, init_type)) };
                            ctx.diagnostics_->register_error(type_error);
                        } else {
                            
                        }

                    }
                }
                break;
            }

            case SemaNodeKind::BinaryExpr: {
                const auto& binary = node.as<BinaryExpr>();
                auto left_type = get_type(ctx, binary.left_);
                auto right_type = get_type(ctx, binary.right_);

                if (left_type != right_type) {
                    if (type_category[left_type] != type_category[right_type]) {
                        auto type_error = TypeError{ std::format("invalid operands to binary expression ({} and {})", 
                            type_to_str(*ctx.type_pool_, left_type), type_to_str(*ctx.type_pool_, right_type)) };
                        ctx.diagnostics_->register_error(type_error);
                    }
                }
                break;
            }

            default:
                break;
        }
    }

    void check_types(SemaContext& ctx)
    {
        check_type(ctx, ctx.sema_tree_->root());
    }

} // Sema