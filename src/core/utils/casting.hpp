#pragma once

#include "../middleend/ir/IR.hpp"

template <typename T>
inline constexpr ValueKind value_kind_v = ValueKind::Invalid;

template <> inline constexpr ValueKind value_kind_v<Function>   = ValueKind::Function;
template <> inline constexpr ValueKind value_kind_v<BasicBlock> = ValueKind::BasicBlock;
template <> inline constexpr ValueKind value_kind_v<Argument>   = ValueKind::Argument;
template <> inline constexpr ValueKind value_kind_v<Alloca>     = ValueKind::Alloca; // inst begin
template <> inline constexpr ValueKind value_kind_v<Load>       = ValueKind::Load;
template <> inline constexpr ValueKind value_kind_v<Store>      = ValueKind::Store;
template <> inline constexpr ValueKind value_kind_v<Add>        = ValueKind::Add;
template <> inline constexpr ValueKind value_kind_v<Sub>        = ValueKind::Sub;
template <> inline constexpr ValueKind value_kind_v<Mul>        = ValueKind::Mul;
template <> inline constexpr ValueKind value_kind_v<Div>        = ValueKind::Div;
template <> inline constexpr ValueKind value_kind_v<Eq>         = ValueKind::Eq;
template <> inline constexpr ValueKind value_kind_v<Ne>         = ValueKind::Ne;
template <> inline constexpr ValueKind value_kind_v<Slt>        = ValueKind::Slt;
template <> inline constexpr ValueKind value_kind_v<Call>       = ValueKind::Call;
template <> inline constexpr ValueKind value_kind_v<Return>     = ValueKind::Return;
template <> inline constexpr ValueKind value_kind_v<Branch>     = ValueKind::Branch;
template <> inline constexpr ValueKind value_kind_v<PtrAdd>     = ValueKind::PtrAdd;
template <> inline constexpr ValueKind value_kind_v<Phi>        = ValueKind::Phi; // inst end
template <> inline constexpr ValueKind value_kind_v<Literal>    = ValueKind::Const;


template <typename T, DerivedFromValue V>
bool isa(const std::unique_ptr<V>& value)
{
    if (!value)
        return false;

    return value->kind_ == value_kind_v<T>;
}

template <typename T, DerivedFromValue V>
bool isa(V* value)
{
    if (!value)
        return false;

    return value->kind_ == value_kind_v<T>;
}

template <typename T, DerivedFromValue V>
T* dyn_cast(const std::unique_ptr<V>& value)
{
    if (value && value->kind_ == value_kind_v<T>)
        return static_cast<T*>(value.get());

    return nullptr;
}

template <typename T, DerivedFromValue V>
T* dyn_cast(V* value)
{
    if (value && value->kind_ == value_kind_v<T>)
        return static_cast<T*>(value);

    return nullptr;
}

template <DerivedFromValue V>
bool is_instruction(std::unique_ptr<V>& value) 
{
    return value->kind_ >= ValueKind::Alloca && value->kind_ <= ValueKind::Phi;
}

template <DerivedFromValue V>
bool is_instruction(V* value) 
{
    return value->kind_ >= ValueKind::Alloca && value->kind_ <= ValueKind::Phi;
}

template <DerivedFromValue V>
bool is_terminator(std::unique_ptr<V>& value) 
{
    return value->kind_ == ValueKind::Return || 
           value->kind_ == ValueKind::Branch;
}

template <DerivedFromValue V>
bool is_terminator(V* value) 
{
    return value->kind_ == ValueKind::Return || 
           value->kind_ == ValueKind::Branch;
}