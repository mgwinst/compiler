#pragma once

#include <memory>

#include "../sema/sematree.hpp"
#include "../sema/types/type_pool.hpp"
#include "../error/diagnostics.hpp"
#include "../utils/utils.hpp"

struct CompilationContext
{
    std::vector<SourceFile> source_files_;
    
    CompilationContext(std::vector<std::string> files) {
        for (const auto& file : files)
            source_files_.push_back(get_source_file(file));
    }
};

namespace Sema
{
    struct SemaContext
    {
        std::shared_ptr<SemaTree> sema_tree_;
        std::shared_ptr<TypePool> type_pool_;
        std::shared_ptr<SymbolTable> symbol_table_;
        std::shared_ptr<Diagnostics> diagnostics_;

        SemaContext() : 
            sema_tree_{ std::make_shared<SemaTree>() },
            type_pool_{ std::make_shared<TypePool>() },
            symbol_table_{ std::make_shared<SymbolTable>() },
            diagnostics_{ std::make_shared<Diagnostics>() } {}
    };

} // namespace Sema


