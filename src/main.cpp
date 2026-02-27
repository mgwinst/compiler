#include "core/context/context.hpp"
#include "core/parser/parser.hpp"
#include "core/sema/sematree.hpp"
#include "core/utils/utils.hpp"
// #include "core/sema/semtree.hpp"

int main()
{
    CompilationContext comp_ctx;   

    auto source_file = get_source_file("../../test/test_program.txt");
    ParseContext parse_ctx{ source_file };
    parse_ctx.parse_compilation_unit();
    parse_ctx.diagnostics_.dump_errors();
    
    const auto& ast = parse_ctx.ast_;
    // ast.print();

    Sema::SemaContext sema_ctx;
    std::println("pre: {}", sema_ctx.type_pool_->types_.size());

    sema_ctx.build_sema_tree(ast);

    //std::println("{}", sema_ctx.symbol_table_->symbol_pool_.size());
    std::println("post: {}", sema_ctx.type_pool_->types_.size());

}



