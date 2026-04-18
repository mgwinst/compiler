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
            auto* alloca = create<IR::AllocaInst>(var);

            if (!var.has_initializer()) {
                return alloca;
            } else {
                auto* init_value = lower(*var.init_);
                return create<IR::StoreInst>(alloca, init_value);
            }
        }

        case SemaNodeKind::FuncDecl: {
            const auto& func = node.as<Sema::FuncDecl>();
            auto* function = create<IR::Function>(func); // implicitly creates entry basic block

            for (auto x : func.params_) {
                const auto& param = tree_.nodes_[x].as<Sema::ParamDecl>();
                auto* arg = create<IR::Argument>(x);
                auto* alloca = create<IR::AllocaInst>(param);
                auto* store = create<IR::StoreInst>(alloca, arg);
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

            auto* if_then_block = create<IR::BasicBlock>("if.then");
            auto* if_end_block = create<IR::BasicBlock>("if.end");

            set_current_block(static_cast<BasicBlock*>(if_then_block));
            lower(if_stmt.then_stmt_);           
            create<IR::Terminator>(TerminatorKind::Branch, static_cast<BasicBlock*>(if_end_block));

            set_current_block(starting_block);

            auto* terminator = create<IR::Terminator>(TerminatorKind::Branch, cond, static_cast<BasicBlock*>(if_then_block), static_cast<BasicBlock*>(if_end_block));
            
            set_current_block(static_cast<BasicBlock*>(if_end_block));

            return terminator;
        }

        case SemaNodeKind::BinaryExpr: {
            const auto& binary = node.as<Sema::BinaryExpr>();
            auto* left = lower(binary.left_);
            auto* right = lower(binary.right_);

            const BinaryOp op = binary_ops[binary.op_];

            switch (op) {
                case BinaryOp::Assign: return create<IR::StoreInst>(left, right);
                case BinaryOp::Add:    return create<IR::AddInst>(left, right);
                case BinaryOp::Sub:    return create<IR::SubInst>(left, right);
                case BinaryOp::Mul:    return create<IR::MulInst>(left, right);
                case BinaryOp::Div:    return create<IR::DivInst>(left, right);
                case BinaryOp::Eq:     return create<IR::EqInst>(left, right);
                case BinaryOp::Ne:     return create<IR::NeInst>(left, right);
                case BinaryOp::Slt:    return create<IR::SltInst>(left, right);

                default:
                    std::println("binary op error");
                    std::terminate();

            }
        }

        case SemaNodeKind::ReferenceExpr: {
            const auto& ref = node.as<Sema::ReferenceExpr>();
            return create<IR::LoadInst>(get_value(ref));
        }

        default:
            std::println("error T={}", (int)node.get_kind());
            break;
    }
}