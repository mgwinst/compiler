#include <ranges>
#include <unordered_map>
#include <string_view>
#include <utility>

#include "LoweringEngine.hpp"
#include "../utils/enums.hpp"

IR::Program LoweringEngine::run()
{
    lower(tree_.root());
    return std::move(program_);
}

IR::Value* LoweringEngine::lower(SemaNodeID node_id)
{
    const auto& node = tree_.nodes_[node_id];

    switch (node.get_kind()) {
        case SemaNodeKind::ModuleDecl: {
            const auto& module = node.as<Sema::ModuleDecl>();
            
            for (auto decl : module.declarations()) {
                lower(decl);
            }

            return nullptr;
        }

        case SemaNodeKind::VarDecl: {
            const auto& var = node.as<Sema::VarDecl>();
            auto* alloca = create_instruction<IR::AllocaInst>(var);

            if (!var.has_initializer()) {
                return alloca;
            } else {
                auto* init_value = lower(*var.init_);
                return create_instruction<IR::StoreInst>(alloca, init_value);
            }
        }

        case SemaNodeKind::FuncDecl: {
            const auto& func = node.as<Sema::FuncDecl>();
            auto* function = create_function(func); // implicitly creates entry basic block

            for (auto x : func.params_) {
                const auto& param = tree_.nodes_[x].as<Sema::ParamDecl>();
                auto* arg = create_argument(x);
                auto* alloca = create_instruction<IR::AllocaInst>(param);
                auto* store = create_instruction<IR::StoreInst>(alloca, arg);
            }

            lower(func.body_);

            return function;
        }

        case SemaNodeKind::CompoundStmt: {
            const auto& compound = node.as<Sema::CompoundStmt>();
            for (auto c : compound.children_)
                lower(c);

            return nullptr;
        }

        case SemaNodeKind::IfStmt: {
            const auto& if_stmt = node.as<Sema::IfStmt>();
    
            auto* starting_block = current_block();
            auto* cond = lower(if_stmt.cond_);

            auto* if_then_block = create_basic_block("if.then");
            auto* if_end_block = create_basic_block("if.end");

            set_current_block(if_then_block);
            lower(if_stmt.then_stmt_);      
            create_instruction<IR::Terminator>(if_end_block);

            set_current_block(starting_block);

            auto* terminator = create_instruction<IR::Terminator>(cond, if_then_block, if_end_block);
            
            set_current_block(static_cast<BasicBlock*>(if_end_block));

            return terminator;
        }

        case SemaNodeKind::IntegerLiteralExpr: {
            const auto& int_literal = node.as<Sema::IntegerLiteralExpr>();
            return intern_literal(int_literal.value_);
        }

        case SemaNodeKind::FloatLiteralExpr: {
            const auto& float_literal = node.as<Sema::FloatLiteralExpr>();
            return intern_literal(float_literal.value_);
        }

        case SemaNodeKind::BinaryExpr: {
            const auto& binary = node.as<Sema::BinaryExpr>();
            auto* left = lower(binary.left_);
            auto* right = lower(binary.right_);

            const BinaryOp op = binary_ops[binary.op_];

            switch (op) {
                case BinaryOp::Assign: return create_instruction<IR::StoreInst>(left, right);
                case BinaryOp::Add:    return create_instruction<IR::AddInst>(left, right);
                case BinaryOp::Sub:    return create_instruction<IR::SubInst>(left, right);
                case BinaryOp::Mul:    return create_instruction<IR::MulInst>(left, right);
                case BinaryOp::Div:    return create_instruction<IR::DivInst>(left, right);
                case BinaryOp::Eq:     return create_instruction<IR::EqInst>(left, right);
                case BinaryOp::Ne:     return create_instruction<IR::NeInst>(left, right);
                case BinaryOp::Slt:    return create_instruction<IR::SltInst>(left, right);

                default:
                    std::println("binary op error");
                    std::terminate();

            }
        }

        case SemaNodeKind::ReferenceExpr: {
            const auto& ref = node.as<Sema::ReferenceExpr>();
            return create_instruction<IR::LoadInst>(get_value(ref));
        }

        default:
            std::println("error T={}", (int)node.get_kind());
            break;
    }
}

IR::Function* LoweringEngine::create_function(const Sema::FuncDecl& func)
{
    auto& func_name = ctx_.get_symbol(func).identifier_;
    auto function = std::make_unique<IR::Function>();
    function->set_name(func_name);

    auto* ptr = function.get();

    set_current_function(ptr);
    reset_value_count();
    create_basic_block("entry");

    return program_.insert(std::move(function)); // return pointer
}

IR::Argument* LoweringEngine::create_argument(SemaNodeID node_id)
{
    auto arg_name = ctx_.symbol_table_.get_symbol(node_id).identifier_;
    auto arg = std::make_unique<IR::Argument>();
    arg->set_name(arg_name);
    return current_function()->insert(std::move(arg)); // insert<T>
}

IR::BasicBlock* LoweringEngine::create_basic_block(std::string_view name)
{
    auto block = std::make_unique<IR::BasicBlock>(name);
    set_current_block(block.get());
    return current_function()->insert(std::move(block)); // insert<T>
}