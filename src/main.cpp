#include "core/context/context.hpp"
#include "core/driver/pipeline.hpp"
#include "core/utils/print/print.hpp"
#include "core/middleend/analysis/dominator.hpp"
#include "core/middleend/simplify/simplify.hpp"


void test()
{
    std::vector<std::string> files{ "../../tests/test_program.txt" }; // cli
    CompilerContext compiler_ctx{ files };

    for (auto& file : compiler_ctx.source_files_) {
        ModuleContext ctx;

        // ctx should always be the last argument in calls

        AST ast = parse(file);
        SemaTree sema_tree = decorate(ctx, ast);
        type_check(ctx, sema_tree);
        desugar(ctx, sema_tree);


        // when we dump IR, we prob want to emit non simplified, pre mem2reg IR
        IR::Program program = lower(ctx, sema_tree);

        PrettyPrinter printer{ ctx };
        printer.print(sema_tree);
        printer.print(program);

        std::println();

        simplify(program);
        printer.print(program);
        
    }
}

int main()
{
    test();
}
