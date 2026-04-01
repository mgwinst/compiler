#pragma once

#include <boost/container/small_vector.hpp>
#include <unordered_map>

#include "type_hash.hpp"
#include "types.hpp"
#include "../../ast/ast.hpp"

inline constexpr std::size_t INLINE_VEC_SIZE{ 8 };

class TypePool
{
public:
    TypePool() noexcept;

    const Type& get_type(TypeID ref) const noexcept;
    Type& get_type(TypeID ref) noexcept;

    TypeID resolve_type(const ASTNodeID type_expr, const AST& ast) noexcept;




    std::vector<Type> types_;
    std::unordered_map<TypeID, boost::container::small_vector<TypeID, INLINE_VEC_SIZE>> buckets_;

    std::unordered_map<std::string, TypeID> user_defined_map; // fixes unknown type resolution problem

    template <typename T, typename... Args>
        requires std::is_constructible_v<T, Args...>
    [[nodiscard]] TypeID get_or_create(Args&&... args) noexcept
    {
        auto& bucket = buckets_[TypeHasher<T>::hash(args...)];

        for (TypeID id : bucket) {
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