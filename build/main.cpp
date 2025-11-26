#include <fstream>
#include <print>
#include <iostream>

#include "core/lexer/lexer.hpp"
#include "core/parser/ast.hpp"
#include "core/parser/parser.hpp"
#include "core/utils/utils.hpp"

int main(int argc, char** argv) 
{   
    auto source_text = get_source_text("../tests/samples/sample_program.txt");
    /*
    auto source_text2 = get_source_text("somefile.txt");
    auto source_text3 = get_source_text("somefile.txt");
    auto source_text4 = get_source_text("somefile.txt");
    auto source_text5 = get_source_text("somefile.txt");
    */
    
    auto parser = Parser{ source_text };

    parser.main_parse();

    parser.ast_.print();

    parser.diagnostics_.dump_errors();

    return 0;
}
