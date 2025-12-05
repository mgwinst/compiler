#include "utils/utils.hpp"
#include "parser/parser.hpp"

int main(int argc, char** argv)
{
    auto source = get_source_file("../../test/test_program.txt");
    auto lexer = Lexer{ source.data };

    while (true) {
        lexer.cur_token_ = lexer.get_token();
        if (lexer.cur_token_.type_ == TokenType::END_OF_FILE) 
            break; 
            
        std::println("{}", lexer.cur_token_.to_string());
    }

    auto parser = Parser{ source };
    parser.parse_compilation_unit();
    parser.ast_.print();
    parser.diagnostics_.dump_errors();
}
