#pragma once

#include "../ir/Value.hpp"

using namespace IR;

template <typename T>
inline constexpr ValueKind value_kind_v = ValueKind::Invalid;

template <> inline constexpr ValueKind value_kind_v<Function>       = ValueKind::FunctionVal;
template <> inline constexpr ValueKind value_kind_v<BasicBlock>     = ValueKind::BasicBlockVal;
template <> inline constexpr ValueKind value_kind_v<Argument>       = ValueKind::ArgumentVal;
template <> inline constexpr ValueKind value_kind_v<AllocaInst>     = ValueKind::AllocaInstVal;
template <> inline constexpr ValueKind value_kind_v<LoadInst>       = ValueKind::LoadInstVal;
template <> inline constexpr ValueKind value_kind_v<StoreInst>      = ValueKind::StoreInstVal;
template <> inline constexpr ValueKind value_kind_v<AddInst>        = ValueKind::AddInstVal;
template <> inline constexpr ValueKind value_kind_v<SubInst>        = ValueKind::SubInstVal;
template <> inline constexpr ValueKind value_kind_v<MulInst>        = ValueKind::MulInstVal;
template <> inline constexpr ValueKind value_kind_v<DivInst>        = ValueKind::DivInstVal;
template <> inline constexpr ValueKind value_kind_v<EqInst>         = ValueKind::EqInstVal;
template <> inline constexpr ValueKind value_kind_v<NeInst>         = ValueKind::NeInstVal;
template <> inline constexpr ValueKind value_kind_v<SltInst>        = ValueKind::SltInstVal;
template <> inline constexpr ValueKind value_kind_v<PhiInst>        = ValueKind::PhiInstVal;
template <> inline constexpr ValueKind value_kind_v<ConstantInt>    = ValueKind::ConstantIntVal;
template <> inline constexpr ValueKind value_kind_v<ConstantFloat>  = ValueKind::ConstantFloatVal;
template <> inline constexpr ValueKind value_kind_v<Terminator>     = ValueKind::TerminatorVal;


template <typename T>
bool isa(Value* value)
{
    return value->get_kind() == value_kind_v<T>;
}

template <typename T>
T* dyn_cast(Value* value)
{
    if (value->get_kind() == value_kind_v<T>)
        return static_cast<T*>(value);
    return nullptr;
}