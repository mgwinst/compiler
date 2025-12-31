#include <cassert>
#include <print>
#include <ranges>
#include <boost/container/small_vector.hpp>

#include "core/sema/type_pool.hpp"

namespace views = std::views;

void test_type_pool()
{
    constexpr std::array types = {
        "int[]"sv,
    };

    Sema::TypePool type_pool{};

    auto v = types | views::transform([&](auto sv) { return type_pool.intern_type(sv); });
    auto vec = std::vector(begin(v), end(v));

    assert(type_pool.types_.back().get_kind() == Sema::TypeKind::Array);
    assert(type_pool.types_.back().as<Sema::ArrayType>().inner_type_ == 4);
}

int main(int argc, char** argv)
{
    // test_type_pool();
 

    boost::container::small_vector<int, 10> v;

    std::println("{}", v.internal_capacity());
    
}



