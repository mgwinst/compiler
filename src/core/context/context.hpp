#pragma once

#include <memory>

#include "../sema/sematree.hpp"
#include "../sema/types/type_pool.hpp"
#include "../error/diagnostics.hpp"
#include "../utils/utils.hpp"

struct CompilationContext
{
    std::vector<SourceFile> source_files_;
    
    CompilationContext(std::vector<std::string> files) 
    {
        for (const auto& file : files)
            source_files_.push_back(get_source_file(file));
    }
};

namespace Sema
{
    struct SemaContext
    {
        std::unique_ptr<SemaTree> sema_tree_;
        std::unique_ptr<TypePool> type_pool_;
        std::unique_ptr<SymbolTable> symbol_table_;
        std::unique_ptr<Diagnostics> diagnostics_;

        SemaContext() : 
            sema_tree_{ std::make_unique<SemaTree>() },
            type_pool_{ std::make_unique<TypePool>() },
            symbol_table_{ std::make_unique<SymbolTable>() },
            diagnostics_{ std::make_unique<Diagnostics>() } {}
    };

} // namespace Sema


