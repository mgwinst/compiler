#pragma once   

#include "types.hpp"

namespace Sema
{
    inline std::size_t hash_combine(std::size_t seed, std::size_t value)
    {
        return seed ^ (value + 0x9e3779b9 + (seed << 6) + (seed >> 2));
    }

    template <typename T>
    struct TypeHasher;

    template <>
    struct TypeHasher<ReferenceType>
    {
        static constexpr std::size_t magic = 0x1ce5b6d7a4f3e291ULL;
        
        static std::size_t hash(TypeRef inner_type)
        {
            return hash_combine(magic, inner_type);
        }
    };

    template <>
    struct TypeHasher<PointerType>
    {
        static constexpr std::size_t magic = 0x2d4f6e8a9b1c3d5fULL;

        static std::size_t hash(TypeRef inner_type)
        {
            return hash_combine(magic, inner_type);
        }
    };

    template <>
    struct TypeHasher<ArrayType>
    {
        static constexpr std::size_t magic = 0x3e6d8f9ab1c2d4e7ULL;
        
        static std::size_t hash(TypeRef inner_type)
        {
            return hash_combine(magic, inner_type);
        }
    };

    template <>
    struct TypeHasher<QualifierType>
    {
        static constexpr std::size_t magic = 0x4f7e9a0bc1d2e3f8ULL;
        
        static std::size_t hash(TypeRef inner_type)
        {
            return hash_combine(magic, inner_type);
        }
    };

    template <>
    struct TypeHasher<FunctionType>
    {
        static constexpr std::size_t magic = 0x5a8cb9d0e1f20364ULL;
        
        static std::size_t hash()
        {
            return hash_combine();
        }
    };

    template <>
    struct TypeHasher<StructType>
    {
        static constexpr std::size_t magic = 0x6b9dc0e1f2034567ULL;
        
        static std::size_t hash()
        {
            return hash_combine();
        }
    };

    template <>
    struct TypeHasher<UnionType>
    {
        static constexpr std::size_t magic = 0x7c8ed1f203456789ULL;
        
        static std::size_t hash()
        {
            return hash_combine();
        }
    };

    template <>
    struct TypeHasher<EnumType>
    {
        static constexpr std::size_t magic = 0x8d9fe0f112345678ULL;
        
        static std::size_t hash()
        {
            return hash_combine();
        }
    };

    
} // namespace Sema