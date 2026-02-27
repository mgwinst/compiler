#pragma once

#include <boost/container/small_vector.hpp>
#include <unordered_map>

#include "type_hash.hpp"
#include "types.hpp"
#include "../../ast/ast.hpp"

inline constexpr std::size_t VOID_INDEX = 0;
inline constexpr std::size_t BYTE_INDEX = 1;
inline constexpr std::size_t BOOL_INDEX = 2;
inline constexpr std::size_t INT8_INDEX = 3;
inline constexpr std::size_t INT16_INDEX = 4;
inline constexpr std::size_t INT32_INDEX = 5;
inline constexpr std::size_t INT64_INDEX = 6;
inline constexpr std::size_t UINT8_INDEX = 7;
inline constexpr std::size_t UINT16_INDEX = 8;
inline constexpr std::size_t UINT32_INDEX = 9;
inline constexpr std::size_t UINT64_INDEX = 10;
inline constexpr std::size_t FLOAT16_INDEX = 11;
inline constexpr std::size_t FLOAT32_INDEX = 12;
inline constexpr std::size_t FLOAT64_INDEX = 13;

inline constexpr std::size_t INLINE_VEC_SIZE{ 16 };

namespace Sema
{
    struct TypePool
    {
        std::vector<Type> types_;
        std::unordered_map<TypeRef, boost::container::small_vector<TypeRef, INLINE_VEC_SIZE>> buckets_;

        TypePool()
        {
            types_.emplace_back(std::in_place_type<VoidType>);
            types_.emplace_back(std::in_place_type<ByteType>);
            types_.emplace_back(std::in_place_type<BoolType>);

            types_.emplace_back(std::in_place_type<IntegerType>, uint16_t{ 8 }, true);
            types_.emplace_back(std::in_place_type<IntegerType>, uint16_t{ 16 }, true);
            types_.emplace_back(std::in_place_type<IntegerType>, uint16_t{ 32 }, true);
            types_.emplace_back(std::in_place_type<IntegerType>, uint16_t{ 64 }, true);

            types_.emplace_back(std::in_place_type<IntegerType>, uint16_t{ 8 }, false);
            types_.emplace_back(std::in_place_type<IntegerType>, uint16_t{ 16 }, false);
            types_.emplace_back(std::in_place_type<IntegerType>, uint16_t{ 32 }, false);
            types_.emplace_back(std::in_place_type<IntegerType>, uint16_t{ 64 }, false);

            types_.emplace_back(std::in_place_type<FloatType>, uint16_t{ 16 }, true);
            types_.emplace_back(std::in_place_type<FloatType>, uint16_t{ 32 }, true);
            types_.emplace_back(std::in_place_type<FloatType>, uint16_t{ 64 }, true);
        }

        TypeRef resolve_type(const ASTNodeRef type_expr, const AST& ast) noexcept;

        template <typename T, typename... Args>
            requires std::is_constructible_v<T, Args...>
        [[nodiscard]] TypeRef get_or_create(Args&&... args)
        {
            auto& bucket = buckets_[TypeHasher<T>::hash(args...)];

            for (TypeRef ref : bucket) {
                if (types_[ref].get_kind() == type_kind_v<T>) {
                    if (types_[ref].as<T>() == T{ args... })
                        return ref;
                }
            }

            auto id = types_.size();
            types_.emplace_back(std::in_place_type<T>, std::forward<Args>(args)...);
            bucket.push_back(id);

            return id;
        }
    };

} // namespace Sema