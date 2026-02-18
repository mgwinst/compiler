#include <print>
#include <iostream>

#include "ast.hpp"

#define NODE_LIMIT (1 << 18)

AST::AST() noexcept
{
    nodes_.reserve(NODE_LIMIT);
}

ASTNodeRef AST::root() const noexcept
{
    if (nodes_.empty()) {
        std::println(std::cerr, "AST is empty");
        exit(EXIT_FAILURE);
    } else {
        ASTNodeRef root = 0;
        return root;
    }
}

void AST::print() const noexcept
{
    std::println("{}", node_to_str(root(), ""));
}

std::string AST::node_to_str(ASTNodeRef ref, std::string indent) const noexcept
{
    const auto& node = nodes_[ref];

    switch (node.get_kind()) {
        case ASTNodeKind::CompilationUnitDecl: {
            const auto& comp_unit = node.as<SyntaxTree::CompilationUnitDecl>();

            std::string decls_str{};

            for (auto i = 0uz; i < comp_unit.decls_.size(); i++) {
                decls_str += node_to_str(comp_unit.decls_[i], indent + "  ");
                if (i != comp_unit.decls_.size() - 1)
                    decls_str += "\n\n";
            }

            return indent + std::format("CompilationUnitDecl ({})\n{}", comp_unit.name_, decls_str);
        }

        case ASTNodeKind::VarDecl: {
            const auto& var = node.as<SyntaxTree::VarDecl>();

            auto decl_type = (var.is_const_ ? "ConstVarDecl" : "VarDecl");

            if (!var.init_) {
                return indent + std::format("{} ['{}', {}]", decl_type, var.name_, var.type_);
            } else {
                return indent + std::format("{} ['{}', {}]\n{}", decl_type, var.name_, var.type_, node_to_str(*var.init_, indent + "  "));
            }
        }

        case ASTNodeKind::ParamDecl: {
            const auto& param = node.as<SyntaxTree::ParamDecl>();           

            auto decl_type = (param.is_const_ ? "ConstParamDecl" : "ParamDecl");
            return indent + std::format("{} ['{}', {}]", decl_type, param.name_, param.type_);
        }

        case ASTNodeKind::FuncDecl: {
            const auto& func = node.as<SyntaxTree::FuncDecl>();

            std::string param_list_str{};
            std::string param_decls_str{};
            
            if (func.params_.size() > 0) {
                for (auto i = 0uz; i < func.params_.size(); i++) {
                    param_list_str += nodes_[func.params_[i]].as<SyntaxTree::ParamDecl>().type_;

                    param_decls_str += node_to_str(func.params_[i], indent + "  ");
                    if (i != func.params_.size() - 1) {
                        param_list_str += ", ";
                        param_decls_str += '\n';
                    }
                }

                return indent + std::format("FuncDecl '{}' ({}) -> ({})\n{}\n{}", 
                    func.name_, 
                    param_list_str, 
                    func.return_type_, 
                    param_decls_str, 
                    node_to_str(func.body_, indent + "  "));
            } 

            return indent + std::format("FuncDecl '{}' () . ({})\n{}",
                func.name_, 
                func.return_type_, 
                node_to_str(func.body_, indent + "  "));
        }

        case ASTNodeKind::StructDecl: {
            const auto& s = node.as<SyntaxTree::StructDecl>();

            std::string field_list_str{};
            for (auto i = 0uz; i < s.fields_.size(); i++) {
                field_list_str += node_to_str(s.fields_[i], indent + "  ");
                if (i != s.fields_.size() - 1)
                    field_list_str += '\n';
            }
            
            return indent + std::format("StructDef ['{}']\n{}", s.type_, field_list_str);
        }

        case ASTNodeKind::CompoundStmt: {
            const auto& cstmt = node.as<SyntaxTree::CompoundStmt>();  

            std::string expr_strs{};
            for (auto i = 0uz; i < cstmt.exprs_.size(); i++) {
                expr_strs += node_to_str(cstmt.exprs_[i], indent + "  ");
                if (i != cstmt.exprs_.size() - 1)
                    expr_strs += '\n';
            }           

            return indent + std::format("CompoundStmt\n{}", expr_strs); 
        }

        case ASTNodeKind::ReturnStmt: {
            const auto& ret_stmt = node.as<SyntaxTree::ReturnStmt>();

            return indent + node_to_str(ret_stmt.value_, indent + "  ");
        }

        case ASTNodeKind::IfStmt: {
            const auto& if_stmt = node.as<SyntaxTree::IfStmt>();

            if (if_stmt.else_stmt_) {
                return indent + std::format("IfStmt\n{}\n{}\n{}", 
                    node_to_str(if_stmt.cond_, indent + "  "),
                    node_to_str(if_stmt.then_stmt_, indent + "  "),
                    node_to_str(*if_stmt.else_stmt_, indent + "  "));
            } else {
                return indent + std::format("IfStmt\n{}\n{}", 
                    node_to_str(if_stmt.cond_, indent + "  "),
                    node_to_str(if_stmt.then_stmt_, indent + "  "));
            }
        }

        case ASTNodeKind::WhileStmt: {
            const auto& while_stmt = node.as<SyntaxTree::WhileStmt>();

            return indent + std::format("WhileStmt\n{}\n{}", 
                node_to_str(while_stmt.cond_, indent + "  "),
                node_to_str(while_stmt.body_, indent + "  "));
        }

        case ASTNodeKind::ForStmt: {
            const auto& for_stmt = node.as<SyntaxTree::ForStmt>();

            return indent + std::format("ForStmt\n{}\n{}\n{}\n{}",
                node_to_str(for_stmt.init_, indent + "  "),
                node_to_str(for_stmt.cond_, indent + "  "),
                node_to_str(for_stmt.update_, indent + "  "),
                node_to_str(for_stmt.body_, indent + "  "));
        }

        case ASTNodeKind::IntegerLiteralExpr: {
            const auto& i = node.as<SyntaxTree::IntegerLiteralExpr>();
            return indent + std::format("IntLiteral ['{}']", i.value_);
        }

        case ASTNodeKind::FloatLiteralExpr: {
            const auto& f = node.as<SyntaxTree::FloatLiteralExpr>();
            return indent + std::format("FloatLiteral ['{}']", f.value_);
        }

        case ASTNodeKind::CharLiteralExpr: {
            const auto& c = node.as<SyntaxTree::CharLiteralExpr>();
            return indent + std::format("CharLiteral ['{}']", c.value_);
        }

        case ASTNodeKind::StringLiteralExpr: {
            const auto& str = node.as<SyntaxTree::StringLiteralExpr>();
            return indent + std::format("StringLiteral ['{}']", str.value_);
        }

        case ASTNodeKind::BooleanLiteralExpr: {
            const auto& b = node.as<SyntaxTree::BooleanLiteralExpr>();
            return indent + std::format("BoolLiteral ['{}']", b.value_);
        }

        case ASTNodeKind::UnaryExpr: {
            const auto& unary = node.as<SyntaxTree::UnaryExpr>();

            if (unary.is_postfix_) {
                return indent + std::format("PostfixUnaryOp ['{}']\n{}", unary.op_, node_to_str(unary.operand_, indent + "  "));
            } else {
                return indent + std::format("PrefixUnaryOp ['{}']\n{}", unary.op_, node_to_str(unary.operand_, indent + "  "));
            }
        }

        case ASTNodeKind::BinaryExpr: {
            const auto& binary = node.as<SyntaxTree::BinaryExpr>();  

            return indent + std::format("BinOp ['{}']\n{}\n{}",
                binary.op_,
                node_to_str(binary.left_, indent + "  "),
                node_to_str(binary.right_, indent + "  "));
        }

        case ASTNodeKind::ReferenceExpr: {
            const auto& r = node.as<SyntaxTree::ReferenceExpr>();

            return indent + std::format("RefExpr ['{}']", r.name_);
        }

        case ASTNodeKind::CallExpr: {
            const auto& call = node.as<SyntaxTree::CallExpr>();           

            std::string args_str{};
            for (auto i = 0uz; i < call.args_.size(); i++) {
                args_str += node_to_str(call.args_[i], indent + "  ");
                if (i != call.args_.size() - 1)
                    args_str += '\n';
            }

            return indent + std::format("CallExpr\n{}\n{}", node_to_str(call.callee_, indent + "  "), args_str);
        }

        case ASTNodeKind::MemberExpr: {
            const auto& m = node.as<SyntaxTree::MemberExpr>();           

            return indent + std::format("MemberExpr ['.{}']\n{}", 
                m.member_,
                node_to_str(m.base_, indent + "  "));
        }

        case ASTNodeKind::ArraySubscriptExpr: {
            const auto& arr_expr = node.as<SyntaxTree::ArraySubscriptExpr>();

            return indent + std::format("ArraySubscriptExpr\n{}\n{}", 
                node_to_str(arr_expr.base_, indent + "  "),
                node_to_str(arr_expr.index_, indent + "  "));
        }

        case ASTNodeKind::InitListExpr: {
            const auto& init_list = node.as<SyntaxTree::InitListExpr>();

            std::string init_list_str{};
            for (auto i = 0uz; i < init_list.init_values_.size(); i++) {
                init_list_str += node_to_str(init_list.init_values_[i], indent + "  ");
                if (i != init_list.init_values_.size() - 1) {
                    init_list_str += "\n";
                }
            }

            return indent + std::format("InitListExpr\n{}", init_list_str);
        }

        case ASTNodeKind::ExplicitCastExpr:
        case ASTNodeKind::ImplicitCastExpr:
        case ASTNodeKind::NewExpr:
        case ASTNodeKind::DeleteExpr:      
        case ASTNodeKind::ConstructExpr:  
        case ASTNodeKind::DestructExpr: {
            return indent + std::format("");
        }

        default:
            return "parse error";
    }
}