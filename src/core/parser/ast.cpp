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
            std::string str{};
            for (size_t i = 0; i < node.decls_.size(); i++) {
                str += decl_to_str(node.decls_[i], (indent + "    "));
                if (i != node.decls_.size() - 1) {
                    str += '\n';
                }
            }
            return indent + std::format("CompilationUnitDecl ({})\n{}", node.name_, str);
        },

        [this, &indent] (const VarDecl& node) {
            std::string str{};
            auto decl_type = (node.constness_ == Constness::CONST ? "ConstVarDecl" : "VarDecl");

            if (!node.init_) {
                return indent + std::format("{} ['{}', {}]", decl_type, node.name_, node.type_);
            } else {
                std::string init_values{};

                if (std::holds_alternative<ExprRef>(*node.init_)) {
                    auto init = std::get<ExprRef>(*node.init_);
                    init_values += expr_to_str(init, indent + "    ");
                } else {
                    auto& init = std::get<std::vector<ExprRef>>(*node.init_);
                    for (size_t i = 0; i < init.size(); i++) {
                        init_values += expr_to_str(init[i], indent + "    ");
                        if (i != init.size() - 1)
                            init_values += '\n';
                    }
                }

                return indent + std::format("{} ['{}', {}]\n{}", decl_type, node.name_, node.type_, init_values);
            }
        },

        [this, &indent] (const ParamDecl& node) {
            auto decl_type = (node.constness_ == Constness::CONST ? "ConstParamDecl" : "ParamDecl");
            return indent + std::format("{} ['{}', {}]", decl_type, node.name_, node.type_);
        },
        
        [this, &indent] (const FuncDecl& node) {
            std::string param_list{};
            for (size_t i = 0; i < node.params_.size(); i++) {
                param_list += std::get<ParamDecl>(decls_[node.params_[i]]).type_;
                if (i != node.params_.size() - 1)
                    param_list += ", ";
            }

            std::string param_decls{};
            for (size_t i = 0; i < node.params_.size(); i++) {
                param_decls += decl_to_str(node.params_[i], indent + "    ");
                if (i != node.params_.size() - 1)
                    param_decls += '\n';
            }
            
            return indent + std::format("FuncDecl '{}' ({}) -> ({})\n{}", node.name_, param_list, node.return_type_, param_decls);
        },

        [this, &indent] (const StructDef& node) {
            std::string field_list;
            for (size_t i = 0; i < node.fields_.size(); i++) {
                field_list += decl_to_str(node.fields_[i], indent + "    ");
                if (i != node.fields_.size() - 1)
                    field_list += '\n';
            }
            
            return indent + std::format("StructDef ['{}']\n{}", node.type_, field_list);
        }
    }, decls_[ref]);
}

std::string AST::expr_to_str(ExprRef ref, std::string indent) const noexcept
{
    return std::visit(Overload{
        [&indent](const CompoundStmt&)       { return indent + std::format(""); },
        [&indent](const ReturnStmt&)         { return indent + std::format(""); },
        [&indent](const IfStmt&)             { return indent + std::format(""); },
        [&indent](const WhileStmt&)          { return indent + std::format(""); },
        [&indent](const ForStmt&)            { return indent + std::format(""); },
        [&indent](const IntegerLiteralExpr&) { return indent + std::format(""); },
        [&indent](const FloatLiteralExpr&)   { return indent + std::format(""); },
        [&indent](const CharLiteralExpr&)    { return indent + std::format(""); },
        [&indent](const StringLiteralExpr&)  { return indent + std::format(""); },
        [&indent](const BooleanExpr&)        { return indent + std::format(""); },
        [&indent](const UnaryExpr&)    { return indent + std::format(""); },

        [this, &indent](const BinaryExpr& node) {
            return indent + std::format("BinOp ['{}']\n{}\n{}\n",
                node.op_,
                expr_to_str(node.left_, indent + " "),
                expr_to_str(node.right_, indent + " "));
        },

        [&indent](const ReferenceExpr& node) {
            return indent + std::format("ReferenceExpr ['{}']", node.name_);
        },

        [&indent](const IndexExpr&) { return indent + std::format(""); },
        [&indent](const CallExpr&)  { return indent + std::format(""); }

    }, exprs_[ref]);
}
