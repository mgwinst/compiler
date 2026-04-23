#pragma once

#include <unordered_map>

#include "../ir/Value.hpp"

class ConstantPool
{
public:
    template <typename T>
    auto try_insert(T literal)
    {
        return get_container<T>().try_emplace(literal, new IR::Literal{literal}); // second arg is wrong, we want lazy forwarding args to unique_ptr<IR::Literal> ctor
    }

private:
    std::unordered_map<int64_t, std::unique_ptr<IR::Literal>> integer_pool_;
    std::unordered_map<double, std::unique_ptr<IR::Literal>> float_pool_;
    std::unordered_map<std::string, std::unique_ptr<IR::Literal>> string_pool_;

    template <typename T>
    auto& get_container()
    {
        if constexpr (std::same_as<T, int64_t>)
            return integer_pool_;
        else if constexpr (std::same_as<T, double>)
            return float_pool_;
        else if constexpr (std::same_as<T, std::string>)
            return string_pool_;
        else
            static_assert(always_false<T>, "T is not an internable type");
    }
};