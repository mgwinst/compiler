#include "core/sema/type_pool.hpp"

void test_type_pool()
{
    constexpr std::array types = {
        "int32[]"sv,
    };

    Sema::TypePool type_pool{};

    for (const auto& type : types)
        std::println("{}", type_pool.intern_type(type));

}

int main(int argc, char** argv)
{
    test_type_pool();   

}



