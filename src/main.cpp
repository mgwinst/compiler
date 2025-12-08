#include "utils/utils.hpp"
#include "utils/string_utils.hpp"
#include "parser/parser.hpp"

void test_lexer()
{
    auto source = get_source_file("../../test/test_program.txt");
    auto lexer = Lexer{ source.data };

    while (1) {
        Token tok = lexer.get_token();
        if (tok.type_ == TokenType::END_OF_FILE) 
            break;

        std::println("{}", tok.to_string());
    }
}

void test_parser()
{
    auto source = get_source_file("../../test/test_program.txt");

    auto parser = Parser{ source };
    parser.parse_compilation_unit();
    parser.ast_.print();

    std::println();

    parser.diagnostics_.dump_errors();
}

int main(int argc, char** argv)
{
    test_lexer();
    std::println();
    test_parser();
}
