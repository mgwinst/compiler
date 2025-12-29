#pragma once

#include <unordered_map>

#include "type_hash.hpp"

inline constexpr std::size_t BYTE_INDEX = 0;
inline constexpr std::size_t BOOL_INDEX = 1;

inline constexpr std::size_t INT8_INDEX = 2;
inline constexpr std::size_t INT16_INDEX = 3;
inline constexpr std::size_t INT32_INDEX = 4;
inline constexpr std::size_t INT64_INDEX = 5;

inline constexpr std::size_t UINT8_INDEX = 6;
inline constexpr std::size_t UINT16_INDEX = 7;
inline constexpr std::size_t UINT32_INDEX = 8;
inline constexpr std::size_t UINT64_INDEX = 9;

inline constexpr std::size_t FLOAT16_INDEX = 10;
inline constexpr std::size_t FLOAT32_INDEX = 11;
inline constexpr std::size_t FLOAT64_INDEX = 12;

namespace Sema
{
    struct TypePool
    {
        std::vector<Type> types_;
        std::unordered_map<TypeRef, std::vector<TypeRef>> buckets_; // small buffer opt later to avoid heap

        TypePool();

        TypeRef intern_type(std::string_view type_str);

        template <typename T, typename... Args>
            requires std::is_constructible_v<T, Args...>
        [[nodiscard]] TypeRef get_or_create(Args&&... args)
        {
            auto& bucket = buckets_[TypeHasher<T>::hash(args...)];

            for (TypeRef ref : bucket) {
                if (types_[ref].get_kind() == type_kind_v<T>) {
                    // if (types_[ref].as<T>() == std::tie(args...))
                    if (types_[ref].as<T>() == T{ args... })
                        return ref;
                }
            }

            auto id = types_.size();
            types_.emplace_back(std::in_place_type<T>, std::forward<Args>(args)...);
            bucket.push_back(id);

            return id;
        }

        TypeRef parse_type(std::string_view type_str) noexcept;
    };

} // namespace Sema
