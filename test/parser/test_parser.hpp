#include "doctest/doctest.h"

#include "frontend/parser/parser.hpp"
#include "utils/utils.hpp"
#include "utils/casting.hpp"

// check source location for one of the examples

inline AST parse(const std::string& source_file)
{
    Module module = get_module(source_file);
    Parser parser{module};
    AST ast = parser.run();
    return ast;
}