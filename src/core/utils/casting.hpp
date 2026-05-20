#pragma once

#include "../middleend/ir/IR.hpp"

using namespace IR;

template <typename T>
inline constexpr ValueKind value_kind_v = ValueKind::Invalid;

template <> inline constexpr ValueKind value_kind_v<Function>   = ValueKind::FunctionVal;
template <> inline constexpr ValueKind value_kind_v<BasicBlock> = ValueKind::BasicBlockVal;
template <> inline constexpr ValueKind value_kind_v<Argument>   = ValueKind::ArgumentVal;
template <> inline constexpr ValueKind value_kind_v<AllocaInst> = ValueKind::AllocaInstVal; // inst begin
template <> inline constexpr ValueKind value_kind_v<LoadInst>   = ValueKind::LoadInstVal;
template <> inline constexpr ValueKind value_kind_v<StoreInst>  = ValueKind::StoreInstVal;
template <> inline constexpr ValueKind value_kind_v<AddInst>    = ValueKind::AddInstVal;
template <> inline constexpr ValueKind value_kind_v<SubInst>    = ValueKind::SubInstVal;
template <> inline constexpr ValueKind value_kind_v<MulInst>    = ValueKind::MulInstVal;
template <> inline constexpr ValueKind value_kind_v<DivInst>    = ValueKind::DivInstVal;
template <> inline constexpr ValueKind value_kind_v<EqInst>     = ValueKind::EqInstVal;
template <> inline constexpr ValueKind value_kind_v<NeInst>     = ValueKind::NeInstVal;
template <> inline constexpr ValueKind value_kind_v<SltInst>    = ValueKind::SltInstVal;
template <> inline constexpr ValueKind value_kind_v<CallInst>   = ValueKind::CallInstVal;
template <> inline constexpr ValueKind value_kind_v<RetInst>    = ValueKind::RetInstVal;
template <> inline constexpr ValueKind value_kind_v<BranchInst> = ValueKind::BranchInstVal;
template <> inline constexpr ValueKind value_kind_v<PtrAdd>     = ValueKind::PtrAddVal;
template <> inline constexpr ValueKind value_kind_v<PhiInst>    = ValueKind::PhiInstVal; // inst end
template <> inline constexpr ValueKind value_kind_v<Literal>    = ValueKind::IntLiteralVal;


template <typename T, DerivedFromValue V>
bool isa(const std::unique_ptr<V>& value)
{
    return value->kind_ == value_kind_v<T>;
}

template <typename T, DerivedFromValue V>
bool isa(V* value)
{
    return value->kind_ == value_kind_v<T>;
}

template <typename T, DerivedFromValue V>
T* dyn_cast(const std::unique_ptr<V>& value)
{
    if (!value)
        return nullptr;

    if (value->kind_ == value_kind_v<T>)
        return static_cast<T*>(value.get());

    return nullptr;
}

template <typename T, DerivedFromValue V>
T* dyn_cast(V* value)
{
    if (!value)
        return nullptr;

    if (value->kind_ == value_kind_v<T>)
        return static_cast<T*>(value);

    return nullptr;
}

template <DerivedFromValue V>
bool is_instruction(std::unique_ptr<V>& value) 
{
    return value->kind_ >= ValueKind::AllocaInstVal && value->kind_ <= ValueKind::PhiInstVal;
}

template <DerivedFromValue V>
bool is_instruction(V* value) 
{
    return value->kind_ >= ValueKind::AllocaInstVal && value->kind_ <= ValueKind::PhiInstVal;
}

template <DerivedFromValue V>
bool is_terminator(std::unique_ptr<V>& value) 
{
    return value->kind_ == ValueKind::RetInstVal || 
           value->kind_ == ValueKind::BranchInstVal;
}

template <DerivedFromValue V>
bool is_terminator(V* value) 
{
    return value->kind_ == ValueKind::RetInstVal || 
           value->kind_ == ValueKind::BranchInstVal;
}

