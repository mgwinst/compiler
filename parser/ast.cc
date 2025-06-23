#include <iostream>

#include "ast_nodes.h"

using namespace AST;

int main() {
    auto lhs_node_a = std::make_unique<Expr>(std::in_place_type<VariableExpr>, "a");
    auto rhs_node_b = std::make_unique<Expr>(std::in_place_type<VariableExpr>, "b");
    auto binary_op_node1 = std::make_unique<Expr>(std::in_place_type<BinaryExpr>, '>', std::move(lhs_node_a), std::move(rhs_node_b));

    auto decl_var1 = std::make_unique<Expr>(std::in_place_type<VariableDeclExpr>, "int32", "var1");
    auto decl_var2 = std::make_unique<Expr>(std::in_place_type<VariableDeclExpr>, "float32", "var2");

    auto lhs_node_x = std::make_unique<Expr>(std::in_place_type<VariableExpr>, "x");
    auto rhs_node_y = std::make_unique<Expr>(std::in_place_type<VariableExpr>, "y");
    auto binary_op_node2 = std::make_unique<Expr>(std::in_place_type<BinaryExpr>, '+', std::move(lhs_node_x), std::move(rhs_node_y));

    std::vector<std::unique_ptr<Expr>> v;
    v.push_back(std::move(binary_op_node2)),
    v.push_back(std::move(decl_var1));
    v.push_back(std::move(decl_var2));

    auto if_statement = std::make_unique<Expr>(
        std::in_place_type<IfExpr>, 
        std::move(binary_op_node1),
        std::move(v)
    );



    auto s = AST::to_string(*if_statement);

    std::cout << s << std::endl;
}

