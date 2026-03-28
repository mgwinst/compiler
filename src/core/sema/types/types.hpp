#pragma once

#include <cstddef>
#include <cwchar>
#include <type_traits>
#include <utility>
#include <new>
#include <vector>

#include "../../utils/macros.hpp"
#include "../../utils/alias.hpp"
#include "../../utils/enums.hpp"

namespace Sema
{
    struct ErrorType
    {

    };

    struct VoidType
    {
        bool operator==(const VoidType&) const { return true; }
    };

    struct ByteType
    {
        bool operator==(const ByteType&) const { return true; }
    };

    struct CharType
    {
        bool operator==(const CharType&) const { return true; }
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
        TypeId inner_type_;

        bool operator==(const ReferenceType& other) const = default;
    };

    struct PointerType
    {
        TypeId inner_type_;

        bool operator==(const PointerType& other) const = default;
    };

    struct ArrayType
    {
        TypeId inner_type_;
        uint64_t size_;

        bool operator==(const ArrayType& other) const = default;
    };

    struct QualifierType
    {
        QualifierKind kind_;
        TypeId inner_type_;

        QualifierType(QualifierKind kind, TypeId inner_type) :
            kind_{ kind },
            inner_type_{ inner_type } {}

        bool operator==(const QualifierType& other) const = default;
    };

    /*
    struct Parameter
    {

    }
    */

    struct FunctionType
    {
        std::string name_;
        std::vector<TypeId> params_;
        TypeId return_type_;

        FunctionType(std::string name, std::vector<TypeId> params, TypeId return_type) :
            name_{ std::move(name) },
            params_{ std::move(params) },
            return_type_{ return_type } {}

        bool operator==(const FunctionType& other) const = default;
    };

    struct Field
    {
        std::string name_;          
        TypeId type_;

        bool operator==(const Field& other) const = default;
    };

    struct RecordType 
    {
        RecordKind kind_;
        std::string name_;
        std::vector<Field> fields_;
        // uint64_t size_;

        RecordType(RecordKind kind, std::string name) :
            kind_{ kind },
            name_{ std::move(name) } {}

        bool operator==(const RecordType& other) const = default;

        Field* lookup_field(std::string_view name)
        {
            auto it = std::ranges::find_if(fields_, [&](const Field& f) { return f.name_ == name; });
            return (it != fields_.end()) ? &(*it) : nullptr;
        }
    };

    // records are the only nominally typed constructs 
    // records also share namespace -> union S; struct S; -> Error in C. do we want this behavior?

    template <typename T>
    inline constexpr TypeKind type_kind_v = TypeKind::Invalid;

    template <> inline constexpr TypeKind type_kind_v<ErrorType>     = TypeKind::Error;
    template <> inline constexpr TypeKind type_kind_v<VoidType>      = TypeKind::Void;
    template <> inline constexpr TypeKind type_kind_v<ByteType>      = TypeKind::Byte;
    template <> inline constexpr TypeKind type_kind_v<CharType>      = TypeKind::Char;
    template <> inline constexpr TypeKind type_kind_v<BoolType>      = TypeKind::Bool;
    template <> inline constexpr TypeKind type_kind_v<IntegerType>   = TypeKind::Integer;
    template <> inline constexpr TypeKind type_kind_v<FloatType>     = TypeKind::Float;
    template <> inline constexpr TypeKind type_kind_v<ReferenceType> = TypeKind::Reference;
    template <> inline constexpr TypeKind type_kind_v<PointerType>   = TypeKind::Pointer;
    template <> inline constexpr TypeKind type_kind_v<ArrayType>     = TypeKind::Array;
    template <> inline constexpr TypeKind type_kind_v<QualifierType> = TypeKind::Qualifier;
    template <> inline constexpr TypeKind type_kind_v<FunctionType>  = TypeKind::Function;
    template <> inline constexpr TypeKind type_kind_v<RecordType>    = TypeKind::Record;

    class Type
    {
    public:
        template <typename T>
            requires (type_kind_v<T> != TypeKind::Invalid)
        Type(std::in_place_type_t<T>, auto&&... args) : kind_{ type_kind_v<T> }
        {
            ::new (reinterpret_cast<T*>(data_)) T{ std::forward<decltype(args)>(args)... };
        }

        Type(Type&& other) noexcept : kind_{ other.kind_ }
        {
            move_construct_from(other);
            other.kind_ = TypeKind::Invalid;
        }

        Type& operator=(Type&& other) noexcept
        {
            this->~Type();
            kind_ = other.kind_;
            move_construct_from(other);
            other.kind_ = TypeKind::Invalid;
            
            return *this;
        }

        Type(const Type&) = delete;
        Type& operator=(const Type&) = delete;

        ~Type() noexcept
        {
            destroy_active();
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
            if (kind_ != type_kind_v<T>) [[unlikely]] {
                error_exit("types don't match");
            }

            return *std::launder(reinterpret_cast<T*>(data_));
        }

    private:
        alignas(64) std::byte data_[127];
        TypeKind kind_;

        void move_construct_from(Type& other)
        {
            switch (kind_) {
                case TypeKind::Error:       move_construct<ErrorType>(other);      break;
                case TypeKind::Void:        move_construct<VoidType>(other);      break;
                case TypeKind::Byte:        move_construct<ByteType>(other);      break;
                case TypeKind::Char:        move_construct<CharType>(other);      break;
                case TypeKind::Bool:        move_construct<BoolType>(other);      break;
                case TypeKind::Integer:     move_construct<IntegerType>(other);   break;
                case TypeKind::Float:       move_construct<FloatType>(other);     break;
                case TypeKind::Reference:   move_construct<ReferenceType>(other); break;
                case TypeKind::Pointer:     move_construct<PointerType>(other);   break;
                case TypeKind::Array:       move_construct<ArrayType>(other);     break;
                case TypeKind::Qualifier:   move_construct<QualifierType>(other); break;
                case TypeKind::Function:    move_construct<FunctionType>(other);  break;
                case TypeKind::Record:      move_construct<RecordType>(other);    break;
                case TypeKind::Invalid: [[fallthrough]];
                default: break;
            }
        }

        template <typename T>
        void move_construct(Type& other) noexcept
        {
            ::new (data_) T{ std::move(other.as<T>()) };
        }

        void destroy_active() noexcept
        {
            switch (kind_) {
                case TypeKind::Error:       destroy<ErrorType>();      break;
                case TypeKind::Void:        destroy<VoidType>();      break;
                case TypeKind::Byte:        destroy<ByteType>();      break;
                case TypeKind::Char:        destroy<CharType>();      break;
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

        template <typename T>
        void destroy()
        {
            if constexpr (!std::is_trivially_destructible_v<T>)
                std::launder(reinterpret_cast<T*>(data_))->~T();
        }
    };

} // namespace Sema

inline constexpr SemaNodeId ERROR_TYPE = 0;
inline constexpr SemaNodeId VOID = 1;
inline constexpr SemaNodeId BYTE = 2;
inline constexpr SemaNodeId CHAR = 3;
inline constexpr SemaNodeId BOOL = 4;
inline constexpr SemaNodeId INT8 = 5;
inline constexpr SemaNodeId INT16 = 6;
inline constexpr SemaNodeId INT32 = 7;
inline constexpr SemaNodeId INT64 = 8;
inline constexpr SemaNodeId UINT8 = 9;
inline constexpr SemaNodeId UINT16 = 10;
inline constexpr SemaNodeId UINT32 = 11;
inline constexpr SemaNodeId UINT64 = 12;
inline constexpr SemaNodeId FLOAT16 = 13;
inline constexpr SemaNodeId FLOAT32 = 14;
inline constexpr SemaNodeId FLOAT64 = 15;
