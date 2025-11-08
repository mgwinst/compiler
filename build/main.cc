#include <fstream>

#include "lexer/lexer.h"
#include "parser/ast.h"
#include "parser/parser.h"
#include "utils/utils.h"

int main(int argc, char** argv) 
{
    std::string source_text{};

    try {
        source_text = get_source_text("../tests/samples/sample_program");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    auto lexer = Lexer{ source_text };

    while (true) {
        lexer.cur_token = lexer.get_token();
        std::println("{}", lexer.cur_token.to_string());
        if (lexer.cur_token.type == TokenType::END_OF_FILE)
            break;
    }

    return 0;
}
