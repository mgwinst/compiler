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
            auto [type, name] = extract_info(var);
            auto* alloca = create_alloca(type, name);

            if (!var.has_initializer()) {
                return alloca;
            } else {
                auto* init_value = lower(*var.init_);
                return create_store(alloca, init_value);
            }
        }

        case SemaNodeKind::ParamDecl: {
            const auto& param = node.as<Sema::ParamDecl>();
            auto [type, name] = extract_info(param);
            auto* arg = create_arg(name);
            auto* alloca = create_alloca(type, name);
            auto* store = create_store(alloca, arg);

            return nullptr;
        }

        case SemaNodeKind::FuncDecl: {
            const auto& func = node.as<Sema::FuncDecl>();
            auto* function = create_function(func);

            auto ret_type = ctx_.get_type(func.type_id_).as<FunctionType>().return_type_;
            if (ret_type != VOID) {
                auto* ret_val = create_alloca(ret_type, "retval");
                auto* ret_block = create_basic_block("return");
                current_function()->initialize_return(ret_val, ret_block);

                auto* ret = create_load(ret_type, ret_val, ret_block);
                create_ret(ret, ret_block);
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
            create_store(current_function()->return_value_, value);
            create_br(current_function()->return_block_);

            return nullptr;
        }

        case SemaNodeKind::BreakStmt: {
            const auto& brk_stmt = node.as<Sema::BreakStmt>();
            return create_br(get_loop_context().end_);
        };

        case SemaNodeKind::ContinueStmt: {
            const auto& cont_stmt = node.as<Sema::BreakStmt>();
            return create_br(get_loop_context().header_);
        };

        case SemaNodeKind::IfStmt: {
            const auto& if_stmt = node.as<Sema::IfStmt>();
    
            auto* starting_block = current_block();
            auto* cond = lower(if_stmt.cond_);

            auto* if_then_block = create_basic_block("if.then");
            auto* if_end_block = create_basic_block("if.end");

            set_current_block(if_then_block);
            lower(if_stmt.then_stmt_);      
            create_br(if_end_block);

            set_current_block(starting_block);
            create_br(cond, if_then_block, if_end_block);
            
            set_current_block(if_end_block);

            return nullptr;
        }

        case SemaNodeKind::WhileStmt: {
            const auto& while_stmt = node.as<Sema::WhileStmt>();
            
            auto* preheader = current_block();
            auto* cond = create_basic_block("loop.cond"); // loop header
            auto* body = create_basic_block("loop.body");
            auto* end = create_basic_block("loop.end");

            push_loop_context(preheader, cond, body, end);

            set_current_block(preheader);
            create_br(cond);

            set_current_block(cond);
            auto* cmp = lower(while_stmt.cond_); // assert(cmp is binary instruction)
            create_br(cmp, body, end);
            
            set_current_block(body);
            lower(while_stmt.body_);
            create_br(cond);

            set_current_block(end);

            pop_loop_context();

            return nullptr;
        }

        case SemaNodeKind::IntegerLiteralExpr: {
            const auto& int_literal = node.as<Sema::IntegerLiteralExpr>();
            return intern_literal(int_literal.value_);
        }

        case SemaNodeKind::FloatLiteralExpr: {
            const auto& float_literal = node.as<Sema::FloatLiteralExpr>();
            return intern_literal(float_literal.value_);
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

            switch (op) {
                case BinaryOp::Assign: return create_store(left, right);
                case BinaryOp::Add:    return create_add(left, right);
                case BinaryOp::Sub:    return create_sub(left, right);
                case BinaryOp::Mul:    return create_mul(left, right);
                case BinaryOp::Div:    return create_div(left, right);
                case BinaryOp::Eq:     return create_eq(left, right);
                case BinaryOp::Ne:     return create_ne(left, right);
                case BinaryOp::Slt:    return create_slt(left, right);

                default:
                    error_exit("binary op error");
            }
        }

        case SemaNodeKind::ReferenceExpr: {
            const auto& ref = node.as<Sema::ReferenceExpr>();
            return create_load(ref.type_id_, get_value(ref));
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
            auto* index = create_literal(static_cast<int64_t>(field_index)); 

            // assert type is ptr or record
            auto* base_ptr = get_value(base);

            return create_ptradd(base_ptr, index);
        };

        case SemaNodeKind::ArraySubscriptExpr: {
            const auto& expr = node.as<Sema::ArraySubscriptExpr>();
            const auto& base = tree_.nodes_[expr.base_].as<Sema::ReferenceExpr>();

            // assert type is ptr or record
            auto* base_ptr = get_value(base);

            auto* index = lower(expr.index_);

            return create_ptradd(base_ptr, index);
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

IR::Function* LoweringEngine::create_function(const Sema::FuncDecl& func)
{
    auto& func_name = ctx_.get_symbol(func).identifier_;
    auto function = std::make_unique<IR::Function>();
    function->set_name(func_name);

    set_current_function(function.get());
    value_count_ = 0;
    auto* entry_block = create_basic_block("entry");
    set_current_block(entry_block);

    return program_.insert(std::move(function));
}

IR::Argument* LoweringEngine::create_arg(std::string_view name)
{
    auto arg = std::make_unique<IR::Argument>();
    arg->set_name(name);
    return current_function()->insert(std::move(arg));
}

IR::BasicBlock* LoweringEngine::create_basic_block(std::string_view name)
{
    auto block = std::make_unique<IR::BasicBlock>(name);
    return current_function()->insert(std::move(block));
}

IR::Instruction* LoweringEngine::create_alloca(TypeID type_id, std::string_view name, IR::BasicBlock* block)
{
    auto alloca = std::make_unique<AllocaInst>(type_id, name);
    name_to_value_map_[alloca->get_name()] = alloca.get();

    if (!block)
        block = current_block();

    return block->insert(std::move(alloca));
}

IR::Instruction* LoweringEngine::create_load(TypeID type_id, IR::Value* ptr, IR::BasicBlock* block)
{
    return create<IR::LoadInst>(block, type_id, ptr);
}

IR::Instruction* LoweringEngine::create_store(IR::Value* dst, IR::Value* src, IR::BasicBlock* block)
{
    return create<IR::StoreInst>(block, dst, src);
}

IR::Instruction* LoweringEngine::create_add(IR::Value* src1, IR::Value* src2, IR::BasicBlock* block)
{
    return create<IR::AddInst>(block, src1, src2);
}

IR::Instruction* LoweringEngine::create_sub(IR::Value* src1, IR::Value* src2, IR::BasicBlock* block)
{
    return create<IR::SubInst>(block, src1, src2);
}

IR::Instruction* LoweringEngine::create_mul(IR::Value* src1, IR::Value* src2, IR::BasicBlock* block)
{
    return create<IR::MulInst>(block, src1, src2);
}

IR::Instruction* LoweringEngine::create_div(IR::Value* src1, IR::Value* src2, IR::BasicBlock* block)
{
    return create<IR::DivInst>(block, src1, src2);
}

IR::Instruction* LoweringEngine::create_eq(IR::Value* src1, IR::Value* src2, IR::BasicBlock* block)
{
    return create<IR::EqInst>(block, src1, src2);
}

IR::Instruction* LoweringEngine::create_ne(IR::Value* src1, IR::Value* src2, IR::BasicBlock* block)
{
    return create<IR::NeInst>(block, src1, src2);
}

IR::Instruction* LoweringEngine::create_slt(IR::Value* src1, IR::Value* src2, IR::BasicBlock* block)
{
    return create<IR::SltInst>(block, src1, src2);
}

IR::Instruction* LoweringEngine::create_ret(IR::Value* src, IR::BasicBlock* block)
{
    return create<IR::Terminator>(block, src);
}

IR::Instruction* LoweringEngine::create_br(IR::BasicBlock* target, IR::BasicBlock* block)
{
    return create<IR::Terminator>(block, target);
}

IR::Instruction* LoweringEngine::create_br(IR::Value* cond, IR::BasicBlock* target1, IR::BasicBlock* target2, IR::BasicBlock* block)
{
    return create<IR::Terminator>(block, cond, target1, target2);
}

IR::Instruction* LoweringEngine::create_ptradd(IR::Value* base_ptr, IR::Value* index, IR::BasicBlock* block)
{
    return create<IR::PtrAdd>(block, base_ptr, index);
}

IR::Function* LoweringEngine::current_function() const 
{ 
    return current_function_; 
}

IR::BasicBlock *LoweringEngine::current_block() const 
{ 
    return current_basic_block_;
}

void LoweringEngine::set_current_function(IR::Function* function) 
{ 
    current_function_ = function;
}

void LoweringEngine::set_current_block(IR::BasicBlock* block) 
{ 
    current_basic_block_ = block;
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