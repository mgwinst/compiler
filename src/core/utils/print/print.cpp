#include <iomanip>

#include "print.hpp"
#include "../casting.hpp"
#include "../../context/context.hpp"

namespace
{
    std::unordered_map<QualifierKind, std::string> qualkind_to_str {
        {QualifierKind::Const, "const"}
    };

    std::unordered_map<RecordKind, std::string> record_kind_to_str {
        {RecordKind::Struct,  "struct"},
        {RecordKind::Enum,    "enum"},
        {RecordKind::Union,   "union"},
        {RecordKind::Unknown, "unknown"}
    };
}

// ******************** TYPE PRINTING ********************

std::string type_to_str(const ModuleContext& ctx, TypeID type_ref)
{
    const auto& type = ctx.type_pool_.get_type(type_ref);

    switch (type.get_kind()) {
        case TypeKind::Error: {
            return "ErrorType";
        }       

        case TypeKind::Void: {
            return "void";
        }

        case TypeKind::Byte: {
            return "byte";
        }

        case TypeKind::Char: {
            return "char";
        }

        case TypeKind::Bool: {
            return "bool";
        }

        case TypeKind::Integer: {
            const auto& t = type.as<IntegerType>();
            if (t.is_signed_)
                return std::format("int{}", t.bit_width_);
            return std::format("uint{}", t.bit_width_);
        }

        case TypeKind::Float: {
            const auto& t = type.as<FloatType>();
                return std::format("float{}", t.bit_width_);
        }

        case TypeKind::Reference: {
            const auto& t = type.as<ReferenceType>();
            return std::format("{}&", type_to_str(ctx, t.inner_type_));
        }

        case TypeKind::Pointer: {
            const auto& t = type.as<PointerType>();
            return std::format("{}*", type_to_str(ctx, t.inner_type_));
        }

        // FIX: print size
        case TypeKind::Array: {
            const auto& t = type.as<ArrayType>();
            return std::format("{}[]", type_to_str(ctx, t.inner_type_));
        }

        case TypeKind::Qualifier: {
            const auto& t = type.as<QualifierType>();
            return std::format("{} {}", qualkind_to_str[t.kind_], type_to_str(ctx, t.inner_type_));
        }

        case TypeKind::Function: {
            const auto& t = type.as<FunctionType>();
            return std::format("Function '{}'", t.name_);
        }

        case TypeKind::Record: {
            const auto& t = type.as<RecordType>();
            return std::format("{} {}", record_kind_to_str[t.kind_], t.name_);
        }

        default:
            std::println("{}", (int)type.get_kind());
            error_exit("type mismatch");
    }
}

// ******************** SEMANTIC NODE PRINTING ********************

std::string node_to_str(const ModuleContext& ctx, const SemaTree& tree, const SemaNodeID ref, std::string indent)
{
    const auto& node = tree.nodes_[ref];

    switch (node.get_kind()) {
        case SemaNodeKind::ModuleDecl: {
            const auto& module = node.as<Sema::ModuleDecl>();

            std::string decls_str{};

            for (auto i = 0uz; i < module.decls_.size(); i++) {
                decls_str += node_to_str(ctx, tree, module.decls_[i], indent + "  ");
                if (i != module.decls_.size() - 1)
                    decls_str += "\n\n";
            }

            return indent + std::format("ModuleDecl ({})\n{}", module.name_, decls_str);
        }

        case SemaNodeKind::VarDecl: {
            const auto& var = node.as<Sema::VarDecl>();

            const auto& symbol = ctx.symbol_table_.get_symbol(var.symbol_id_);

            if (var.init_)
                return indent + std::format("VarDecl ['{}', {}]\n{}", symbol.identifier_, type_to_str(ctx, symbol.type_id_), node_to_str(ctx, tree, *var.init_, indent + "  "));
            else
                return indent + std::format("VarDecl ['{}', {}]", symbol.identifier_, type_to_str(ctx, symbol.type_id_));
        }

        case SemaNodeKind::ParamDecl: {
            const auto& param = node.as<Sema::ParamDecl>();           

            const auto& symbol = ctx.symbol_table_.get_symbol(param.symbol_id_);

            return indent + std::format("ParamDecl ['{}', {}]", symbol.identifier_, type_to_str(ctx, symbol.type_id_));
        }

        case SemaNodeKind::FuncDecl: {
            const auto& func = node.as<Sema::FuncDecl>();

            const auto& func_symbol = ctx.symbol_table_.get_symbol(func.symbol_id_);

            auto ret_type = ctx.type_pool_.get_type(func_symbol.type_id_).as<FunctionType>().return_type_;
            auto ret_type_str = type_to_str(ctx, ret_type);

            std::string param_type_list_str{};
            std::string param_decls_str{};
            
            if (func.params_.size() > 0) {
                for (auto i = 0uz; i < func.params_.size(); i++) {
                    const auto& param = tree.nodes_[func.params_[i]].as<Sema::ParamDecl>();
                    const auto& param_symbol = ctx.symbol_table_.get_symbol(param.symbol_id_);
                    param_type_list_str += type_to_str(ctx, param_symbol.type_id_);

                    param_decls_str += node_to_str(ctx, tree, func.params_[i], indent + "  ");

                    if (i < func.params_.size() - 1) {
                        param_type_list_str += ", ";
                        param_decls_str += '\n';
                    }
                }

                return indent + std::format("FuncDecl '{}' ({}) -> ({})\n{}\n{}",
                    func_symbol.identifier_,
                    param_type_list_str,
                    ret_type_str,
                    param_decls_str,
                    node_to_str(ctx, tree, func.body_, indent + "  "));
            }

            return indent + std::format("FuncDecl '{}' () -> ({})\n{}",
                func_symbol.identifier_,
                ret_type_str,
                node_to_str(ctx, tree, func.body_, indent + "  "));
        }


        case SemaNodeKind::RecordDecl: {
            const auto& rec = node.as<Sema::RecordDecl>();
            
            std::string field_list_str{};
            for (auto i = 0uz; i < rec.fields_.size(); i++) {
                field_list_str += node_to_str(ctx, tree, rec.fields_[i], indent + "  ");
                if (i != rec.fields_.size() - 1)
                    field_list_str += '\n';
            }
            
            const auto& symbol = ctx.symbol_table_.get_symbol(rec.symbol_id_);

            return indent + std::format("RecordDecl ['{}']\n{}", type_to_str(ctx, symbol.type_id_), field_list_str);
        }

        case SemaNodeKind::CompoundStmt: {
            const auto& cstmt = node.as<Sema::CompoundStmt>();  

            if (cstmt.children_.empty())
                return indent + std::format("CompoundStmt");

            std::string expr_strs{};
            for (auto i = 0uz; i < cstmt.children_.size(); i++) {
                expr_strs += node_to_str(ctx, tree, cstmt.children_[i], indent + "  ");
                if (i != cstmt.children_.size() - 1)
                    expr_strs += '\n';
            }

            return indent + std::format("CompoundStmt\n{}", expr_strs); 
        }

        case SemaNodeKind::ReturnStmt: {
            const auto& ret_stmt = node.as<Sema::ReturnStmt>();
            return indent + std::format("ReturnStmt\n{}", node_to_str(ctx, tree, ret_stmt.value_, indent + "  "));
        }

        case SemaNodeKind::BreakStmt: {
            return indent + std::format("BreakStmt");
        }

        case SemaNodeKind::ContinueStmt: {
            return indent + std::format("ContinueStmt");
        }

        case SemaNodeKind::IfStmt: {
            const auto& if_stmt = node.as<Sema::IfStmt>();

            if (if_stmt.else_stmt_.has_value()) {
                return indent + std::format("IfStmt\n{}\n{}\n{}", 
                    node_to_str(ctx, tree, if_stmt.cond_, indent + "  "),
                    node_to_str(ctx, tree, if_stmt.then_stmt_, indent + "  "),
                    node_to_str(ctx, tree, *if_stmt.else_stmt_, indent + "  "));
            } else {
                return indent + std::format("IfStmt\n{}\n{}", 
                    node_to_str(ctx, tree, if_stmt.cond_, indent + "  "),
                    node_to_str(ctx, tree, if_stmt.then_stmt_, indent + "  "));
            }
        }

        case SemaNodeKind::WhileStmt: {
            const auto& while_stmt = node.as<Sema::WhileStmt>();

            return indent + std::format("WhileStmt\n{}\n{}", 
                node_to_str(ctx, tree, while_stmt.cond_, indent + "  "),
                node_to_str(ctx, tree, while_stmt.body_, indent + "  "));
        }

        case SemaNodeKind::ForStmt: {
            const auto& for_stmt = node.as<Sema::ForStmt>();

            return indent + std::format("ForStmt\n{}\n{}\n{}\n{}",
                node_to_str(ctx, tree, for_stmt.init_, indent + "  "),
                node_to_str(ctx, tree, for_stmt.cond_, indent + "  "),
                node_to_str(ctx, tree, for_stmt.update_, indent + "  "),
                node_to_str(ctx, tree, for_stmt.body_, indent + "  "));
        }

        case SemaNodeKind::IntegerLiteralExpr: {
            const auto& i = node.as<Sema::IntegerLiteralExpr>();
            return indent + std::format("IntLiteral ['{}']", i.value_);
        }

        case SemaNodeKind::FloatLiteralExpr: {
            const auto& f = node.as<Sema::FloatLiteralExpr>();
            return indent + std::format("FloatLiteral ['{}']", f.value_);
        }

        case SemaNodeKind::CharLiteralExpr: {
            const auto& c = node.as<Sema::CharLiteralExpr>();
            return indent + std::format("CharLiteral ['{}']", c.value_);
        }

        case SemaNodeKind::StringLiteralExpr: {
            const auto& str = node.as<Sema::StringLiteralExpr>();
            return indent + std::format("StringLiteral ['{}']", str.value_);
        }

        case SemaNodeKind::BooleanLiteralExpr: {
            const auto& b = node.as<Sema::BooleanLiteralExpr>();
            return indent + std::format("BoolLiteral ['{}']", b.value_);
        }

        case SemaNodeKind::UnaryExpr: {
            const auto& unary = node.as<Sema::UnaryExpr>();

            if (unary.is_postfix_) {
                return indent + std::format("PostfixUnaryOp ['{}']\n{}", unary.op_, node_to_str(ctx, tree, unary.operand_, indent + "  "));
            } else {
                return indent + std::format("PrefixUnaryOp ['{}']\n{}", unary.op_, node_to_str(ctx, tree, unary.operand_, indent + "  "));
            }
        }

        case SemaNodeKind::BinaryExpr: {
            const auto& binary = node.as<Sema::BinaryExpr>();  

            return indent + std::format("BinOp ['{}']\n{}\n{}",
                binary.op_,
                node_to_str(ctx, tree, binary.left_, indent + "  "),
                node_to_str(ctx, tree, binary.right_, indent + "  "));
        }

        case SemaNodeKind::ReferenceExpr: {
            const auto& r = node.as<Sema::ReferenceExpr>();

            const auto& symbol = ctx.symbol_table_.get_symbol(r.symbol_id_);

            return indent + std::format("RefExpr ['{}', {}]", symbol.identifier_, type_to_str(ctx, symbol.type_id_));
        }

        case SemaNodeKind::CallExpr: {
            const auto& call = node.as<Sema::CallExpr>();           

            std::string args_str{};
            for (auto i = 0uz; i < call.args_.size(); i++) {
                args_str += node_to_str(ctx, tree, call.args_[i], indent + "  ");
                if (i != call.args_.size() - 1)
                    args_str += '\n';
            }

            return indent + std::format("CallExpr\n{}\n{}", node_to_str(ctx, tree, call.callee_, indent + "  "), args_str);
        }

        case SemaNodeKind::MemberExpr: {
            const auto& m = node.as<Sema::MemberExpr>();           

            return indent + std::format("MemberExpr ['.{}']\n{}", 
                m.member_,
                node_to_str(ctx, tree, m.base_, indent + "  "));
        }

        case SemaNodeKind::ArraySubscriptExpr: {
            const auto& arr_expr = node.as<Sema::ArraySubscriptExpr>();

            return indent + std::format("ArraySubscriptExpr\n{}\n{}", 
                node_to_str(ctx, tree, arr_expr.base_, indent + "  "),
                node_to_str(ctx, tree, arr_expr.index_, indent + "  "));
        }

        case SemaNodeKind::InitListExpr: {
            const auto& init_list = node.as<Sema::InitListExpr>();

            std::string init_list_str{};
            for (auto i = 0uz; i < init_list.init_values_.size(); i++) {
                init_list_str += node_to_str(ctx, tree, init_list.init_values_[i], indent + "  ");
                if (i != init_list.init_values_.size() - 1) {
                    init_list_str += "\n";
                }
            }

            return indent + std::format("InitListExpr\n{}", init_list_str);
        }

        case SemaNodeKind::ExplicitCastExpr:
        case SemaNodeKind::ImplicitCastExpr: {
            const auto& cast = node.as<Sema::ImplicitCastExpr>();
            return indent + std::format("ImplicitCastExpr\n{}", (int)cast.kind_, node_to_str(ctx, tree, cast.expr_, indent + "  "));
        }

        default:
            return "parse error";
    }
}

// ******************** IR PRINTING ********************

std::string get_target_list_str(Branch* branch)
{
    std::string target_label_list{};
    for (auto target : branch->targets()) {
    target_label_list += std::format("label %{}", target->name_);
    if (target != branch->targets().back())
        target_label_list += ", ";
    }

    return target_label_list;
}

std::string PrettyPrinter::ir_type_str(TypeID type_id) const
{
    auto& type = ctx_.get_type(type_id);

    switch (type.get_kind()) {
        case TypeKind::Error: {
            return "ErrorType";
        }

        case TypeKind::Void: {
            return "void";
        }

        case TypeKind::Byte: {
            return "u8";
        }

        case TypeKind::Char: {
            return "u8";
        }

        case TypeKind::Bool: {
            return "u8";
        }

        case TypeKind::Integer: {
            const auto& t = type.as<IntegerType>();
            if (t.is_signed_)
                return std::format("i{}", t.bit_width_);
            return std::format("u{}", t.bit_width_);
        }

        case TypeKind::Float: {
            const auto& t = type.as<FloatType>();
                return std::format("f{}", t.bit_width_);
        }

        case TypeKind::Pointer: {
            return "ptr";
        }

        // FIX: print size
        case TypeKind::Array: {
            const auto& t = type.as<ArrayType>();
            return std::format("[{} x {}]", t.size_, ir_type_str(t.inner_type_));
        }

        case TypeKind::Function: {
            // just return type.
        }

        case TypeKind::Record: {
            const auto& t = type.as<RecordType>();
            return std::format("{} {}", record_kind_to_str[t.kind_], t.name_);
        }

        default:
            std::println("{}", (int)type.get_kind());
            error_exit("type mismatch");
    }
}

std::string PrettyPrinter::ir_value_to_str(Value* value) const
{
    switch (value->kind_) {
        case ValueKind::Alloca: {
            auto* inst = static_cast<Alloca*>(value);
            return std::format("%{} = alloca", inst->name_); // %x = alloca int32
        }

        case ValueKind::Load: {
            auto* inst = static_cast<Load*>(value);
            return std::format("%{} = load %{}", inst->name_, inst->operands_[0]->name_);
        }

        case ValueKind::Store: {
            auto* inst = static_cast<Store*>(value);
            return std::format("store %{}, %{}", inst->operands_[0]->name_, inst->operands_[1]->name_);
        }

        case ValueKind::Add: {
            auto* inst = static_cast<Add*>(value);
            return std::format("%{} = add %{}, %{}", inst->name_, inst->operands_[0]->name_, inst->operands_[1]->name_);
        }

        case ValueKind::Mul: {
            auto* inst = static_cast<Mul*>(value);
            return std::format("%{} = mul %{}, %{}", inst->name_, inst->operands_[0]->name_, inst->operands_[1]->name_);
        }

        case ValueKind::Eq: {
            auto* inst = static_cast<Eq*>(value);
            return std::format("%{} = eq %{}, %{}", inst->name_, inst->operands_[0]->name_, inst->operands_[1]->name_);
        }

        case ValueKind::Ne: {
            auto* inst = static_cast<Ne*>(value);
            return std::format("%{} = ne %{}, %{}", inst->name_, inst->operands_[0]->name_, inst->operands_[1]->name_);
        }

        case ValueKind::Slt: {
            auto* inst = static_cast<Slt*>(value);
            return std::format("%{} = slt %{}, %{}", inst->name_, inst->operands_[0]->name_, inst->operands_[1]->name_);
        }

        case ValueKind::Return: {
            auto* inst = static_cast<Return*>(value);
            return std::format("ret %{}", inst->operands_[0]->name_);
        }

        case ValueKind::Branch: {
            auto* inst = static_cast<Branch*>(value);
            
            return inst->is_conditional() ? 
                std::format("br %{}, label %{}, label %{}", inst->condition()->name_, inst->targets()[0]->name_, inst->targets()[1]->name_) :
                std::format("br label %{}", inst->operands_[0]->name_);
        }

        case ValueKind::PtrAdd: {
            auto* inst = static_cast<PtrAdd*>(value);
            return std::format("%{} = ptradd %{}, %{}", inst->name_, inst->operands_[0]->name_, inst->operands_[1]->name_);
        };

        default: {
            std::perror("Unknown ValueKind");
            std::terminate();
        }
    }
}

void PrettyPrinter::print(const SemaTree& tree) const
{
    std::println("{}\n", node_to_str(ctx_, tree, tree.root(), ""));
}


std::string get_arg_list_str(const std::unique_ptr<Function>& function)
{
    std::string arg_list;

    for (auto& arg : function->args_) {
        arg_list += "%" + arg->name_;
        if (arg != function->args_.back()) {
            arg_list += ", ";
        }
    }

    return arg_list;
}

std::string get_pred_list_str(const std::unique_ptr<BasicBlock>& block)
{
    std::string pred_list;

    if (block->predecessors().size() > 0) {
        pred_list = "preds: [";
        for (auto pred : block->predecessors()) {
            pred_list += "%" + pred->name_;
            if (pred != block->predecessors().back()) {
                pred_list += ", ";
            }
        }
        pred_list += ']';
    }

    return pred_list;
}

void PrettyPrinter::print(Program& program) const
{
    for (const auto& function : program.functions_) {
        std::print("define @{}({}) -> () ", function->name_, get_arg_list_str(function));
        std::cout << "{\n";
        for (const auto& block : function->blocks_) {
            std::println("{:<30}{}", block->name_ + ":", get_pred_list_str(block));
            for (const auto& inst : block->instructions_) {
                std::println("  {}", ir_value_to_str(inst.get()));
            }
            if (block != function->blocks_.back()) {
                std::println();
            }
        }
        std::cout << "}\n\n";
    }
}