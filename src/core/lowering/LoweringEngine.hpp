#include "../context/context.hpp"
#include "../frontend/sema/sematree.hpp"
#include "../ir/Value.hpp"
#include "../utils/alias.hpp"
#include "../utils/casting.hpp"

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
    uint32_t value_count_ = 0u; // reset per new function

    IR::Value* lower(SemaNodeID node_id);

    IR::Function* current_function() const { return current_function_; } 
    IR::BasicBlock* current_block() const { return current_basic_block_; } 
    void set_current_function(IR::Function* function) { current_function_ = function; }
    void set_current_block(IR::BasicBlock* block) { current_basic_block_ = block; }

    template <DerivedFromValue T>
    IR::Value* insert(std::unique_ptr<T> value)
    {
        auto* ptr = value.get();

        if constexpr (std::same_as<T, IR::Function>) {
            set_current_function(value.get());
            reset_value_count();
            auto* bb = create<IR::BasicBlock>("entry");
            program_.insert(std::move(value));
            return ptr;                  
        } else if constexpr (std::same_as<T, IR::BasicBlock>) {
            set_current_block(value.get());
            current_function()->add_block(std::move(value));
            return ptr;
        } else if constexpr (std::same_as<T, IR::Argument>) {
            current_function()->add_argument(std::move(value));
            return ptr;
        } else if constexpr (std::same_as<T, IR::AllocaInst>) {
            name_to_value_map_[value->get_name()] = value.get();
            return current_block()->insert(std::move(value));
        } else {
            value->set_name(std::to_string(value_count_));
            update_value_count();
            return current_block()->insert(std::move(value));
        }
    }

    template <std::same_as<IR::Function> T>
    IR::Value* create(const Sema::FuncDecl& func)
    {
        auto& func_name = ctx_.get_symbol(func).identifier_;
        auto function = std::make_unique<T>();
        function->set_name(func_name);

        return insert(std::move(function));
    }

    template <std::same_as<IR::Argument> T>
    IR::Value* create(SemaNodeID node_id)
    {
        auto arg_name = ctx_.symbol_table_.get_symbol(node_id).identifier_;
        auto arg = std::make_unique<T>();
        arg->set_name(arg_name);

        return insert(std::move(arg));
    }

    template <std::same_as<IR::AllocaInst> T>
    IR::Value* create(const ContainsSymbol auto& node)
    {
        auto& symbol = ctx_.get_symbol(node);
        return insert(std::make_unique<IR::AllocaInst>(symbol.type_id_, symbol.identifier_));
    }

    template <DerivedFromValue T, typename... Args>
        requires std::constructible_from<T, Args...>
    IR::Value* create(Args&&... args)
    {
        return insert(std::make_unique<T>(std::forward<Args>(args)...));
    }

    // query value graph with name {name : value*}
    IR::Value* get_value(const Sema::ReferenceExpr& ref) const
    {
        auto& name = ctx_.get_symbol(ref).identifier_;
        if (auto it = name_to_value_map_.find(name); it != name_to_value_map_.end())
            return it->second;

        return nullptr;
    }

    void restore_basic_block(BasicBlock* block)
    {
        set_current_block(block);
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
