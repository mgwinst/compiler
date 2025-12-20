#pragma once

#include <cstdint>
#include <cstddef>
#include <optional>
#include <vector>
#include <string_view>
#include <algorithm>
#include <array>

using namespace std::string_view_literals;

using TypeRef = std::size_t;
using NodeRef = std::size_t;

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
    TypeRef inner_type_;
};

struct PointerType
{
    TypeRef inner_type_;
};

struct ArrayType
{
    NodeRef size_;
    TypeRef inner_type_;
};

struct FunctionType
{
    TypeRef return_type_;
    std::vector<TypeRef> param_types_;
};

struct StructType
{

};

struct UnionType
{

};

struct EnumType
{

};

struct QualType
{
    enum class QualKind : uint8_t
    {
        Const,
        Restrict

    } qualifier_;

    TypeRef inner_type_;

    QualType(QualKind qualifier, TypeRef inner_type) :
        qualifier_{ qualifier },
        inner_type_{ inner_type } {}
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

template <> inline constexpr TypeKind type_kind_v<ByteType>      = TypeKind::Byte;
template <> inline constexpr TypeKind type_kind_v<BoolType>      = TypeKind::Bool;
template <> inline constexpr TypeKind type_kind_v<IntegerType>   = TypeKind::Integer;
template <> inline constexpr TypeKind type_kind_v<FloatType>     = TypeKind::Float;
template <> inline constexpr TypeKind type_kind_v<ReferenceType> = TypeKind::Reference;
template <> inline constexpr TypeKind type_kind_v<PointerType>   = TypeKind::Pointer;
template <> inline constexpr TypeKind type_kind_v<ArrayType>     = TypeKind::Array;
template <> inline constexpr TypeKind type_kind_v<FunctionType>  = TypeKind::Function;
template <> inline constexpr TypeKind type_kind_v<StructType>    = TypeKind::Struct;
template <> inline constexpr TypeKind type_kind_v<UnionType>     = TypeKind::Union;
template <> inline constexpr TypeKind type_kind_v<EnumType>      = TypeKind::Enumeration;
template <> inline constexpr TypeKind type_kind_v<QualType>      = TypeKind::Qualifier;

struct Type
{
    TypeKind kind_;
    alignas(64) std::byte data_[128];

    template <typename T>
    Type(std::in_place_type_t<T>, auto&&... args) : kind_{ type_kind_v<T> }
    {
        static_assert(type_kind_v<T> != TypeKind::Invalid, "undefined type T");

        ::new (data_) T{ std::forward<decltype(args)>(args)... };
    }
};

class TypePool
{
public:
    auto intern_type(std::string_view type_str) -> TypeRef;

    template <typename T, typename... Args>
        requires std::is_constructible_v<T, Args...>
    [[nodiscard]] auto create_type(std::string_view type_str, Args&&... args)
    {
        types_.emplace_back(std::in_place_type_t<T>, args...);
        type_to_index_.emplace{type_str, types_.size() - 1};

        return TypeRef{ types_.size() - 1 };
    }

private:
    std::vector<Type> types_; 
    std::unordered_map<std::string, TypeRef> type_to_index_;

    auto lookup_type(std::string_view type_str) -> std::optional<TypeRef>;
    auto parse_type(std::string_view type_str) -> TypeRef;
};

