#include <cstdlib>
#include <functional>

#include "core/context/context.hpp"
#include "core/parser/parser.hpp"
#include "core/utils/print/print.hpp"
#include "core/ast/ast.hpp"
#include "core/sema/passes/sema_pass.hpp"

void debug_print(Sema::SemaContext& ctx)
{
    print_type_pool(ctx);
    print_symbol_table(ctx);
    print_sema_tree(ctx);
}

void test()
{
    std::vector<std::string> files{ "../../test/test_program.txt" }; // cli
    CompilationContext comp_ctx{ files };

    for (auto& file : comp_ctx.source_files_) {
        auto ast = std::invoke([&] -> std::unique_ptr<AST> {
            Parser parser{ file };
            parser.parse_compilation_unit();

            if (parser.diagnostics_.errors_.size() > 0) {
                parser.diagnostics_.dump_errors();
                std::exit(1);
            }

            return std::move(parser.ast_);
        });

        Sema::SemaContext sema_ctx;
        build_sema_tree(sema_ctx, *ast); // return tree? keep isoloted in ctx?
        check_types(sema_ctx);

        sema_ctx.diagnostics_->dump_errors();

        std::println();

        debug_print(sema_ctx);
    }
}

int main()
{
    test();
}



