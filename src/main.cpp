#include "utils/utils.hpp"
#include "parser/parser.hpp"

int main(int argc, char** argv)
{
    auto source = get_source_file("../../test/test_program.txt");
    auto lexer = Lexer{ source.data };

    while (1) {
        Token tok = lexer.get_token();
        if (tok.type_ == TokenType::END_OF_FILE) 
            break;

        std::println("{}", tok.to_string());
    }
    
    std::println();

    auto parser = Parser{ source };
    parser.parse_compilation_unit();
    parser.ast_.print();

    std::println();   

    parser.diagnostics_.dump_errors();
    
    std::println();
}
