#include "core/context/context.hpp"
#include "core/parser/parser.hpp"
#include "core/sema/sematree.hpp"
#include "core/utils/utils.hpp"
#include "core/utils/print/print.hpp"

int main()
{
    CompilationContext comp_ctx;   

    auto source_file = get_source_file("../../test/test_program.txt");
    ParseContext parse_ctx{ source_file };
    parse_ctx.parse_compilation_unit();
    parse_ctx.diagnostics_.dump_errors();
    
    const auto& ast = parse_ctx.ast_;

    Sema::SemaContext sema_ctx;

    sema_ctx.build_sema_tree(ast);

    std::println("new types: {}\n", sema_ctx.type_pool_->types_.size() - 14);
    
    sema_ctx.type_pool_->print();

    std::println();

    std::println("symbol table size: {}\n", sema_ctx.symbol_table_->symbol_pool_.size());
    sema_ctx.symbol_table_->print();

    std::println();

    print(*sema_ctx.sema_tree_, sema_ctx);

}



