#include <fstream>
#include <print>
#include <iostream>

#include "core/lexer/lexer.hpp"
#include "core/parser/ast.hpp"
#include "core/parser/parser.hpp"
#include "core/utils/utils.hpp"

int main(int argc, char** argv) 
{   
    auto source_file = get_source_file("../tests/samples/sample_program.txt");

    auto parser = Parser{ source_file };

    parser.parse_compilation_unit();

    parser.ast_.print();

    parser.diagnostics_.dump_errors();

    return 0;
}
