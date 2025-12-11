#include <gtest/gtest.h>
#include <fstream>

#include "parser/parser.hpp"
#include "utils/utils.hpp"

TEST(TestParser, Parsing)
{
    auto source_file = get_source_file("parser/sample_program.txt");
    auto parser = Parser{ source_file };
    parser.parse_compilation_unit();
    // parser.ast_.print();
    // parser.diagnostics_.dump_errors();
}