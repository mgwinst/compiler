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
        DeclRef root = 0;
        std::println("{}", decl_to_str(root, ""));
    }
}

std::string AST::decl_to_str(DeclRef ref, std::string indent) const noexcept
{
    return std::visit(Overload{
        [this, &indent] (const CompilationUnitDecl& node) {
            std::string str{};
            for (size_t i = 0; i < node.decls_.size(); i++) {
                auto child = node.decls_[i];
                str += decl_to_str(child, (indent + "    "));
                if (i != node.decls_.size() - 1) {
                    str += '\n';
                }
            }
            return indent + std::format("CompilationUnitDecl ({})\n{}", node.name_, str);
        },

        [this, &indent] (const VarDecl& node) {
            auto decl_type = (node.constness_ == Constness::CONST ? "ConstVarDecl" : "VarDecl");

            if (!node.init_) {
                return indent + std::format("{} ['{}', {}]", decl_type, node.name_, node.type_);
            } else {
                return indent + std::format("{} ['{}', {}]\n{}", decl_type, node.name_, node.type_, expr_to_str(*node.init_, indent + "    "));
            }
        },

        [this, &indent] (const ParamDecl& node) {
            auto decl_type = (node.constness_ == Constness::CONST ? "ConstParamDecl" : "ParamDecl");
            return indent + std::format("{} ['{}', {}]", decl_type, node.name_, node.type_);
        },
        
        [this, &indent] (const FuncDecl& node) {
            // FuncDecl 'f' (int, int) -> (int)
            return indent + std::format("");
        },

        [this, &indent] (const StructDecl& node) {
            return indent + std::format("");
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
