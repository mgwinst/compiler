#include "ir.hpp"
#include "../context/context.hpp"
#include "../frontend/sema/sematree.hpp"
#include "../utils/alias.hpp"

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
    

    std::unordered_map<std::string, IR::Value*> name_to_value_map_; // scope aware? perhaps keep scopes around in symbol table from sema?
    



    IR::Value* lower(SemaNodeID node_id);

    IR::Function* current_function() const { return current_function_; } 
    IR::BasicBlock* current_block() const { return current_basic_block_; } 


    // inserting into basic block should keep track of numbering values
    template <DerivedFromValue T>
    IR::Value* insert(T* value)
    {
        // if value then give unique name before inserting (update name -> value* map)
        return current_block()->insert(value);
    }

    IR::Function* create_function();
    IR::BasicBlock* create_basic_block();
    IR::Value* create_argument(SemaNodeID node_id);
    IR::Value* create_alloca(TypeID type, const std::string& name);
    IR::Value* create_store(IR::Value* ptr, IR::Value* value);
    IR::Value* create_load(IR::Value* ptr);
    IR::Value* create_add(IR::Value* src1, IR::Value* src2);
    IR::Value* create_mul(IR::Value* src1, IR::Value* src2);
    IR::Value* create_ret(IR::Value* value);

    void set_current_function(IR::Function* function);
    void set_current_block(IR::BasicBlock* block);


    // query value graph with name {name : value*}
    IR::Value* get_value(const Sema::ReferenceExpr& ref) const
    {
        auto& name = ctx_.symbol_table_.get_symbol(ref.target_symbol_).identifier_;
        if (auto it = name_to_value_map_.find(name); it != name_to_value_map_.end())
            return it->second;

        return nullptr;
    }
};

// also explore pass manager (passing in objects that inherit from 'pass')
