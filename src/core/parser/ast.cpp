#include "ast.hpp"

DeclRef AST::root() const noexcept
{
    if (decls_.empty()) {
        std::println(std::cerr, "AST is empty");
        exit(EXIT_FAILURE);
    } else {
        DeclRef root = 0;
        return root;
    }
}

void AST::print() const noexcept
{
    if (decls_.empty()) {
        std::println(std::cerr, "AST is empty");
        exit(EXIT_FAILURE);
    } else {
        std::println("{}", decl_to_str(root(), ""));
    }
}

std::string AST::decl_to_str(DeclRef ref, std::string indent) const noexcept
{
    return std::visit(Overload{
        [this, &indent] (const CompilationUnitDecl& node) {
            std::string decls_str{};
            for (size_t i = 0; i < node.decls_.size(); i++) {
                decls_str += decl_to_str(node.decls_[i], (indent + "    "));
                if (i != node.decls_.size() - 1) {
                    decls_str += "\n\n";
                }
            }
            return indent + std::format("CompilationUnitDecl ({})\n{}", node.name_, decls_str);
        },

        [this, &indent] (const VarDecl& node) {
            auto decl_type = (node.constness_ == Constness::CONST ? "ConstVarDecl" : "VarDecl");

            if (!node.init_) {
                return indent + std::format("{} ['{}', {}]", decl_type, node.name_, node.type_);
            } else {
                std::string init_values_str{};

                if (std::holds_alternative<ExprRef>(*node.init_)) {
                    auto init = std::get<ExprRef>(*node.init_);
                    init_values_str += expr_to_str(init, indent + "    ");
                } else {
                    auto& init = std::get<std::vector<ExprRef>>(*node.init_);
                    for (size_t i = 0; i < init.size(); i++) {
                        init_values_str += expr_to_str(init[i], indent + "    ");
                        if (i != init.size() - 1)
                            init_values_str += '\n';
                    }
                }

                return indent + std::format("{} ['{}', {}]\n{}", decl_type, node.name_, node.type_, init_values_str);
            }
        },

        [this, &indent] (const ParamDecl& node) {
            auto decl_type = (node.constness_ == Constness::CONST ? "ConstParamDecl" : "ParamDecl");
            return indent + std::format("{} ['{}', {}]", decl_type, node.name_, node.type_);
        },
        
        [this, &indent] (const FuncDecl& node) {
            std::string param_list_str{};
            std::string param_decls_str{};
            
            if (node.params_.size() > 0) {
                for (size_t i = 0; i < node.params_.size(); i++) {
                    param_list_str += std::get<ParamDecl>(decls_[node.params_[i]]).type_;
                    param_decls_str += decl_to_str(node.params_[i], indent + "    ");
                    if (i != node.params_.size() - 1) {
                        param_list_str += ", ";
                        param_decls_str += '\n';
                    }
                }

                return indent + std::format("FuncDecl '{}' ({}) -> ({})\n{}\n{}", 
                    node.name_, 
                    param_list_str, 
                    node.return_type_, 
                    param_decls_str, 
                    expr_to_str(node.body_, indent + "    "));
            } 

            return indent + std::format("FuncDecl '{}' () -> ({})\n{}",
                node.name_, 
                node.return_type_, 
                expr_to_str(node.body_, indent + "    "));
        },

        [this, &indent] (const StructDef& node) {
            std::string field_list_str;
            for (size_t i = 0; i < node.fields_.size(); i++) {
                field_list_str += decl_to_str(node.fields_[i], indent + "    ");
                if (i != node.fields_.size() - 1)
                    field_list_str += '\n';
            }
            
            return indent + std::format("StructDef ['{}']\n{}", node.type_, field_list_str);
        }
    }, decls_[ref]);
}

std::string AST::expr_to_str(ExprRef ref, std::string indent) const noexcept
{
    return std::visit(Overload{
        [this, &indent](const CompoundStmt& node) { 
            std::string expr_strs;
            for (size_t i = 0; i < node.exprs_.size(); i++) {
                expr_strs += expr_to_str(node.exprs_[i], indent + "    ");
                if (i != node.exprs_.size() - 1)
                    expr_strs += '\n';
            }           

            return indent + std::format("CompoundStatement\n{}", expr_strs); 
        },

        [&indent] (const ReturnStmt&)         { return indent + std::format(""); },
        [&indent] (const IfStmt&)             { return indent + std::format(""); },
        [&indent] (const WhileStmt&)          { return indent + std::format(""); },
        [&indent] (const ForStmt&)            { return indent + std::format(""); },

        [&indent] (const IntegerLiteralExpr& node) {
            return indent + std::format("IntLiteral [{}]", node.value_); 
        },

        [&indent] (const FloatLiteralExpr node) {
            return indent + std::format("FloatLiteral [{}]", node.value_); 
        },

        [&indent] (const CharLiteralExpr& node) {
            return indent + std::format("CharLiteral [{}]", node.value_); 
        },

        [&indent] (const StringLiteralExpr node) {
            return indent + std::format("StrLiteral [{}]", node.value_); 
        },

        [&indent] (const BooleanLiteralExpr node) {
            return indent + std::format("BoolLiteral [{}]", node.value_); 
        },

        [this, &indent] (const UnaryExpr& node) { 
            if (node.is_postfix_) {
                return indent + std::format("PostfixUnaryOp ['{}']\n{}", node.op_, expr_to_str(node.operand_, indent + "    "));
            } else {
                return indent + std::format("PrefixUnaryOp ['{}']\n{}", node.op_, expr_to_str(node.operand_, indent + "    "));
            }
        },

        [this, &indent] (const BinaryExpr& node) {
            return indent + std::format("BinOp ['{}']\n{}\n{}",
                node.op_,
                expr_to_str(node.left_, indent + "    "),
                expr_to_str(node.right_, indent + "    "));
        },

        [&indent] (const ReferenceExpr& node) {
            return indent + std::format("RefExpr ['{}']", node.name_);
        },

        [this, &indent] (const ArraySubscriptExpr& node) {
            return indent + std::format("ArraySubscriptExpr\n{}\n{}", 
                expr_to_str(node.base_, indent + "    "),
                expr_to_str(node.index_, indent + "    "));
        },

        [this, &indent] (const CallExpr& node)  {
            std::string args_str;
            for (size_t i = 0; i < node.args_.size(); i++) {
                args_str += expr_to_str(node.args_[i], indent + "    ");
                if (i != node.args_.size() - 1)
                    args_str += '\n';
            }

            return indent + std::format("CallExpr\n{}\n{}", expr_to_str(node.callee_, indent + "    "), args_str);
        },

        [this, &indent] (const MemberExpr& node) {
            return indent + std::format("MemberExpr ['.{}']\n{}", 
                expr_to_str(node.member_, ""), 
                expr_to_str(node.base_, indent + "    "));
        }

    }, exprs_[ref]);
}
