#pragma once

#include "ast/ast.hpp"

class Printer
{
public:
    Printer(const AST& ast) noexcept;

    void print() const noexcept;

private:
    const AST& ast_;

    std::string node_to_str(NodeRef ref, std::string indent) const noexcept;
};