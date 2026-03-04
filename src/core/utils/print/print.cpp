#include "print.hpp"

namespace
{
    std::unordered_map<QualifierKind, std::string> qualkind_to_str {
        {QualifierKind::Const, "const"}
    };
}

std::string type_to_str(const Sema::TypePool& type_pool, TypeRef type_ref)
{
    const auto& type = type_pool.types_[type_ref];

    // STILL NEED QUALIFIER TYPE
    switch (type.get_kind()) {
        case TypeKind::Void: {
            return std::format("void");
        }

        case TypeKind::Byte: {
            return std::format("byte");
        }

        case TypeKind::Bool: {
            return std::format("bool");
        }

        case TypeKind::Integer: {
            const auto& t = type.as<Sema::IntegerType>();
            if (t.is_signed_)
                return std::format("int{}", t.bit_width_);
            return std::format("uint{}", t.bit_width_);
        }

        case TypeKind::Float: {
            const auto& t = type.as<Sema::FloatType>();
                return std::format("float{}", t.bit_width_);
        }

        case TypeKind::Reference: {
            const auto& t = type.as<Sema::ReferenceType>();
            return std::format("{}&", type_to_str(type_pool, t.inner_type_));
        }

        case TypeKind::Pointer: {
            const auto& t = type.as<Sema::PointerType>();
            return std::format("{}*", type_to_str(type_pool, t.inner_type_));
        }

        // FIX: print size
        case TypeKind::Array: {
            const auto& t = type.as<Sema::ArrayType>();
            return std::format("{}[]", type_to_str(type_pool, t.inner_type_));
        }

        case TypeKind::Qualifier: {
            const auto& t = type.as<Sema::QualifierType>();
            return std::format("{} {}", qualkind_to_str[t.kind_], type_to_str(type_pool, t.inner_type_));
        }

        case TypeKind::Function: {
            const auto& t = type.as<Sema::FunctionType>();
            return std::format("Function '{}'", t.name_);
        }

        // include "struct/enum/union" ?
        case TypeKind::Record: {
            const auto& t = type.as<Sema::RecordType>();
            return std::format("{}", t.name_);
        }

        default:
            error_exit("type mismatch");
    }
}





void print(const Sema::SemaTree& sema_tree, const Sema::SemaContext& sema_ctx)
{
    std::println("{}", sema_node_to_str(sema_ctx, sema_tree, sema_tree.root(), ""));
}

std::string sema_node_to_str(const Sema::SemaContext& sema_ctx, const Sema::SemaTree& sema_tree, const SemaNodeRef ref, std::string indent)
{
    const auto& node = sema_tree.nodes_[ref];

    switch (node.get_kind()) {
        case SemaNodeKind::CompilationUnitDecl: {
            const auto& comp_unit = node.as<Sema::CompilationUnitDecl>();

            std::string decls_str{};

            for (auto i = 0uz; i < comp_unit.decls_.size(); i++) {
                decls_str += sema_node_to_str(sema_ctx, sema_tree, comp_unit.decls_[i], indent + "  ");
                if (i != comp_unit.decls_.size() - 1)
                    decls_str += "\n\n";
            }

            return indent + std::format("CompilationUnitDecl ({})\n{}", comp_unit.name_, decls_str);
        }

        case SemaNodeKind::VarDecl: {
            const auto& var = node.as<Sema::VarDecl>();

            auto [name, type] = query_symbol(sema_ctx, var.symbol_);

            if (var.init_)
                return indent + std::format("VarDecl ['{}', {}]\n{}", name, type_to_str(*sema_ctx.type_pool_, type), sema_node_to_str(sema_ctx, sema_tree, *var.init_, indent + "  "));
            else
                return indent + std::format("VarDecl ['{}', {}]", name, type_to_str(*sema_ctx.type_pool_, type));

        }

        case SemaNodeKind::ParamDecl: {
            const auto& param = node.as<Sema::ParamDecl>();           

            auto [name, type] = query_symbol(sema_ctx, param.symbol_);

            return indent + std::format("ParamDecl ['{}', {}]", name, type_to_str(*sema_ctx.type_pool_, type));
        }

        // FuncDecl 'f' (int, int) -> (int)
        case SemaNodeKind::FuncDecl: {
            const auto& func = node.as<Sema::FuncDecl>();

            auto [func_name, func_type] = query_symbol(sema_ctx, func.symbol_);
            auto ret_type = sema_ctx.type_pool_->types_[func_type].as<Sema::FunctionType>().return_type_;
            auto ret_type_str = type_to_str(*sema_ctx.type_pool_, ret_type);

            std::string param_type_list_str{};
            std::string param_decls_str{};
            
            if (func.params_.size() > 0) {
                for (auto i = 0uz; i < func.params_.size(); i++) {
                    const auto& param = sema_ctx.sema_tree_->nodes_[func.params_[i]].as<Sema::ParamDecl>();
                    auto [name, type] = query_symbol(sema_ctx, param.symbol_);
                    param_type_list_str += type_to_str(*sema_ctx.type_pool_, type);

                    param_decls_str += sema_node_to_str(sema_ctx, sema_tree, func.params_[i], indent + "  ");

                    if (i < func.params_.size() - 1) {
                        param_type_list_str += ", ";
                        param_decls_str += '\n';
                    }
                }

                return indent + std::format("FuncDecl '{}' ({}) -> ({})\n{}\n{}",
                    func_name,
                    param_type_list_str,
                    ret_type_str,
                    param_decls_str,
                    sema_node_to_str(sema_ctx, sema_tree, func.body_, indent + "  "));
            }

            return indent + std::format("FuncDecl '{}' () -> ({})\n{}",
                func_name,
                ret_type_str,
                sema_node_to_str(sema_ctx, sema_tree, func.body_, indent + "  "));
        }


        case SemaNodeKind::RecordDecl: {
            const auto& rec = node.as<Sema::RecordDecl>();
            
            std::string field_list_str{};
            for (auto i = 0uz; i < rec.fields_.size(); i++) {
                field_list_str += sema_node_to_str(sema_ctx, sema_tree, rec.fields_[i], indent + "  ");
                if (i != rec.fields_.size() - 1)
                    field_list_str += '\n';
            }
            
            auto [name, type] = query_symbol(sema_ctx, rec.symbol_);

            return indent + std::format("RecordDecl ['{}']\n{}", type_to_str(*sema_ctx.type_pool_, type), field_list_str);
        }

        case SemaNodeKind::CompoundStmt: {
            const auto& cstmt = node.as<Sema::CompoundStmt>();  

            std::string expr_strs{};
            for (auto i = 0uz; i < cstmt.children_.size(); i++) {
                expr_strs += sema_node_to_str(sema_ctx, sema_tree, cstmt.children_[i], indent + "  ");
                if (i != cstmt.children_.size() - 1)
                    expr_strs += '\n';
            }           

            return indent + std::format("CompoundStmt\n{}", expr_strs); 
        }

        case SemaNodeKind::ReturnStmt: {
            const auto& ret_stmt = node.as<Sema::ReturnStmt>();

            return indent + sema_node_to_str(sema_ctx, sema_tree, ret_stmt.value_, indent + "  ");
        }

        case SemaNodeKind::IfStmt: {
            const auto& if_stmt = node.as<Sema::IfStmt>();

            if (if_stmt.else_stmt_) {
                return indent + std::format("IfStmt\n{}\n{}\n{}", 
                    sema_node_to_str(sema_ctx, sema_tree, if_stmt.cond_, indent + "  "),
                    sema_node_to_str(sema_ctx, sema_tree, if_stmt.then_stmt_, indent + "  "),
                    sema_node_to_str(sema_ctx, sema_tree, *if_stmt.else_stmt_, indent + "  "));
            } else {
                return indent + std::format("IfStmt\n{}\n{}", 
                    sema_node_to_str(sema_ctx, sema_tree, if_stmt.cond_, indent + "  "),
                    sema_node_to_str(sema_ctx, sema_tree, if_stmt.then_stmt_, indent + "  "));
            }
        }

        case SemaNodeKind::WhileStmt: {
            const auto& while_stmt = node.as<Sema::WhileStmt>();

            return indent + std::format("WhileStmt\n{}\n{}", 
                sema_node_to_str(sema_ctx, sema_tree, while_stmt.cond_, indent + "  "),
                sema_node_to_str(sema_ctx, sema_tree, while_stmt.body_, indent + "  "));
        }

        case SemaNodeKind::ForStmt: {
            const auto& for_stmt = node.as<Sema::ForStmt>();

            return indent + std::format("ForStmt\n{}\n{}\n{}\n{}",
                sema_node_to_str(sema_ctx, sema_tree, for_stmt.init_, indent + "  "),
                sema_node_to_str(sema_ctx, sema_tree, for_stmt.cond_, indent + "  "),
                sema_node_to_str(sema_ctx, sema_tree, for_stmt.update_, indent + "  "),
                sema_node_to_str(sema_ctx, sema_tree, for_stmt.body_, indent + "  "));
        }

        case SemaNodeKind::IntegerLiteralExpr: {
            const auto& i = node.as<Sema::IntegerLiteralExpr>();
            return indent + std::format("IntLiteral ['{}']", i.value_);
        }

        case SemaNodeKind::FloatLiteralExpr: {
            const auto& f = node.as<Sema::FloatLiteralExpr>();
            return indent + std::format("FloatLiteral ['{}']", f.value_);
        }

        case SemaNodeKind::CharLiteralExpr: {
            const auto& c = node.as<Sema::CharLiteralExpr>();
            return indent + std::format("CharLiteral ['{}']", c.value_);
        }

        case SemaNodeKind::StringLiteralExpr: {
            const auto& str = node.as<Sema::StringLiteralExpr>();
            return indent + std::format("StringLiteral ['{}']", str.value_);
        }

        case SemaNodeKind::BooleanLiteralExpr: {
            const auto& b = node.as<Sema::BooleanLiteralExpr>();
            return indent + std::format("BoolLiteral ['{}']", b.value_);
        }

        case SemaNodeKind::UnaryExpr: {
            const auto& unary = node.as<Sema::UnaryExpr>();

            if (unary.is_postfix_) {
                return indent + std::format("PostfixUnaryOp ['{}']\n{}", unary.op_, sema_node_to_str(sema_ctx, sema_tree, unary.operand_, indent + "  "));
            } else {
                return indent + std::format("PrefixUnaryOp ['{}']\n{}", unary.op_, sema_node_to_str(sema_ctx, sema_tree, unary.operand_, indent + "  "));
            }
        }

        case SemaNodeKind::BinaryExpr: {
            const auto& binary = node.as<Sema::BinaryExpr>();  

            return indent + std::format("BinOp ['{}']\n{}\n{}",
                binary.op_,
                sema_node_to_str(sema_ctx, sema_tree, binary.left_, indent + "  "),
                sema_node_to_str(sema_ctx, sema_tree, binary.right_, indent + "  "));
        }

        case SemaNodeKind::ReferenceExpr: {
            const auto& r = node.as<Sema::ReferenceExpr>();

            auto [name, type] = query_symbol(sema_ctx, r.target_symbol_);

            return indent + std::format("RefExpr ['{}', {}]", name, type_to_str(*sema_ctx.type_pool_, type));
        }

        case SemaNodeKind::CallExpr: {
            const auto& call = node.as<Sema::CallExpr>();           

            std::string args_str{};
            for (auto i = 0uz; i < call.args_.size(); i++) {
                args_str += sema_node_to_str(sema_ctx, sema_tree, call.args_[i], indent + "  ");
                if (i != call.args_.size() - 1)
                    args_str += '\n';
            }

            return indent + std::format("CallExpr\n{}\n{}", sema_node_to_str(sema_ctx, sema_tree, call.callee_, indent + "  "), args_str);
        }

        case SemaNodeKind::MemberExpr: {
            const auto& m = node.as<Sema::MemberExpr>();           

            return indent + std::format("MemberExpr ['.{}']\n{}", 
                m.member_,
                sema_node_to_str(sema_ctx, sema_tree, m.base_, indent + "  "));
        }

        case SemaNodeKind::ArraySubscriptExpr: {
            const auto& arr_expr = node.as<Sema::ArraySubscriptExpr>();

            return indent + std::format("ArraySubscriptExpr\n{}\n{}", 
                sema_node_to_str(sema_ctx, sema_tree, arr_expr.base_, indent + "  "),
                sema_node_to_str(sema_ctx, sema_tree, arr_expr.index_, indent + "  "));
        }

        case SemaNodeKind::InitListExpr: {
            const auto& init_list = node.as<Sema::InitListExpr>();

            std::string init_list_str{};
            for (auto i = 0uz; i < init_list.init_values_.size(); i++) {
                init_list_str += sema_node_to_str(sema_ctx, sema_tree, init_list.init_values_[i], indent + "  ");
                if (i != init_list.init_values_.size() - 1) {
                    init_list_str += "\n";
                }
            }

            return indent + std::format("InitListExpr\n{}", init_list_str);
        }

        case SemaNodeKind::ExplicitCastExpr:
        case SemaNodeKind::ImplicitCastExpr: {
            return indent + std::format("");
        }

        default:
            return "parse error";
    }
}
