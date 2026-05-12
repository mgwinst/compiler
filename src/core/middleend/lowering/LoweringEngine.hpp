#include <ranges>
#include <stack>

#include "../../context/context.hpp"
#include "../../frontend/sema/sematree.hpp"
#include "../ir/Value.hpp"
#include "../../utils/alias.hpp"
#include "../../utils/casting.hpp"

struct LoopContext
{
    IR::BasicBlock *preheader_, *header_, *body_, *end_;
};

class LoweringEngine
{
public:
    LoweringEngine(const ModuleContext& ctx, const SemaTree& tree) :
        ctx_{ ctx }, 
        tree_{ tree } {}

    IR::Program run();

private:
    const ModuleContext& ctx_;
    const SemaTree& tree_;
    IR::Program program_;
    IR::Function* current_function_ = nullptr;
    IR::BasicBlock* current_basic_block_ = nullptr;
    std::unordered_map<std::string, IR::Value*> name_to_value_map_; // scope aware? // this must outlive the LoweringEngine object
    std::stack<LoopContext> loop_context_stack_;
    uint32_t value_count_ = 0u; // reset per new function

    IR::Value* lower(SemaNodeID node_id);

    template <DerivedFromInstruction T, typename... Args>
    IR::Instruction* create(IR::BasicBlock* block, Args&&... args);

    IR::Function* create_function(const Sema::FuncDecl& func);
    IR::BasicBlock* create_basic_block(std::string_view name = ""); // option to where to place block?
    IR::Argument* create_arg(std::string_view name = "");
    IR::Literal* create_literal(auto literal);
    IR::Instruction* create_alloca(TypeID type_id, std::string_view name, IR::BasicBlock* block = nullptr);
    IR::Instruction* create_load(TypeID type_id, IR::Value* ptr, IR::BasicBlock* block = nullptr);
    IR::Instruction* create_store(IR::Value* dst, IR::Value* src, IR::BasicBlock* block = nullptr);
    IR::Instruction* create_add(IR::Value* src1, IR::Value* src2, IR::BasicBlock* block = nullptr);
    IR::Instruction* create_sub(IR::Value* src1, IR::Value* src2, IR::BasicBlock* block = nullptr);
    IR::Instruction* create_mul(IR::Value* src1, IR::Value* src2, IR::BasicBlock* block = nullptr);
    IR::Instruction* create_div(IR::Value* src1, IR::Value* src2, IR::BasicBlock* block = nullptr);
    IR::Instruction* create_eq(IR::Value* src1, IR::Value* src2, IR::BasicBlock* block = nullptr);
    IR::Instruction* create_ne(IR::Value* src1, IR::Value* src2, IR::BasicBlock* block = nullptr);
    IR::Instruction* create_slt(IR::Value* src1, IR::Value* src2, IR::BasicBlock* block = nullptr);
    IR::Instruction* create_ret(IR::Value* src, IR::BasicBlock* block = nullptr);
    IR::Instruction* create_br(IR::BasicBlock* target, IR::BasicBlock* block = nullptr);
    IR::Instruction* create_br(IR::Value* cond, IR::BasicBlock* target1, IR::BasicBlock* target2, IR::BasicBlock* block = nullptr);
    IR::Instruction* create_ptradd(IR::Value* base_ptr, IR::Value* index, IR::BasicBlock* block = nullptr);
    
    IR::Function* current_function() const;
    IR::BasicBlock* current_block() const;
    void set_current_function(IR::Function* function);
    void set_current_block(IR::BasicBlock* block);

    IR::Value* get_value(const Sema::ReferenceExpr& ref) const; // map {ref.name : value*}
    IR::Literal* intern_literal(auto literal);
    void push_loop_context(BasicBlock* preheader, BasicBlock* header, BasicBlock* body, BasicBlock* end);
    void pop_loop_context();
    LoopContext& get_loop_context();
    std::tuple<TypeID, std::string> extract_info(const ContainsSymbol auto& node);
};

template <DerivedFromInstruction T, typename... Args>
IR::Instruction* LoweringEngine::create(IR::BasicBlock* block, Args&&... args)
{
    auto inst = std::make_unique<T>(std::forward<Args>(args)...);
    inst->set_name(std::to_string(++value_count_));

    if (!block)
        block = current_block();

    return block->insert(std::move(inst));
}

IR::Literal* LoweringEngine::intern_literal(auto literal)
{
    auto [it, inserted] = program_.constants().try_insert(literal); 
    return it->second.get();
}

IR::Literal* LoweringEngine::create_literal(auto literal)
{
    return intern_literal(literal);
}

// also explore pass manager (passing in objects that inherit from 'pass')