#include <docstring/docstring.h>

#include "frontend/parser/parser.hpp"
#include "utils/utils.hpp"

TEST(TestParser, Parsing)
{
    auto module = get_module("parser/sample_program.txt");
    auto parser = Parser{ module };
    parser.parse_compilation_unit();
    // parser.ast_.print();
    // parser.diagnostics_.dump_errors();
}
