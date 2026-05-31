#include "IR.hpp"

Value::Value(ValueKind kind, TypeID type_id, std::string_view name, Value* parent) :
    kind_{ kind },
    type_id_{ type_id },
    name_{ name },
    users_{ },
    parent_{ parent } {}

void Value::add_use(Value* value)
{
    if (value)
        users_.push_back(value);
}