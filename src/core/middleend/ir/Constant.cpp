#include "IR.hpp"

Const::Const(int64_t data) : 
    Value{ValueKind::Const, nullptr, std::to_string(data)+"L"},
    data_{ data } {}

