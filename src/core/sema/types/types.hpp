#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>
#include <new>
#include <vector>

#include "../../utils/macros.hpp"
#include "../../utils/concepts.hpp"
#include "../../utils/alias.hpp"


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
        bool signedness_;

        bool operator==(const IntegerType& other) const = default;
    };

    struct FloatType
    {
        uint16_t bit_width_;
        bool signedness_;

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
        enum class QualKind : uint8_t
        {
            Const

        } qualifier_;

        TypeRef inner_type_;

        QualifierType(QualKind qualifier, TypeRef inner_type) :
            qualifier_{ qualifier },
            inner_type_{ inner_type } {}

        bool operator==(const QualifierType& other) const = default;
    };

    /*
    struct FunctionType
    {
        std::vector<TypeRef> param_types_;
        TypeRef return_type_;

        FunctionType(Contiguous auto&& param_types, TypeRef return_type) :
            param_types_{ std::forward<decltype(param_types)>(param_types) },
            return_type_{ return_type } {}
        
        bool operator==(const FunctionType& other) const {
            return return_type_ == other.return_type_ &&
                   param_types_ == other.param_types_;
        }
    };
    */

    // nominally type functions over name for now...
    // include return type tho?

    struct FunctionType
    {
        std::string name_;
        TypeRef return_type_;

        FunctionType(StringLike auto&& name, TypeRef return_type) :
            name_{ std::forward<decltype(name)>(name) },
            return_type_{ return_type } {}

        bool operator==(const FunctionType& other) const {
            return return_type_ == other.return_type_ &&
                   name_        == other.name_;
        }
    };

    struct RecordType 
    {
        std::string name_;

        RecordType(StringLike auto&& name) :
            name_{ std::forward<decltype(name)>(name) } {}

        bool operator==(const RecordType& other) const = default;
    };

    enum class TypeKind : uint8_t
    {
        Void,
        Byte,
        Bool,
        Integer,
        Float,
        Reference,
        Pointer,
        Array,
        Qualifier,
        Function,
        Record,
        Invalid
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


