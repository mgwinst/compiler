#include "utils/utils.hpp"
#include "parser/parser.hpp"

int main(int argc, char** argv)
{
    auto source = get_source_file("../../test/test_program.txt");
    auto parser = Parser{ source };

    parser.parse_compilation_unit();
    parser.ast_.print();
    parser.diagnostics_.dump_errors();
}
