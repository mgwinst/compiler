#include <unordered_map>
#include <string_view>
#include <ranges>
#include <utility>

#include "LoweringEngine.hpp"
#include "frontend/sema/helpers.hpp"
#include "utils/casting.hpp"
#include "utils/enums.hpp"

using namespace Sema;

LoweringEngine::LoweringEngine(ModuleContext& ctx, const SemaTree& tree) :
    ctx_{ ctx }, 
    tree_{ tree },
    program_{ },
    builder_{ &program_ } {}

Program LoweringEngine::run()
{
    lower(tree_.root_);
    return std::move(program_);
}

bool load_required(SemaNode* node)
{
    if (isa<ReferenceExpr>(node))
        return true;
    
    if (auto* unary = dyn_cast<UnaryExpr>(node)) {
        if (unary->op_ == "*") {
            return true;
        }
    }
    
    return false;
}

Value* LoweringEngine::lower(SemaNode* node)
{
    switch (node->kind_) {
        case SemaNodeKind::ModuleDecl: {
            auto* module = cast<ModuleDecl>(node);
            
            for (auto* decl : module->decls_) {
                lower(decl);
            }

            return nullptr;
        }

        case SemaNodeKind::VarDecl: {
            auto* var = cast<VarDecl>(node);

            auto* alloca = builder_.create<Alloca>(var->type(), var->name());
            alloca_map_[var->name()] = alloca;

            if (!var->init_) {
                return alloca;
            } else {
                auto* init_value = lower(var->init_);

                if (load_required(var->init_))
                    init_value = builder_.create<Load>(init_value);

                return builder_.create<Store>(alloca, init_value);
            }
        }

        case SemaNodeKind::ParamDecl: {
            auto* param = cast<ParamDecl>(node);
 
            auto* arg = builder_.create<Argument>(param->type(), param->name());
            auto* alloca = builder_.create<Alloca>(param->type(), param->name() + ".ptr");
            alloca_map_[param->name()] = alloca;
            auto* store = builder_.create<Store>(alloca, arg);

            return nullptr;
        }
    
        case SemaNodeKind::FuncDecl: {
            auto* func = cast<FuncDecl>(node);

            auto* function = builder_.create<Function>(func->name());
            function_map_[func->name()] = function;
            set_current_function(function);

            auto* entry = builder_.create<BasicBlock>("entry");
            set_current_block(entry);

            auto* return_type = cast<FunctionType>(func->type())->return_type_;

            current_function()->return_block_ = builder_.create<BasicBlock>("return");

            if (return_type == ctx_.type_table_.builtin_types_["void"]) {
                set_current_block(current_function()->return_block_);
                builder_.create<Return>();
            } else {
                current_function()->return_value_ = builder_.create<Alloca>(return_type, "retval"); // technically doesn't need to be in alloca map, we can't anyway because of collision (many retvals in the graph)
                set_current_block(current_function()->return_block_);
                builder_.create<Return>(builder_.create<Load>(return_type, current_function()->return_value_));
            }

            set_current_block(entry);

            for (auto* p : func->params_) {
                lower(p);
            }

            lower(func->body_);

            return function;
        }

        case SemaNodeKind::CompoundStmt: {
            auto* compound = cast<CompoundStmt>(node);
            for (auto c : compound->children_) {
                lower(c);
            }

            return nullptr;
        }

        case SemaNodeKind::ReturnStmt: {
            auto* return_stmt = cast<ReturnStmt>(node);

            if (return_stmt->value_) {
                auto* value = lower(return_stmt->value_);

                if (load_required(return_stmt->value_))
                    value = builder_.create<Load>(value);

                builder_.create<Store>(current_function()->return_value_, value);
            }

            builder_.create<Branch>(current_function()->return_block_);

            return nullptr;
        }

        case SemaNodeKind::BreakStmt: {
            return builder_.create<Branch>(get_loop_context().end_);
        };

        case SemaNodeKind::ContinueStmt: {
            return builder_.create<Branch>(get_loop_context().header_);
        };

        case SemaNodeKind::IfStmt: {
            auto* if_stmt = cast<IfStmt>(node);
    
            auto* header_block = current_block();
            auto* cond = lower(if_stmt->cond_);

            auto* if_then_block = builder_.create<BasicBlock>("if.then");
            auto* if_end_block = builder_.create<BasicBlock>("if.end");

            set_current_block(if_then_block);
            lower(if_stmt->then_stmt_);      
            builder_.create<Branch>(if_end_block);

            BasicBlock* if_else_block = nullptr;
            if (if_stmt->else_stmt_) {
                if_else_block = builder_.create<BasicBlock>("if.else");
                set_current_block(if_else_block);
                lower(if_stmt->else_stmt_);
                builder_.create<Branch>(if_end_block);
            }

            set_current_block(header_block);

            if (if_else_block) {
                builder_.create<Branch>(cond, if_then_block, if_else_block);
            } else {
                builder_.create<Branch>(cond, if_then_block, if_end_block);
            }
            
            set_current_block(if_end_block);

            return nullptr;
        }

        case SemaNodeKind::WhileStmt: {
            auto* while_stmt = cast<WhileStmt>(node);
            
            auto* preheader = current_block();
            auto* cond = builder_.create<BasicBlock>("loop.cond"); // loop header
            auto* body = builder_.create<BasicBlock>("loop.body");
            auto* end = builder_.create<BasicBlock>("loop.end");

            push_loop_context(preheader, cond, body, end);

            builder_.create<Branch>(cond);

            set_current_block(cond);
            auto* cmp = lower(while_stmt->cond_); // assert(cmp is binary instruction)
            builder_.create<Branch>(cmp, body, end);
            
            set_current_block(body);
            lower(while_stmt->body_);
            builder_.create<Branch>(cond);

            set_current_block(end);

            pop_loop_context();

            return nullptr;
        }

        case SemaNodeKind::IntegerLiteralExpr: {
            auto* int_literal = cast<IntegerLiteralExpr>(node);
            return builder_.get_or_create_literal(int_literal->value_);
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
            auto* bool_literal = cast<BooleanLiteralExpr>(node);
            return bool_literal->value_ ? 
                builder_.get_or_create_literal(static_cast<int64_t>(1)) :
                builder_.get_or_create_literal(static_cast<int64_t>(0));
        };

        case SemaNodeKind::UnaryExpr: {
            auto* unary = cast<UnaryExpr>(node);

            auto* operand = lower(unary->operand_);

            // address of
            if (unary->op_ == "&")
                return operand;
            
            // pointer dereference
            if (unary->op_ == "*")
                return builder_.create<Load>(operand);

            return nullptr;
        };

        case SemaNodeKind::BinaryExpr: {
            auto* binary = cast<BinaryExpr>(node);

            const BinaryOp op = binary_ops[binary->op_];
            assert(op != BinaryOp::Invalid);

            auto* left = lower(binary->left_);
            auto* right = lower(binary->right_);

            if (op == BinaryOp::Assign) {
                if (load_required(binary->right_))
                    right = builder_.create<Load>(right);
                return builder_.create<Store>(left, right);
            }

            if (load_required(binary->left_))
                left = builder_.create<Load>(left);

            if (load_required(binary->right_))
                right = builder_.create<Load>(right);

            switch (op) {
                case BinaryOp::Add:  return builder_.create<Add>(left, right);
                case BinaryOp::Sub:  return builder_.create<Sub>(left, right);
                case BinaryOp::Mul:  return builder_.create<Mul>(left, right);
                case BinaryOp::Div:  return builder_.create<Div>(left, right);
                case BinaryOp::Mod:  return builder_.create<Mod>(left, right);
                case BinaryOp::Eq:   return builder_.create<Eq>(left, right);
                case BinaryOp::Ne:   return builder_.create<Ne>(left, right);
                case BinaryOp::Slt:  return builder_.create<Slt>(left, right);

                default:
                    error_exit("lowering binary op error");
            }
        }

        case SemaNodeKind::ReferenceExpr: {
            auto* ref = cast<ReferenceExpr>(node);
            return get_value(ref);
        }

        case SemaNodeKind::CallExpr: {
            auto* call = cast<CallExpr>(node);
            
            auto* callee = function_map_[cast<ReferenceExpr>(call->callee_)->name_];
            
            std::vector<Value*> args;
            for (auto* arg : call->args_) {
                auto* a = lower(arg);

                // this is a special case that is not caught by the generic algorithm for determining when to load
                // (think about why we are not loading on every reference automatically)
                if (isa<Alloca>(a))
                    a = builder_.create<Load>(a);

                args.push_back(a);
            }

            return builder_.create<Call>(callee, std::move(args));
        }

        case SemaNodeKind::MemberExpr: {
            auto* expr = cast<MemberExpr>(node);
            auto* base = cast<ReferenceExpr>(expr->base_);
            
            auto* record_type = cast<RecordType>(base->type());

            auto field_index = record_type->field_position(expr->member_);
            auto* index = builder_.get_or_create_literal(static_cast<int64_t>(field_index));

            auto* base_ptr = get_value(base);

            return builder_.create<PtrAdd>(base_ptr, index);
        };

        case SemaNodeKind::ArraySubscriptExpr: {
            auto* expr = cast<ArraySubscriptExpr>(node);
            auto* base = cast<ReferenceExpr>(expr->base_);

            // assert type is ptr or record
            auto* base_ptr = get_value(base);

            auto* index = lower(expr->index_);

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













Function* LoweringEngine::current_function() const 
{ 
    return builder_.current_function_; 
}

BasicBlock* LoweringEngine::current_block() const 
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

Value* LoweringEngine::get_value(ReferenceExpr* ref)
{
    if (auto it = alloca_map_.find(ref->name()); it != alloca_map_.end())
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