#pragma once

#include <cstddef>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <new>

#include "../../utils/macros.hpp"
#include "../../utils/alias.hpp"
#include "../../utils/enums.hpp"


namespace Sema
{
    struct VoidType
    {
        bool operator==(const VoidType&) const { return true; }
    };

    struct ByteType
    {
        bool operator==(const ByteType&) const { return true; }
    };

    struct BoolType
    {
        bool operator==(const BoolType&) const { return true; }
    };

    struct IntegerType
    {
        uint16_t bit_width_;
        bool is_signed_;

        bool operator==(const IntegerType& other) const = default;
    };

    struct FloatType
    {
        uint16_t bit_width_;

        bool operator==(const FloatType& other) const = default;
    };

    struct ReferenceType
    {
        TypeRef inner_type_;

        bool operator==(const ReferenceType& other) const = default;
    };

    struct PointerType
    {
        TypeRef inner_type_;

        bool operator==(const PointerType& other) const = default;
    };

    struct ArrayType
    {
        TypeRef inner_type_;
        uint64_t size_;

        bool operator==(const ArrayType& other) const = default;
    };

    struct QualifierType
    {
        QualifierKind kind_;
        TypeRef inner_type_;

        QualifierType(QualifierKind kind, TypeRef inner_type) :
            kind_{ kind },
            inner_type_{ inner_type } {}

        bool operator==(const QualifierType& other) const = default;
    };

    struct FunctionType
    {
        std::string name_;
        TypeRef return_type_;

        FunctionType(std::string name, TypeRef return_type) :
            name_{ std::move(name) },
            return_type_{ return_type } {}

        bool operator==(const FunctionType& other) const = default;
    };

    struct RecordType 
    {
        RecordKind kind_;
        std::string name_;

        RecordType(RecordKind kind, std::string name) :
            kind_{ kind },
            name_{ std::move(name) } {}

        bool operator==(const RecordType& other) const = default;
    };

    template <typename T>
    inline constexpr TypeKind type_kind_v = TypeKind::Invalid;

    template <> inline constexpr TypeKind type_kind_v<VoidType>      = TypeKind::Void;
    template <> inline constexpr TypeKind type_kind_v<ByteType>      = TypeKind::Byte;
    template <> inline constexpr TypeKind type_kind_v<BoolType>      = TypeKind::Bool;
    template <> inline constexpr TypeKind type_kind_v<IntegerType>   = TypeKind::Integer;
    template <> inline constexpr TypeKind type_kind_v<FloatType>     = TypeKind::Float;
    template <> inline constexpr TypeKind type_kind_v<ReferenceType> = TypeKind::Reference;
    template <> inline constexpr TypeKind type_kind_v<PointerType>   = TypeKind::Pointer;
    template <> inline constexpr TypeKind type_kind_v<ArrayType>     = TypeKind::Array;
    template <> inline constexpr TypeKind type_kind_v<QualifierType> = TypeKind::Qualifier;
    template <> inline constexpr TypeKind type_kind_v<FunctionType>  = TypeKind::Function;
    template <> inline constexpr TypeKind type_kind_v<RecordType>    = TypeKind::Record;

    struct Type
    {
        alignas(64) std::byte data_[63];
        TypeKind kind_;

        template <typename T>
            requires (type_kind_v<T> != TypeKind::Invalid)
        Type(std::in_place_type_t<T>, auto&&... args) : kind_{ type_kind_v<T> }
        {
            ::new (reinterpret_cast<T*>(data_)) T{ std::forward<decltype(args)>(args)... };
        }

        ~Type()
        {
            switch (kind_) {
                case TypeKind::Void:        destroy<VoidType>();      break;
                case TypeKind::Byte:        destroy<ByteType>();      break;
                case TypeKind::Bool:        destroy<BoolType>();      break;
                case TypeKind::Integer:     destroy<IntegerType>();   break;
                case TypeKind::Float:       destroy<FloatType>();     break;
                case TypeKind::Reference:   destroy<ReferenceType>(); break;
                case TypeKind::Pointer:     destroy<PointerType>();   break;
                case TypeKind::Array:       destroy<ArrayType>();     break;
                case TypeKind::Qualifier:   destroy<QualifierType>(); break;
                case TypeKind::Function:    destroy<FunctionType>();  break;
                case TypeKind::Record:      destroy<RecordType>();    break;
                case TypeKind::Invalid: [[fallthrough]];
                default: break;
            }
        }

        TypeKind get_kind() const
        {
            return kind_;
        }

        template <typename T>
        [[nodiscard]] const T& as() const
        {
            if (kind_ != type_kind_v<T>) [[unlikely]]
                error_exit("types don't match");

            return *std::launder(reinterpret_cast<const T*>(data_));
        }

        template <typename T>
        [[nodiscard]] T& as()
        {
            if (kind_ != type_kind_v<T>) [[unlikely]]
                error_exit("types don't match");

            return *std::launder(reinterpret_cast<T*>(data_));
        }

        template <typename T>
        void destroy()
        {
            if constexpr (!std::is_trivially_destructible_v<T>)
                std::launder(reinterpret_cast<T*>(data_))->~T();
        }
    };

} // namespace Sema

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

enum class TypeCategory
{
    INTEGER,
    FLOATING_POINT
};

inline std::unordered_map<TypeRef, TypeCategory> type_category {
    {INT8_INDEX,    TypeCategory::INTEGER},
    {INT16_INDEX,   TypeCategory::INTEGER},
    {INT32_INDEX,   TypeCategory::INTEGER},
    {INT64_INDEX,   TypeCategory::INTEGER},
    {UINT8_INDEX,   TypeCategory::INTEGER},
    {UINT16_INDEX,  TypeCategory::INTEGER}, 
    {UINT32_INDEX,  TypeCategory::INTEGER},
    {UINT64_INDEX,  TypeCategory::INTEGER},
    {FLOAT16_INDEX, TypeCategory::FLOATING_POINT},
    {FLOAT32_INDEX, TypeCategory::FLOATING_POINT},
    {FLOAT64_INDEX, TypeCategory::FLOATING_POINT}
};

