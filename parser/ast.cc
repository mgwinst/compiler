#include <iostream>

#include "ast_nodes.h"

using namespace AST;

int main() {
    auto lhs_node = std::make_unique<Expr>(std::in_place_type<VariableExpr>, "a");
    auto rhs_node = std::make_unique<Expr>(std::in_place_type<VariableExpr>, "b");
    auto binary_op_node = std::make_unique<Expr>(std::in_place_type<BinaryExpr>, '+', std::move(lhs_node), std::move(rhs_node));

    auto str = AST::to_string(*binary_op_node);
    std::cout << str << std::endl;
}