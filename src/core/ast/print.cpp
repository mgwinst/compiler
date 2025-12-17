#include "ast/print.hpp"

Printer::Printer(const AST& ast) noexcept :
    ast_{ ast } {}

void Printer::print() const noexcept
{
    auto root = ast_.root();
    
    std::println("{}", node_to_str(root, ""));
    
}

std::string Printer::node_to_str(NodeRef ref, std::string indent) const noexcept
{
    const auto& node = ast_[ref];

    switch (node.get_kind()) {
        case NodeKind::CompilationUnitDecl: {
            const auto* comp_unit = node.as<CompilationUnitDecl>();
            // check if (!comp_unit)

            std::string decls_str{};

            for (size_t i{}; i < comp_unit->decls_.size(); i++) {
                decls_str += node_to_str(comp_unit->decls_[i], indent + "    ");
                if (i != comp_unit->decls_.size() - 1)
                    decls_str += "\n\n";
            }

            return indent + std::format("CompilationUnitDecl ({})\n{}", comp_unit->name_, decls_str);
        }

        case NodeKind::VarDecl: {
            const auto* var = node.as<VarDecl>();

            auto decl_type = (var->constness_ == Constness::CONST ? "ConstVarDecl" : "VarDecl");

            if (!var->init_) {
                return indent + std::format("{} ['{}', {}]", decl_type, var->name_, var->type_);
            } else {
                return indent + std::format("{} ['{}', {}]\n{}", decl_type, var->name_, var->type_, node_to_str(*var->init_, indent + "    "));
            }
        }

        case NodeKind::ParamDecl: {
            const auto* param = node.as<ParamDecl>();           

            auto decl_type = (param->constness_ == Constness::CONST ? "ConstParamDecl" : "ParamDecl");
            return indent + std::format("{} ['{}', {}]", decl_type, param->name_, param->type_);
        }

        case NodeKind::FuncDecl: {
            const auto* func = node.as<FuncDecl>();

            std::string param_list_str{};
            std::string param_decls_str{};
            
            if (func->params_.size() > 0) {
                for (size_t i = 0; i < func->params_.size(); i++) {
                    param_list_str += ast_[func->params_[i]].as<ParamDecl>()->type_;

                    param_decls_str += node_to_str(func->params_[i], indent + "    ");
                    if (i != func->params_.size() - 1) {
                        param_list_str += ", ";
                        param_decls_str += '\n';
                    }
                }

                return indent + std::format("FuncDecl '{}' ({}) -> ({})\n{}\n{}", 
                    func->name_, 
                    param_list_str, 
                    func->return_type_, 
                    param_decls_str, 
                    node_to_str(func->body_, indent + "    "));
            } 

            return indent + std::format("FuncDecl '{}' () -> ({})\n{}",
                func->name_, 
                func->return_type_, 
                node_to_str(func->body_, indent + "    "));
        }

        case NodeKind::StructDecl: {
            const auto* s = node.as<StructDecl>();

            std::string field_list_str{};
            for (size_t i = 0; i < s->fields_.size(); i++) {
                field_list_str += node_to_str(s->fields_[i], indent + "    ");
                if (i != s->fields_.size() - 1)
                    field_list_str += '\n';
            }
            
            return indent + std::format("StructDef ['{}']\n{}", s->type_, field_list_str);
        }

        case NodeKind::CompoundStmt: {
            const auto* cstmt = node.as<CompoundStmt>();  

            std::string expr_strs{};
            for (size_t i = 0; i < cstmt->exprs_.size(); i++) {
                expr_strs += node_to_str(cstmt->exprs_[i], indent + "    ");
                if (i != cstmt->exprs_.size() - 1)
                    expr_strs += '\n';
            }           

            return indent + std::format("CompoundStatement\n{}", expr_strs); 
        }

        case NodeKind::ReturnStmt:
        case NodeKind::IfStmt:
        case NodeKind::WhileStmt:
        case NodeKind::ForStmt: {
            return indent + std::format("");
        }

        case NodeKind::IntegerLiteralExpr: {
            const auto* i = node.as<IntegerLiteralExpr>();
            return indent + std::format("IntLiteral ['{}']", i->value_);
        }

        case NodeKind::FloatLiteralExpr: {
            const auto* f = node.as<FloatLiteralExpr>();
            return indent + std::format("FloatLiteral ['{}']", f->value_);
        }

        case NodeKind::CharLiteralExpr: {
            const auto* c = node.as<CharLiteralExpr>();
            return indent + std::format("CharLiteral ['{}']", c->value_);
        }

        case NodeKind::StringLiteralExpr: {
            const auto* str = node.as<StringLiteralExpr>();
            return indent + std::format("StringLiteral ['{}']", str->value_);
        }

        case NodeKind::BooleanLiteralExpr: {
            const auto* b = node.as<BooleanLiteralExpr>();
            return indent + std::format("BoolLiteral ['{}']", b->value_);
        }

        case NodeKind::UnaryExpr: {
            const auto* unary = node.as<UnaryExpr>();

            if (unary->is_postfix_) {
                return indent + std::format("PostfixUnaryOp ['{}']\n{}", unary->op_, node_to_str(unary->operand_, indent + "    "));
            } else {
                return indent + std::format("PrefixUnaryOp ['{}']\n{}", unary->op_, node_to_str(unary->operand_, indent + "    "));
            }
        }

        case NodeKind::BinaryExpr: {
            const auto* binary = node.as<BinaryExpr>();  

            return indent + std::format("BinOp ['{}']\n{}\n{}",
                binary->op_,
                node_to_str(binary->left_, indent + "    "),
                node_to_str(binary->right_, indent + "    "));
        }

        case NodeKind::ReferenceExpr: {
            const auto* ref = node.as<ReferenceExpr>();

            return indent + std::format("RefExpr ['{}']", ref->name_);
        }

        case NodeKind::CallExpr: {
            const auto* call = node.as<CallExpr>();           

            std::string args_str{};
            for (size_t i = 0; i < call->args_.size(); i++) {
                args_str += node_to_str(call->args_[i], indent + "    ");
                if (i != call->args_.size() - 1)
                    args_str += '\n';
            }

            return indent + std::format("CallExpr\n{}\n{}", node_to_str(call->callee_, indent + "    "), args_str);
        }

        case NodeKind::MemberExpr: {
            const auto* m = node.as<MemberExpr>();           

            return indent + std::format("MemberExpr ['.{}']\n{}", 
                m->member_,
                node_to_str(m->base_, indent + "    "));
        }

        case NodeKind::ArraySubscriptExpr: {
            const auto* arr_expr = node.as<ArraySubscriptExpr>();

            return indent + std::format("ArraySubscriptExpr\n{}\n{}", 
                node_to_str(arr_expr->base_, indent + "    "),
                node_to_str(arr_expr->index_, indent + "    "));
        }

        case NodeKind::InitListExpr: {
            const auto* init_list = node.as<InitListExpr>();

            std::string init_list_str{};
            for (size_t i{}; i < init_list->init_values_.size(); i++) {
                init_list_str += node_to_str(init_list->init_values_[i], indent + "    ");
                if (i != init_list->init_values_.size() - 1) {
                    init_list_str += "\n";
                }
            }

            return indent + std::format("InitListExpr\n{}", init_list_str);
        }

        case NodeKind::ExplicitCastExpr:
        case NodeKind::ImplicitCastExpr:
        case NodeKind::NewExpr:
        case NodeKind::DeleteExpr:      
        case NodeKind::ConstructExpr:  
        case NodeKind::DestructExpr: {
            return indent + std::format("");
        }

        default:
            return "parse error";
    }
}