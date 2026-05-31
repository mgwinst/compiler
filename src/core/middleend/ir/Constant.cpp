#include "IR.hpp"

Literal::Literal(int64_t data) : 
    Value{ValueKind::Const, INT32, std::to_string(data)+"L"},
    data_{ data } {}

