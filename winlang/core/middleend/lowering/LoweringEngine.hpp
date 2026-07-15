#include <ranges>
#include <stack>

#include "context/context.hpp"
#include "frontend/sema/sematree.hpp"
#include "middleend/ir/IRBuilder.hpp"

struct LoopContext
{
    BasicBlock *preheader_, *header_, *body_, *end_;
};

class LoweringEngine
{
public:
    LoweringEngine(ModuleContext& ctx, const SemaTree& tree);

    Program run();

private:
    ModuleContext& ctx_;
    const SemaTree& tree_;
    Program program_;
    IRBuilder builder_;

    std::unordered_map<std::string, Alloca*> alloca_map_; // must outlive LoweringEngine object
    std::unordered_map<std::string, Function*> function_map_;
    std::stack<LoopContext> loop_context_stack_;

    Value* lower(SemaNode* node_id);
    
    Function* current_function() const;
    BasicBlock* current_block() const;
    void set_current_function(Function* function);
    void set_current_block(BasicBlock* block);
    Value* get_value(Sema::ReferenceExpr* ref); // {ref.name : Value*} -> get alloca ptr for this variable
    void push_loop_context(BasicBlock* preheader, BasicBlock* header, BasicBlock* body, BasicBlock* end);
    void pop_loop_context();
    LoopContext& get_loop_context();
};