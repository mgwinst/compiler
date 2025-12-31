#include <cassert>
#include <print>
#include <ranges>

#include "core/sema/type_pool.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

void test_type_pool()
{
    constexpr std::array types = {
        "int[]"sv,
    };

    Sema::TypePool type_pool{};

    auto v = types
           | views::transform([&](auto sv) { return type_pool.intern_type(sv); }) 
           | ranges::to<std::vector>();

    std::println("{}", v);

    assert(type_pool.types_.back().get_kind() == Sema::TypeKind::Array);
    assert(type_pool.types_.back().as<Sema::ArrayType>().inner_type_ == 4);
}


int main(int argc, char** argv)
{
    // test_type_pool();
 

    
}



