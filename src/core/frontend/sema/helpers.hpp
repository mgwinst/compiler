#pragma once

#include "frontend/sema/types/types.hpp"
#include "utils/casting.hpp"

inline bool is_const(Type* type)
{
    if (auto* qual = dyn_cast<QualifierType>(type))
        return qual->kind_ == QualifierKind::Const;

    return false; // maybe return inner type instead for more cleaner usage
}

inline bool is_scalar(Type* type)
{
    return isa<PointerType>(type) || isa<IntegerType>(type) || isa<FloatType>(type);
}

inline bool convertible_to_boolean(Type* type)
{
    if (is_const(type))
        return convertible_to_boolean(cast<QualifierType>(type)->inner_type_);

    return isa<BoolType>(type) || is_scalar(type);
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