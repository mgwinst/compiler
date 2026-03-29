#include <functional>

#include "core/context/context.hpp"
#include "core/frontend/error/diagnostics.hpp"
#include "core/frontend/parser/parser.hpp"
#include "core/frontend/ast/ast.hpp"
#include "core/frontend/sema/passes/sema_pass.hpp"
#include "core/utils/print/print.hpp"

void debug_print(Sema::SemaContext& ctx)
{
    // print_type_pool(ctx);
    // print_symbol_table(ctx);
    print_sema_tree(ctx);
}

void test()
{
    std::vector<std::string> files{ "../../test/test_program.txt" }; // cli
    CompilationContext comp_ctx{ files };

    auto dump_errors = [](Diagnostics& diagnostics) {
        if (diagnostics.errors_.size() > 0) {
            diagnostics.dump_errors();
            std::exit(1);
        }
    };

    for (auto& file : comp_ctx.source_files_) {
        auto ast = std::invoke([&] -> std::unique_ptr<AST> {
            Parser parser{ file };
            parser.parse_compilation_unit();

            dump_errors(parser.diagnostics_);

            return std::move(parser.ast_);
        });

        Sema::SemaContext sema_ctx;
        build_sema_tree(sema_ctx, *ast);

        dump_errors(*sema_ctx.diagnostics_);

        check_types(sema_ctx);

        dump_errors(*sema_ctx.diagnostics_);

        desugar(sema_ctx);

        debug_print(sema_ctx);
    }
}

int main()
{
    test();

}



