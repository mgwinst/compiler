#pragma once

#include "../../frontend/sema/sematree.hpp"
#include "../../context/context.hpp"
#include "../../lowering/ir.hpp"

std::string type_to_str(const ModuleContext&, TypeID type_ref);
std::string node_to_str(const ModuleContext& ctx, const SemaTree& tree, const SemaNodeID node_id, std::string indent);
std::string ir_value_to_str(IR::Value* value);

class PrettyPrinter
{
public:
    PrettyPrinter(ModuleContext& ctx) :
        ctx_{ ctx } {}  

    void print(SemaTree& tree) const
    {
        std::println("{}\n", node_to_str(ctx_, tree, tree.root(), ""));
    }

    void print(IR::Program& program) const
    {
        for (auto* function : program.functions()) {
            auto* func = static_cast<IR::Function*>(function);
            std::print("define @{}() -> () ", func->name_);
            std::cout << "{\n";
            for (auto* basic_block : func->blocks_) {
                auto* block = static_cast<IR::BasicBlock*>(basic_block);
                std::println("{}:", basic_block->name_);
                for (auto* value : block->instructions_) {
                    std::print("  {}", ir_value_to_str(value));
                }
            }
            std::cout << "}\n";
        }
    }

private:
    ModuleContext& ctx_;
};

