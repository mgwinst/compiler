#include "../context/context.hpp"
#include "../frontend/sema/sematree.hpp"
#include "../ir/Value.hpp"
#include "../utils/alias.hpp"
#include "../utils/casting.hpp"
#include "ConstantPool.hpp"

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
    std::unordered_map<std::string, IR::Value*> name_to_value_map_; // scope aware?
    ConstantPool intern_pool_; // unique IR::Literal instances for int/float/string

    uint32_t value_count_ = 0u; // reset per new function

    IR::Value* lower(SemaNodeID node_id);

    IR::Function* current_function() const { return current_function_; } 
    IR::BasicBlock* current_block() const { return current_basic_block_; } 
    void set_current_function(const std::unique_ptr<IR::Function>& function) { current_function_ = function.get(); }
    void set_current_block(const std::unique_ptr<IR::BasicBlock>& block) { current_basic_block_ = block.get(); }
    void set_current_function(IR::Function* function) { current_function_ = function; }
    void set_current_block(IR::BasicBlock* block) { current_basic_block_ = block; }

    IR::Function* create_function(const Sema::FuncDecl& func);
    IR::BasicBlock* create_basic_block(std::string_view name);
    IR::Argument* create_argument(SemaNodeID node_id);

    IR::Literal* create_literal(auto literal)
    {
        return intern_literal(literal);
    }

    template <std::same_as<IR::AllocaInst> T>
    IR::Instruction* create_instruction(const ContainsSymbol auto& node)
    {
        auto& symbol = ctx_.get_symbol(node);
        auto alloca = std::make_unique<T>(symbol.type_id_, symbol.identifier_);
        name_to_value_map_[alloca->get_name()] = alloca.get();
        return current_block()->insert(std::move(alloca));
    }

    template <DerivedFromInstruction T, typename... Args>
        requires std::constructible_from<T, Args...>
    IR::Instruction* create_instruction(Args&&... args)
    {
        auto inst = std::make_unique<T>(std::forward<Args>(args)...);
        inst->set_name(std::to_string(value_count_));
        update_value_count();
        return current_block()->insert(std::move(inst));
    }

    // query value graph with name {name : value*}
    IR::Value* get_value(const Sema::ReferenceExpr& ref) const
    {
        auto& name = ctx_.get_symbol(ref).identifier_;
        if (auto it = name_to_value_map_.find(name); it != name_to_value_map_.end())
            return it->second;

        return nullptr;
    }

    IR::Literal* intern_literal(auto literal)
    {
        auto [it, inserted] = intern_pool_.try_insert(literal); 
        return it->second.get();
    }

    void update_value_count()
    {
        ++value_count_;
    }
    
    void reset_value_count()
    {
        value_count_ = 0;
    }

    
};

// also explore pass manager (passing in objects that inherit from 'pass')
