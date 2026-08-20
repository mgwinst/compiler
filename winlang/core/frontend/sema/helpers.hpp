#pragma once

#include "frontend/sema/types/types.hpp"
#include "utils/casting.hpp"

inline bool is_lvalue(Sema::SemaNode* node)
{
    return isa<Sema::ReferenceExpr>(node) ||
           isa<Sema::MemberExpr>(node) ||
           isa<Sema::ArraySubscriptExpr>(node);
}

inline bool is_const(Type* type)
{
    if (auto* qual = dyn_cast<QualifierType>(type))
        return qual->kind_ == QualifierKind::Const;

    return false;
}

inline bool is_scalar(Type* type)
{
    return isa<PointerType>(type) || 
           isa<IntegerType>(type) || 
           isa<FloatType>(type);
}

inline bool is_integral(Type* type)
{
    return isa<IntegerType>(type);
}

inline bool convertible_to_boolean(Type* type)
{
    if (is_const(type))
        return convertible_to_boolean(cast<QualifierType>(type)->inner_type_);

    return isa<BoolType>(type) || is_scalar(type);
}

inline bool is_arithmetic_op(std::string_view op)
{
    return op == "+" ||
           op == "-" ||
           op == "*" ||
           op == "/" ||
           op == "%";
}

inline bool is_bitwise_op(std::string_view op)
{
    return op == "<<" ||
           op == ">>" ||
           op == "&"  || 
           op == "^"  ||
           op == "|";
}

inline bool is_logical_op(std::string_view op)
{
    return op == "&&" ||
           op == "||" || 
           op == "!=" || 
           op == "==" ||
           op == "!";
}

inline bool is_relational_op(std::string_view op) 
{
    return op == ">"  || 
           op == ">=" || 
           op == "<"  || 
           op == "<=";
}

inline bool is_literal(Sema::SemaNode* node)
{
    return isa<Sema::IntegerLiteralExpr>(node) ||
           isa<Sema::FloatLiteralExpr>(node)   ||
           isa<Sema::BooleanLiteralExpr>(node) ||
           isa<Sema::CharLiteralExpr>(node)    ||
           isa<Sema::StringLiteralExpr>(node);
}

inline Type* remove_const_qualifier(Type* type)
{
    if (auto* qual_type = dyn_cast<QualifierType>(type))
        return qual_type->inner_type_;
    
    return type;
}

inline bool is_unsigned(Type* type)
{
    if (is_scalar(type)) {
        if (auto* t = dyn_cast<IntegerType>(type)) {
            return t->is_signed_ == false;
        }
    }

    return false;
}

inline bool is_pointer_dereference(Sema::SemaNode* node)
{
    if (auto* pd = dyn_cast<Sema::UnaryExpr>(node)) {
        return pd->op_ == "*" && pd->is_postfix_ == false;
    }

    return false;
}