#pragma once

#include <cstdint>
#include <cstddef>
#include <functional>
#include <vector>

using TypeRef = std::size_t;

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
        TypeKind kind_;
        alignas(64) std::byte data_[128];
    };

    struct TypePool
    {
        std::vector<Type> types_; 
    };

    

    // construct type objects from parsed full type string in each AST Node. Simple for now... implement proper parse handlers for types in Parser later.
    struct TypeParser
    {
        static TypeRef parse_type(std::string_view type_string);
        static TypeRef parse_built_in_type(std::string_view type_string);
        static TypeRef parse_ref_type(std::string_view type_string);
        static TypeRef parse_pointer_type(std::string_view type_string);
        static TypeRef parse_array_type(std::string_view type_string);
        static TypeRef parse_func_type(std::string_view type_string);
        static TypeRef parse_struct_type(std::string_view type_string);
        static TypeRef parse_union_type(std::string_view type_string);
        static TypeRef parse_enum_type(std::string_view type_string);
        static TypeRef parse_qual_type(std::string_view type_string);
    };

    /*
    TypeRef intern_type(std::string_view type_string)
    {
        // check to see if type in table. if not, construct it and return index into type pool
    }
    */

}

// reserve common types in a separate buffer (built-in types + pointers/arrays to built-ins buffer) ?

// tagged pointers? (prob not as we aren't using pointers to heap allocated Type objects...)
    

// const int arr[10];
// struct Foo arr[1024];
// byte buffer[10];
// const int x;
// float64 value;
    
    
