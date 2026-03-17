#include <cstdlib>
#include <functional>

#include "core/context/context.hpp"
#include "core/parser/parser.hpp"
#include "core/utils/print/print.hpp"
#include "core/ast/ast.hpp"
#include "core/sema/passes/sema_pass.hpp"

/*
void test_lexer()
{
    SourceFile source_file = get_source_file("../../test/test_program.txt");
    Lexer lexer{source_file.data};

    int i = 0;
    while (1) {
        Token tok = lexer.get_token();
        if (tok.type_ == TokenType::END_OF_FILE) 
            break;

        std::print("<{}>     ", tok.source_line_);
        std::print("[{}]\n", tok.to_string());

        i++;
    }
}
*/

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

        if (sema_ctx.diagnostics_->errors_.size() > 0) {
            sema_ctx.diagnostics_->dump_errors();
            std::exit(1);
        }

        check_types(sema_ctx);

        if (sema_ctx.diagnostics_->errors_.size() > 0) {
            sema_ctx.diagnostics_->dump_errors();
            std::exit(1);
        }

        desugar(sema_ctx);

        debug_print(sema_ctx);
    }
}

int main()
{
    // test_lexer();
    test();

}



