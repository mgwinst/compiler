#include <ranges>
#include <stack>

#include "../../context/context.hpp"
#include "../../frontend/sema/sematree.hpp"
#include "../../utils/alias.hpp"
#include "../ir/IRBuilder.hpp"

struct LoopContext
{
    IR::BasicBlock *preheader_, *header_, *body_, *end_;
};

class LoweringEngine
{
public:
    LoweringEngine(const ModuleContext& ctx, const SemaTree& tree) :
        ctx_{ ctx }, 
        tree_{ tree },
        program_{ },
        builder_{ program_ } {}

    IR::Program run();

private:
    const ModuleContext& ctx_;
    const SemaTree& tree_;
    IR::Program program_;
    IRBuilder builder_;

    std::unordered_map<std::string, IR::Value*> name_to_value_map_; // scope aware? // this must outlive the LoweringEngine object
    std::stack<LoopContext> loop_context_stack_;
    uint32_t value_count_ = 0u; // reset per new function

    IR::Value* lower(SemaNodeID node_id);
    
    IR::Function* current_function() const;
    IR::BasicBlock* current_block() const;
    void set_current_function(IR::Function* function);
    void set_current_block(IR::BasicBlock* block);

    IR::Value* get_value(const Sema::ReferenceExpr& ref) const; // {ref.name : Value*}

    void push_loop_context(BasicBlock* preheader, BasicBlock* header, BasicBlock* body, BasicBlock* end);
    void pop_loop_context();
    LoopContext& get_loop_context();

    std::tuple<TypeID, std::string> extract_info(const ContainsSymbol auto& node); // rewrite this to be more generic
};



// also explore pass manager (passing in objects that inherit from 'pass')