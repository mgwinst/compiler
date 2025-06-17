#include <iostream>

#include "ast_nodes.h"

int main() {
    auto lhs_node = std::make_unique<Expr>(VariableExpr{"x"});
    auto rhs_node = std::make_unique<Expr>(IntegerLiteralExpr{42});

    auto binary_op_node = std::make_unique<BinaryExpr>('+', std::move(lhs_node), std::move(rhs_node));
}