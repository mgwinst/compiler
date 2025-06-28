#include <fstream>

#include "lexer/lexer.h"
#include "parser/ast_nodes.h"
#include "parser/parser.h"
#include "utils/utils.h"

int main(int argc, char** argv) {

    std::string source_text;

    try {
        source_text = get_source_text("../samples/sample_program.c");
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    Parser parser{source_text};
    
    


    return 0;
}