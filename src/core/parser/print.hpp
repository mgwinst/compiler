#pragma once

#include "parser/ast.hpp"

struct Printer
{
    const AST& ast_;

    Printer(const AST& ast) noexcept;

    void print() const noexcept;
};