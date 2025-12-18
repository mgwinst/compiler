#include "ast.hpp"

#define NODE_LIMIT (1 << 18)

AST::AST() noexcept
{
    nodes_.reserve(NODE_LIMIT);
}

NodeRef AST::root() const noexcept
{
    if (nodes_.empty()) {
        std::println(std::cerr, "AST is empty");
        exit(EXIT_FAILURE);
    } else {
        NodeRef root = 0;
        return root;
    }
}