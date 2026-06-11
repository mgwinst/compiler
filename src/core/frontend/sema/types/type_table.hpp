#pragma once

#include <unordered_map>
#include <boost/container/small_vector.hpp>

#include "utils/structures/arena.hpp"
#include "utils/utils.hpp"
#include "frontend/ast/ast.hpp"
#include "type_hash.hpp"
#include "types.hpp"

using boost::container::small_vector;

inline constexpr std::size_t inline_vec_size{ 8 };

class TypeTable
{
public:
    std::unordered_map<std::size_t, small_vector<Type*, inline_vec_size>> buckets_;
    std::unordered_map<std::string, Type*> builtin_map_;
    std::unordered_map<std::string, Type*> user_defined_map_; // fixes unknown type resolution problem

    TypeTable();

    TypeTable(const TypeTable&) = delete;
    TypeTable& operator=(const TypeTable&) = delete;
    TypeTable(TypeTable&&) = default;
    TypeTable& operator=(TypeTable&&) = default;
    ~TypeTable() = default;

    Type* resolve_type(Syntax::ASTNode* type_expr);

    template <DerivedFromType T, typename... Args>
        requires std::is_constructible_v<T, Args...>
    [[nodiscard]] Type* get_or_create(Args&&... args)
    {
        auto& bucket = buckets_[TypeHasher<T>::hash(args...)];

        for (Type* ptr : bucket) {
            if (T* p = dyn_cast<T>(ptr)) {
                if (*p == T{ args... }) { // forwarding would invalidate args below if this fails, so we copy construct a temporary.
                    return ptr;
                }
            }
        }

        Type* ptr = arena.emplace<T>(std::forward<Args>(args)...);
        bucket.push_back(ptr);

        return ptr;
    }

private:
    Arena arena;
};