#pragma once

#include "../../frontend/sema/sematree.hpp"
#include "../../middleend/ir/IR.hpp"

struct ModuleContext;

// type_to_str is used in the type checker, so overload it in pretty printer
std::string type_to_str(const ModuleContext&, TypeID type_ref);
std::string node_to_str(const ModuleContext& ctx, const SemaTree& tree, const SemaNodeID node_id, std::string indent);

class PrettyPrinter
{
public:
    PrettyPrinter(ModuleContext& ctx) :
        ctx_{ ctx } {}  

    void print(const SemaTree& tree) const;
    void print(Program& program) const;

private:
    ModuleContext& ctx_;

    std::string ir_value_to_str(Value* value) const;
    std::string ir_type_str(TypeID type_id) const;
};

