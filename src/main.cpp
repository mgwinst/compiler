#include "core/context/context.hpp"
#include "core/driver/pipeline.hpp"
#include "core/utils/print/print.hpp"

void test()
{
    std::vector<std::string> files{ "../../test/test_program.txt" }; // cli
    CompilerContext compiler_ctx{ files };

    for (auto& file : compiler_ctx.source_files_) {
        
        ModuleContext ctx;

        auto ast = parse(file);
        auto sema_tree = decorate(ctx, ast);
        type_check(ctx, sema_tree);
        desugar(ctx, sema_tree);

        // auto program = linearize(ctx, sema_tree);
        // ...

        PrettyPrinter{ ctx }.print(sema_tree);
    }
}



int main()
{
    test();

}



