#include <iostream>

#include "ast_nodes.h"

using namespace AST;

int main() {
    auto var_node = std::make_unique<AST::Expr>(std::in_place_type<AST::VariableExpr>, "a");
    auto rhs_node = std::make_unique<AST::Expr>(std::in_place_type<AST::VariableExpr>, "b");
    // auto binary_op_node = std::make_unique<AST::Expr>(std::in_place_type<AST::BinaryExpr>, '+', std::move(lhs_node), std::move(rhs_node));

    auto var_decl_a = std::make_unique<Expr>(std::in_place_type<VariableDeclExpr>, "int32", "a");
    auto var_decl_b = std::make_unique<Expr>(std::in_place_type<VariableDeclExpr>, "float32", "b");

    auto unary_node = std::make_unique<Expr>(std::in_place_type<UnaryExpr>, "&", std::move(var_node));

    auto s = AST::to_string(*unary_node);

    std::cout << s << std::endl;
}

