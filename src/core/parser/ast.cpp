#include "ast.hpp"

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