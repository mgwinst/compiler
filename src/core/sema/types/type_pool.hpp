#pragma once

#include <boost/container/small_vector.hpp>
#include <unordered_map>

#include "type_hash.hpp"
#include "types.hpp"
#include "../../ast/ast.hpp"

inline constexpr std::size_t INLINE_VEC_SIZE{ 8 };

namespace Sema
{
    class TypePool
    {
    public:
        TypePool() noexcept;

        const Type& get_type(TypeId ref) const noexcept;
        Type& get_type(TypeId ref) noexcept;

        TypeId resolve_type(const ASTNodeId type_expr, const AST& ast) noexcept;




        std::vector<Type> types_;
        std::unordered_map<TypeId, boost::container::small_vector<TypeId, INLINE_VEC_SIZE>> buckets_;

        std::unordered_map<std::string, TypeId> user_def_type_map; // fixes unknown type resolution problem

        template <typename T, typename... Args>
            requires std::is_constructible_v<T, Args...>
        [[nodiscard]] TypeId get_or_create(Args&&... args) noexcept
        {
            auto& bucket = buckets_[TypeHasher<T>::hash(args...)];

            for (TypeId id : bucket) {
                if (types_[id].get_kind() == type_kind_v<T>) {
                    if (types_[id].as<T>() == T{ args... })
                        return id;
                }
            }

            auto id = types_.size();
            types_.emplace_back(std::in_place_type<T>, std::forward<Args>(args)...);
            bucket.push_back(id);

            return id;
        }
    };

} // namespace Sema