#pragma once

#include <cstdint>
#include <cstddef>
#include <variant>

namespace Sema
{
    struct ByteType
    {

    };

    struct BoolType
    {

    };

    struct IntegerType
    {
        int16_t bit_width_;
        bool signedness_;
    };

    struct FloatType
    {
        int16_t bit_width_;
        bool signedness_;
    };

    struct ReferenceType
    {

    };

    struct PointerType
    {

    };
 

    struct ArrayType
    {

    };

    struct FunctionType
    {

    };
    
    struct StructType
    {

    };

    struct UnionType
    {

    };

    struct EnumerationType
    {

    };

    struct QualifierType
    {
        
    };

    enum class TypeKind : uint8_t
    {
        Byte,
        Bool,
        Integer,
        Float,
        Reference,
        Pointer,
        Array,
        Function,
        Struct,
        Union,
        Enumeration,
        Qualifier,
        Invalid
    };

    template <typename T>
    inline constexpr TypeKind type_kind_v = TypeKind::Invalid;
   
    template <> inline constexpr TypeKind type_kind_v<ByteType> = TypeKind::Byte;
    template <> inline constexpr TypeKind type_kind_v<BoolType> = TypeKind::Bool;
    template <> inline constexpr TypeKind type_kind_v<IntegerType> = TypeKind::Integer;
    template <> inline constexpr TypeKind type_kind_v<FloatType> = TypeKind::Float;
    template <> inline constexpr TypeKind type_kind_v<ReferenceType> = TypeKind::Reference;
    template <> inline constexpr TypeKind type_kind_v<PointerType> = TypeKind::Pointer;
    template <> inline constexpr TypeKind type_kind_v<ArrayType> = TypeKind::Array;
    template <> inline constexpr TypeKind type_kind_v<FunctionType> = TypeKind::Function;
    template <> inline constexpr TypeKind type_kind_v<StructType> = TypeKind::Struct;
    template <> inline constexpr TypeKind type_kind_v<UnionType> = TypeKind::Union;
    template <> inline constexpr TypeKind type_kind_v<EnumerationType> = TypeKind::Enumeration;
    template <> inline constexpr TypeKind type_kind_v<QualifierType> = TypeKind::Qualifier;

    struct Type
    {
        TypeKind kind;

        alignas(64) std::byte buffer_[128];
    };

}
