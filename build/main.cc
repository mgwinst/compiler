#include <fstream>
#include <print>
#include <iostream>

#include "lexer/lexer.h"
#include "parser/ast.h"
#include "parser/parser.h"
#include "utils/utils.h"

int main(int argc, char** argv) 
{
    std::string source_text{};

    try {
        source_text = get_source_text("../tests/samples/sample_program.txt");
    } catch (const std::exception& e) {
        std::println(std::cerr, "Error: {}", e.what());
    }
    
    auto parser = Parser{ source_text };

    parser.main_parse(); // each parser object internally manages an AST for each compilation unit, do we want this?
    
    for (const auto& elem : parser.ast.decls) {
        std::println("{}", to_string(elem));
    }


    return 0;
}
