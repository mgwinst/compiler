#include "core/context/context.hpp"
#include "core/driver/pipeline.hpp"
#include "core/utils/print/print.hpp"

void test()
{
    std::vector<std::string> files{ "../../tests/test_program.txt" }; // cli
    CompilerContext compiler_ctx{ files };

    for (auto& file : compiler_ctx.source_files_) {
        ModuleContext ctx;

        AST ast = parse(file);
        SemaTree sema_tree = decorate(ctx, ast);
        type_check(ctx, sema_tree);
        desugar(ctx, sema_tree);
        IR::Program program = lower(ctx, sema_tree);

        PrettyPrinter printer{ ctx };
        printer.print(sema_tree);
        printer.print(program);
    }
}

int main()
{
    test();

}

// THE PROGRAM NEEDS TO STORE ALL VALUE* SO THAT IT CAN FREE THEM