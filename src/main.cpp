#include "core/parser/parser.hpp"
#include "core/utils/utils.hpp"
// #include "core/sema/semtree.hpp"

int main()
{
    auto source_file = get_source_file("../../test/test_program.txt");
    Parser parser{ source_file };
    parser.parse_compilation_unit(); // return the AST?

    const auto& ast = parser.ast_;
    ast.print();
    parser.diagnostics_.dump_errors();

    // auto& sem_tree = Sema::SemTree(ast);
    // sem_tree.print();

}



