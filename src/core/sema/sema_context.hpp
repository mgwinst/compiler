#pragma once

#include <memory>

#include "sematree.hpp"
#include "types/type_pool.hpp"

namespace Sema
{
    struct SemaContext
    {
        std::shared_ptr<SemaTree> sem_tree;
        std::shared_ptr<TypePool> type_pool_;
        std::shared_ptr<SymbolTable> symbol_table_;
    };

} // namespace Sema