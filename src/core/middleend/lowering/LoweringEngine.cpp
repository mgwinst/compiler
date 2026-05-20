#include <unordered_map>
#include <string_view>
#include <ranges>
#include <utility>

#include "LoweringEngine.hpp"
#include "../../utils/enums.hpp"
#include "../../utils/casting.hpp"

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
            auto [type, name] = extract_info(var);

            auto* alloca = builder_.create<AllocaInst>(type, name);
            name_to_value_map_[name] = alloca;

            if (!var.has_initializer()) {
                return alloca;
            } else {
                auto* init_value = lower(*var.init_);
                if (isa<AllocaInst>(init_value))
                    init_value = builder_.create<LoadInst>(init_value);
                return builder_.create<StoreInst>(alloca, init_value);
            }
        }

        case SemaNodeKind::ParamDecl: {
            const auto& param = node.as<Sema::ParamDecl>();
            auto [type, name] = extract_info(param);
            auto* arg = builder_.create<Argument>(type, name);
            auto* alloca = builder_.create<AllocaInst>(type, name);
            auto* store = builder_.create<StoreInst>(alloca, arg);

            return nullptr;
        }
    
        case SemaNodeKind::FuncDecl: {
            const auto& func = node.as<Sema::FuncDecl>();
            auto [_, name] = extract_info(func);

            auto* function = builder_.create<Function>(name);
            set_current_function(function);

            auto* entry = builder_.create<BasicBlock>("entry");
            set_current_block(entry);

            auto ret_type = ctx_.get_type(func.type_id_).as<FunctionType>().return_type_;
            if (ret_type != VOID) {
                auto* ret_val = builder_.create<AllocaInst>(ret_type, "retval");
                auto* ret_block = builder_.create<BasicBlock>("return");
                current_function()->initialize_return(ret_val, ret_block);

                set_current_block(ret_block);
                auto* ret = builder_.create<LoadInst>(ret_type, ret_val);
                builder_.create<RetInst>(ret);
                set_current_block(entry);
            }

            for (auto p : func.params_) {
                lower(p);
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

        // if there is only one predecessor to a return block, remove branch and linearize
        case SemaNodeKind::ReturnStmt: {
            const auto& return_stmt = node.as<Sema::ReturnStmt>();
            auto* value = lower(return_stmt.value_);

            auto* load = builder_.create<LoadInst>(value);

            builder_.create<StoreInst>(current_function()->return_value_, load);
            builder_.create<BranchInst>(current_function()->return_block_);

            return nullptr;
        }

        case SemaNodeKind::BreakStmt: {
            const auto& brk_stmt = node.as<Sema::BreakStmt>();
            return builder_.create<BranchInst>(get_loop_context().end_);
        };

        case SemaNodeKind::ContinueStmt: {
            const auto& cont_stmt = node.as<Sema::BreakStmt>();
            return builder_.create<BranchInst>(get_loop_context().header_);
        };

        case SemaNodeKind::IfStmt: {
            const auto& if_stmt = node.as<Sema::IfStmt>();
    
            auto* header = current_block();
            auto* cond = lower(if_stmt.cond_);

            auto* if_then_block = builder_.create<BasicBlock>("if.then");
            auto* if_else_block = builder_.create<BasicBlock>("if.else");
            auto* if_end_block = builder_.create<BasicBlock>("if.end");

            set_current_block(if_then_block);
            lower(if_stmt.then_stmt_);      
            builder_.create<BranchInst>(if_end_block);

            if (if_stmt.else_stmt_.has_value()) {
                set_current_block(if_else_block);
                lower(*if_stmt.else_stmt_);
                builder_.create<BranchInst>(if_end_block);
            }

            set_current_block(header);

            builder_.create<BranchInst>(cond, if_then_block, if_else_block);
            
            set_current_block(if_end_block);

            return nullptr;
        }

        case SemaNodeKind::WhileStmt: {
            const auto& while_stmt = node.as<Sema::WhileStmt>();
            
            auto* preheader = current_block();
            auto* cond = builder_.create<BasicBlock>("loop.cond"); // loop header
            auto* body = builder_.create<BasicBlock>("loop.body");
            auto* end = builder_.create<BasicBlock>("loop.end");

            push_loop_context(preheader, cond, body, end);

            builder_.create<BranchInst>(cond);

            set_current_block(cond);
            auto* cmp = lower(while_stmt.cond_); // assert(cmp is binary instruction)
            builder_.create<BranchInst>(cmp, body, end);
            
            set_current_block(body);
            lower(while_stmt.body_);
            builder_.create<BranchInst>(cond);

            set_current_block(end);

            pop_loop_context();

            return nullptr;
        }

        case SemaNodeKind::IntegerLiteralExpr: {
            const auto& int_literal = node.as<Sema::IntegerLiteralExpr>();
            return builder_.get_or_create_literal(int_literal.value_);
        }

        case SemaNodeKind::FloatLiteralExpr: {
            return nullptr;
        }

        case SemaNodeKind::CharLiteralExpr: {
            return nullptr;
        };

        case SemaNodeKind::StringLiteralExpr: {
            return nullptr;
        };

        case SemaNodeKind::BooleanLiteralExpr: {
            return nullptr;
        };

        case SemaNodeKind::UnaryExpr: {
            return nullptr;
        };

        case SemaNodeKind::BinaryExpr: {
            const auto& binary = node.as<Sema::BinaryExpr>();
            auto* left = lower(binary.left_);
            auto* right = lower(binary.right_);

            const BinaryOp op = binary_ops[binary.op_];

            assert(op != BinaryOp::Invalid);

            if (op == BinaryOp::Assign) {
                if (isa<Literal>(right)) {
                    return builder_.create<StoreInst>(left, right);
                } else {
                    return builder_.create<StoreInst>(left, builder_.create<LoadInst>(right));
                }
            }

            if (!isa<Literal>(left))
                left = builder_.create<LoadInst>(left);

            if (!isa<Literal>(right))
                right = builder_.create<LoadInst>(right);

            switch (op) {
                case BinaryOp::Add:    return builder_.create<AddInst>(left, right);
                case BinaryOp::Sub:    return builder_.create<SubInst>(left, right);
                case BinaryOp::Mul:    return builder_.create<MulInst>(left, right);
                case BinaryOp::Div:    return builder_.create<DivInst>(left, right);
                case BinaryOp::Eq:     return builder_.create<EqInst>(left, right);
                case BinaryOp::Ne:     return builder_.create<NeInst>(left, right);
                case BinaryOp::Slt:    return builder_.create<SltInst>(left, right);

                default:
                    error_exit("binary op error");
            }
        }

        case SemaNodeKind::ReferenceExpr: {
            const auto& ref = node.as<Sema::ReferenceExpr>();
            return get_value(ref);
        }

        case SemaNodeKind::CallExpr: {
            const auto& call = node.as<Sema::CallExpr>();
              
            return nullptr;
        }

        case SemaNodeKind::MemberExpr: {
            const auto& expr = node.as<Sema::MemberExpr>();
            const auto& base = tree_.nodes_[expr.base_].as<Sema::ReferenceExpr>();
            auto field_index = ctx_.get_type(base.type_id_).as<RecordType>().field_position(expr.member_);
            
            // make this a Value literal? yes to keep uniformity
            auto* index = builder_.get_or_create_literal(static_cast<int64_t>(field_index)); 

            // assert type is ptr or record
            auto* base_ptr = get_value(base);

            return builder_.create<PtrAdd>(base_ptr, index);
        };

        case SemaNodeKind::ArraySubscriptExpr: {
            const auto& expr = node.as<Sema::ArraySubscriptExpr>();
            const auto& base = tree_.nodes_[expr.base_].as<Sema::ReferenceExpr>();

            // assert type is ptr or record
            auto* base_ptr = get_value(base);

            auto* index = lower(expr.index_);

            return builder_.create<PtrAdd>(base_ptr, index);
        }

        case SemaNodeKind::InitListExpr: {
            return nullptr;
        }

        case SemaNodeKind::ExplicitCastExpr: {
            return nullptr;
        }

        default: {
            return nullptr;
        }
    }
}













IR::Function* LoweringEngine::current_function() const 
{ 
    return builder_.current_function_; 
}

IR::BasicBlock* LoweringEngine::current_block() const 
{ 
    return builder_.current_block_;
}

void LoweringEngine::set_current_function(Function* function)
{ 
    builder_.current_function_ = function;
}

void LoweringEngine::set_current_block(BasicBlock* block) 
{ 
    builder_.current_block_ = block;
}

IR::Value* LoweringEngine::get_value(const Sema::ReferenceExpr& ref) const
{
    auto& name = ctx_.get_symbol(ref).identifier_;
    if (auto it = name_to_value_map_.find(name); it != name_to_value_map_.end())
        return it->second;

    return nullptr;

    // assert() instead?
}

void LoweringEngine::push_loop_context(BasicBlock* preheader, BasicBlock* header, BasicBlock* body, BasicBlock* end)
{
    loop_context_stack_.emplace(preheader, header, body, end);
}

void LoweringEngine::pop_loop_context()
{
    loop_context_stack_.pop();
}

LoopContext& LoweringEngine::get_loop_context()
{
    return loop_context_stack_.top();
}

std::tuple<TypeID, std::string> LoweringEngine::extract_info(const ContainsSymbol auto& node)
{
    auto& symbol = ctx_.get_symbol(node);
    return {symbol.type_id_, symbol.identifier_};
}