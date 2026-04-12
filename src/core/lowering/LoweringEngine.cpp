#include <ranges>
#include <unordered_map>
#include <string_view>
#include <utility>

#include "LoweringEngine.hpp"
#include "ir.hpp"

using namespace IR;

Program LoweringEngine::run()
{
    lower(tree_.root());
    return std::move(program_);
}

Value* LoweringEngine::lower(SemaNodeID node_id)
{
    const auto& node = tree_.nodes_[node_id];

    switch (node.get_kind()) {
        case SemaNodeKind::ModuleDecl: {
            const auto& module = node.as<Sema::ModuleDecl>();
            
            for (auto decl : module.declarations()) {
                program_.append(lower(decl));
            }

            return nullptr;
        }

        case SemaNodeKind::VarDecl: {
            const auto& var = node.as<Sema::VarDecl>();
            // auto* alloca = create_alloca(type, align, name="");

            if (!var.has_initializer()) {
                return alloca;
            } else {
                auto* init_value = lower(*var.init_);
                return create_store(alloca, init_value);
            }
        }

        case SemaNodeKind::FuncDecl: {
            const auto& func = node.as<Sema::FuncDecl>();
            auto func_name = ctx_.symbol_table_.get_symbol(func.symbol_).identifier_;

            auto* function = create_function();
            function->name_ = func_name;

            create_basic_block();

            for (auto x : func.params_) {
                // get x's symbol

                auto* arg = create_argument(x);
                auto* alloca = create_alloca();
                auto* store = create_store(alloca, arg);
            }

            lower(func.body_);

            return function;
        }

        case SemaNodeKind::CompoundStmt: {
            const auto& compound = node.as<Sema::CompoundStmt>();
            for (auto c : compound.children_) {
                lower(c);
            }
            
            return nullptr;
        }

        case SemaNodeKind::BinaryExpr: {
            const auto& binary = node.as<Sema::BinaryExpr>();
            auto* left = lower(binary.left_);
            auto* right = lower(binary.right_);

            if (binary.op_ == "+") {
                return create_add(left, right);
            }
        }

        case SemaNodeKind::ReferenceExpr: {
            const auto& ref = node.as<Sema::ReferenceExpr>();
            return create_load(get_value(ref));
        }

        default:
            break;
    }
}

void LoweringEngine::set_current_function(Function* function)
{
    current_function_ = function;
}

void LoweringEngine::set_current_block(BasicBlock* block)
{
    current_basic_block_ = block;
}

Function* LoweringEngine::create_function()
{
    auto* function = new Function{};
    program_.append(function);
    set_current_function(function);

    return function;
}

BasicBlock* LoweringEngine::create_basic_block()
{
    auto* block = new BasicBlock{};
    set_current_block(block);
    current_function()->add_block(current_basic_block_);

    return block;
}

Value* LoweringEngine::create_argument(SemaNodeID node_id)
{
    auto arg_name = ctx_.symbol_table_.get_symbol(node_id).identifier_;
    auto* arg = new Argument{};
    arg->name_ = arg_name;

    current_function()->add_argument(arg);

    return static_cast<Value*>(arg);
}

// how many bytes? (type) how to find this pointer (name -> value*) (do we need this regime for all values? just memory ops? just allocas?)
Value* LoweringEngine::create_alloca(TypeId type, const std::string& name)
{
    return insert(new Instruction{Op::Alloca, type, name});
}

Value* LoweringEngine::create_store(Value* ptr, Value* value)
{
    return insert(new Instruction{Op::Store, ptr, value});
}

Value* LoweringEngine::create_load(Value* ptr)
{
    return insert(new Instruction{Op::Load, ptr});
}

Value* LoweringEngine::create_add(Value* src1, Value* src2)
{
    return insert(new Instruction{Op::Add, src1, src2});
}

Value* LoweringEngine::create_mul(Value* src1, Value* src2)
{
    return insert(new Instruction{Op::Mul, src1, src2});
}

Value* LoweringEngine::create_ret(Value* value)
{

}