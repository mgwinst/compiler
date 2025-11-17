#include <fstream>
#include <print>
#include <iostream>

#include "core/lexer/lexer.hpp"
#include "core/parser/ast.hpp"
#include "core/parser/parser.hpp"
#include "core/utils/utils.hpp"

int main(int argc, char** argv) 
{
    std::string source_text{};

    try {
        source_text = get_source_text("../tests/samples/sample_program.txt");
    } catch (const std::exception& e) {
        std::println(std::cerr, "Error: {}", e.what());
    }
    
    auto parser = Parser{ source_text };

    parser.main_parse();

    parser.ast.print();

    parser.diagnostics.dump_errors();

    return 0;
}
