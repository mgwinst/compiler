#pragma once

#include "../../frontend/sema/sematree.hpp"
#include "../../context/context.hpp"
#include "../../ir/Value.hpp"

std::string type_to_str(const ModuleContext&, TypeID type_ref);
std::string node_to_str(const ModuleContext& ctx, const SemaTree& tree, const SemaNodeID node_id, std::string indent);
std::string ir_value_to_str(IR::Value* value);

class PrettyPrinter
{
public:
    PrettyPrinter(ModuleContext& ctx) :
        ctx_{ ctx } {}  

    void print(const SemaTree& tree) const;
    void print(const IR::Program& program) const;

private:
    ModuleContext& ctx_;
};

