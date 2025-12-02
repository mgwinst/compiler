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
    return std::visit([this, ref, &indent] (auto&& node) -> std::string {
        using NodeType = std::decay_t<decltype(node)>;

        if constexpr (std::is_same_v<NodeType, CompilationUnitDecl>)
        {
            std::string str{};
            for (size_t i = 0; i < node.decls_.size(); i++) {
                auto child_ref = node.decls_[i];
                str += decl_to_str(child_ref, (indent + "    "));
                if (i != node.decls_.size() - 1) {
                    str += '\n';
                }
            }
            return indent + std::format("CompilationUnitDecl ({})\n{}", node.name_, str);
        }
        else if constexpr (std::is_same_v<NodeType, VarDecl>)
        {
            if (!node.init_)
                return indent + std::format("VarDecl ['{}', {}]", node.name_, node.type_);
            else
                return indent + std::format("VarDecl ['{}', {}]\n{}", node.name_, node.type_, expr_to_str(*node.init_, indent + "    "));
        }
        else if constexpr (std::is_same_v<NodeType, ConstVarDecl>)
        {
            return std::format("ConstVarDecl ['{}', {}]", node.name_, node.type_);
        }
        else if constexpr (std::is_same_v<NodeType, ParamDecl>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, FuncDecl>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, StructDecl>)
        {
            return std::format("");
        }
        else
        {
            static_assert(always_false_v<NodeType>, "type not defined in visitor...");
        }
    }, decls_[ref]);
}

std::string AST::expr_to_str(ExprRef ref, std::string indent) const noexcept
{
    return std::visit([this, ref, &indent] (auto&& node) -> std::string {
        using NodeType = std::decay_t<decltype(node)>;

        if constexpr (std::is_same_v<NodeType, CompoundStmt>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, ReturnStmt>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, IfStmt>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, WhileStmt>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, ForStmt>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, IntegerLiteralExpr>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, FloatLiteralExpr>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, CharLiteralExpr>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, StringLiteralExpr>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, BooleanExpr>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, UnaryExpr>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, BinaryExpr>)
        {
            return std::format("BinOp ['{}']\n\t{}\n{}\n", node.op_, expr_to_str(node.left_, indent + "    "), expr_to_str(node.right_, indent + "    "));
        }
        else if constexpr (std::is_same_v<NodeType, ReferenceExpr>)
        {
            return std::format("ReferenceExpr ['{}']", node.name_);
        }
        else if constexpr (std::is_same_v<NodeType, IndexExpr>)
        {
            return std::format("");
        }
        else if constexpr (std::is_same_v<NodeType, CallExpr>)
        {
            return std::format("");
        }
        else
        {
            static_assert(always_false_v<NodeType>, "type not defined in visitor");
        }
    }, exprs_[ref]);
}

