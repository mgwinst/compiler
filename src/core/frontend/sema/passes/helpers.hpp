#pragma once   

#include "../types/types.hpp"
#include "../../../utils/enums.hpp"

inline bool is_integral(TypeId type_id)
{
    return (type_id >= INT8 && type_id <= UINT64);   
}

inline bool is_int_or_ptr(const Sema::Type& type)
{
    return type.get_kind() == TypeKind::Integer || type.get_kind() == TypeKind::Pointer;
}

inline bool is_const(const Sema::Type& type) 
{
    if (type.get_kind() == TypeKind::Qualifier)
        return type.as<Sema::QualifierType>().kind_ == QualifierKind::Const;
    return false;
}

inline void ignore_const()
{
    
}

inline bool convertible_to_boolean(const Sema::Type& type)
{
    /*
    if (is_const(type)) {
        type = type.as<QualifierType>().inner_type_;
    }
    */

    if (type.get_kind() == TypeKind::Record)
        return false;

    return true;
}

inline bool is_bitwise_op(std::string_view op)
{
    return (op == "<<" ||
            op == ">>" ||
            op == "&"  || 
            op == "^"  ||
            op == "|");
}

inline bool is_logical_op(std::string_view op)
{
    return (op == "&&" || op == "||" || op == "!=" || op == "==");
}

inline bool is_relational_op(std::string_view op) 
{
    return (op == ">" || op == ">=" || op == "<" || op == "<=");
}