#pragma once

#include <boost/container/small_vector.hpp>
#include <unordered_map>

#include "type_hash.hpp"
#include "types.hpp"
#include "../../ast/ast.hpp"

inline constexpr std::size_t INLINE_VEC_SIZE{ 16 };

namespace Sema
{
    struct TypePool
    {
        std::vector<Type> types_;
        std::unordered_map<TypeRef, boost::container::small_vector<TypeRef, INLINE_VEC_SIZE>> buckets_;

        TypePool();

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

        void print() const noexcept;
    };

} // namespace Sema