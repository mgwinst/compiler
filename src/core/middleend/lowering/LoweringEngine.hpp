#include <ranges>
#include <stack>

#include "../../context/context.hpp"
#include "../../frontend/sema/sematree.hpp"
#include "../../utils/alias.hpp"
#include "../ir/IRBuilder.hpp"

struct LoopContext
{
    BasicBlock *preheader_, *header_, *body_, *end_;
};

class LoweringEngine
{
public:
    LoweringEngine(const ModuleContext& ctx, const SemaTree& tree);

    Program run();

private:
    const ModuleContext& ctx_;
    const SemaTree& tree_;
    Program program_;
    IRBuilder builder_;

    std::unordered_map<std::string, Value*> name_to_value_map_; // scope aware? // this must outlive the LoweringEngine object?
    std::stack<LoopContext> loop_context_stack_;

    Value* lower(SemaNodeID node_id);
    
    Function* current_function() const;
    BasicBlock* current_block() const;
    void set_current_function(Function* function);
    void set_current_block(BasicBlock* block);
    Value* get_value(const Sema::ReferenceExpr& ref) const; // {ref.name : Value*}
    void push_loop_context(BasicBlock* preheader, BasicBlock* header, BasicBlock* body, BasicBlock* end);
    void pop_loop_context();
    LoopContext& get_loop_context();
    std::tuple<TypeID, const std::string&> extract_info(const ContainsSymbol auto& node); // rewrite this to be more generic
};