#include "doctest/doctest.h"

#include "frontend/parser/parser.hpp"
#include "frontend/translation_unit.hpp"
#include "utils/casting.hpp"

// check source location for one of the examples

inline AST parse(std::string_view file_path)
{
    TranslationUnit translation_unit{ file_path };
    Parser parser{translation_unit};
    AST ast = parser.run();
    return ast;
}
