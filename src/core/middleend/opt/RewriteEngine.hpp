#pragma once

#include <unordered_set>
#include <algorithm>

#include "boost/container/small_vector.hpp"

#include "../ir/IRBuilder.hpp"
#include "../ir/IR.hpp"
#include "../utils/casting.hpp"

using boost::container::small_vector;

// map patterns to rewrite functions (rewrites)?

// fold(add, 3, 2) -> 5
// GEP(ptr, 0) -> ptr
// ...


class RewriteEngine
{
    void fold();

private:
    Program& program_;
    IRBuilder builder_;
};

void RewriteEngine::fold()
{
}