#include <algorithm>

#include "analyzer.hpp"
#include "frontend/sema/types/type_table.hpp"
#include "utils/casting.hpp"
#include "utils/print/print.hpp"
#include "helpers.hpp"

using namespace Sema;

SemaTree SemanticAnalyzer::run()
{
    build_sema_node(ast_.root_);
    ctx_.diagnostics_.report();

    check_type(sema_tree_.root_);
    ctx_.diagnostics_.report();

    desugar(sema_tree_.root_);

    return std::move(sema_tree_);
}

SemaNode* SemanticAnalyzer::build_sema_node(Syntax::ASTNode* node)
{
    switch (node->kind_) {
        case ASTNodeKind::ModuleDecl: {
            auto* ast_module = cast<Syntax::ModuleDecl>(node);

            auto* sema_module = sema_tree_.emplace<ModuleDecl>(ast_module->name_);

            ctx_.symbol_table_.enter_scope();

            for (auto* decl : ast_module->decls_) {
                sema_module->decls_.push_back(cast<Decl>(build_sema_node(decl)));
            }

            ctx_.symbol_table_.exit_scope();

            sema_tree_.root_ = sema_module;

            return nullptr;
        }

        case ASTNodeKind::VarDecl: {
            auto* var = cast<Syntax::VarDecl>(node);

            auto* type = ctx_.type_table_.resolve_type(var->type_expr_);

            if (ctx_.symbol_table_.exists_in_scope(var->name_)) {
                ctx_.diagnostics_.register_error(std::format("redefinition of '{}'", var->name_), var->source_);
            }
            
            auto* symbol = ctx_.symbol_table_.insert(var, type);

            if (var->init_) {
                auto* init = build_sema_node(var->init_);
                return sema_tree_.emplace<VarDecl>(symbol, cast<Expr>(init), var->source_);
            }

            return sema_tree_.emplace<VarDecl>(symbol, nullptr, var->source_);
        }

        case ASTNodeKind::ParamDecl: {
            auto* param = cast<Syntax::ParamDecl>(node);
            
            auto* type = ctx_.type_table_.resolve_type(param->type_expr_);

            if (ctx_.symbol_table_.exists_in_scope(param->name_)) {
                ctx_.diagnostics_.register_error(std::format("redefinition of parameter '{}'", param->name_), param->source_);
            }

            auto* symbol = ctx_.symbol_table_.insert(param, type);

            return sema_tree_.emplace<ParamDecl>(symbol, param->source_);
        }

        case ASTNodeKind::FuncDecl: {
            auto* func = cast<Syntax::FuncDecl>(node);

            auto* type = cast<FunctionType>(ctx_.type_table_.resolve_type(func));

            // handle overload resolution
            auto* existing_func = ctx_.symbol_table_.lookup(func->name_);

            if (existing_func) {

                auto* existing_type = cast<FunctionType>(existing_func->type_);

                if (type == existing_type) {
                    ctx_.diagnostics_.register_error(std::format("redefinition of '{}'", func->name_), func->source_);
                } else if (type->params_ == existing_type->params_ && type->return_type_ != existing_type->return_type_) {
                    ctx_.diagnostics_.register_error(std::format("ambiguous redefinition of '{}' | prototype differs only in return type", func->name_), func->source_);
                }
            }

            auto* symbol = ctx_.symbol_table_.insert(func, type);

            ctx_.symbol_table_.enter_scope();

            std::vector<SemaNode*> params;
            for (auto* p : func->params_) {
                params.push_back(cast<Decl>(build_sema_node(p)));
            }

            auto* body = build_sema_node(func->body_);

            ctx_.symbol_table_.exit_scope();

            return sema_tree_.emplace<FuncDecl>(symbol, std::move(params), cast<Stmt>(body), func->source_);
        }

        case ASTNodeKind::RecordDecl: {
            auto* record = cast<Syntax::RecordDecl>(node);

            auto* type = ctx_.type_table_.resolve_type(record);

            if (ctx_.symbol_table_.exists_in_scope(record->name_)) {
                ctx_.diagnostics_.register_error(std::format("redefinition of '{}'", record->name_), record->source_);
            }

            auto* symbol = ctx_.symbol_table_.insert(record, type);

            ctx_.symbol_table_.enter_scope();               

            std::vector<SemaNode*> fields;
            for (auto* field : record->fields_) {
                fields.push_back(cast<Decl>(build_sema_node(field)));
            }

            ctx_.symbol_table_.exit_scope();

            return sema_tree_.emplace<RecordDecl>(record->kind_, symbol, std::move(fields), record->source_);
        }

        case ASTNodeKind::CompoundStmt: {
            auto* cmpd_stmt = cast<Syntax::CompoundStmt>(node);

            std::vector<SemaNode*> children;
            for (auto* c : cmpd_stmt->children_)
                children.push_back(build_sema_node(c));

            return sema_tree_.emplace<CompoundStmt>(std::move(children));
        }

        case ASTNodeKind::ReturnStmt: {
            auto* ret = cast<Syntax::ReturnStmt>(node);
            return sema_tree_.emplace<ReturnStmt>(cast<Expr>(build_sema_node(ret->value_)));
        }

        case ASTNodeKind::BreakStmt: {
            return sema_tree_.emplace<BreakStmt>();
        }

        case ASTNodeKind::ContinueStmt: {
            return sema_tree_.emplace<ContinueStmt>();
        }

        // fix hacky empty else case -1
        case ASTNodeKind::IfStmt: {
            auto* if_stmt = cast<Syntax::IfStmt>(node);

            ctx_.symbol_table_.enter_scope();
            
            auto* cond = build_sema_node(if_stmt->cond_);
            auto* then_stmt = build_sema_node(if_stmt->then_stmt_);

            auto* else_stmt = if_stmt->else_stmt_ ? build_sema_node(if_stmt->else_stmt_) : nullptr;

            ctx_.symbol_table_.exit_scope();

            if (else_stmt)
                return sema_tree_.emplace<IfStmt>(cast<Expr>(cond), cast<Stmt>(then_stmt), cast<Stmt>(else_stmt));

            return sema_tree_.emplace<IfStmt>(cast<Expr>(cond), cast<Stmt>(then_stmt));
            
        }

        case ASTNodeKind::WhileStmt: {
            auto* while_stmt = cast<Syntax::WhileStmt>(node);
            
            ctx_.symbol_table_.enter_scope();

            auto* cond = build_sema_node(while_stmt->cond_);
            auto* body = build_sema_node(while_stmt->body_);

            ctx_.symbol_table_.exit_scope();
            
            return sema_tree_.emplace<WhileStmt>(cast<Expr>(cond), cast<Stmt>(body));
        }

        case ASTNodeKind::ForStmt: {
            auto* for_stmt = cast<Syntax::ForStmt>(node);
            
            ctx_.symbol_table_.enter_scope();

            auto* init   = build_sema_node(for_stmt->init_);
            auto* cond   = build_sema_node(for_stmt->cond_);
            auto* update = build_sema_node(for_stmt->update_);
            auto* body   = build_sema_node(for_stmt->body_);

            ctx_.symbol_table_.exit_scope();
            
            return sema_tree_.emplace<ForStmt>(cast<Decl>(init), cast<Expr>(cond), cast<Expr>(update), cast<Stmt>(body));
        }

        case ASTNodeKind::IntegerLiteralExpr: {
            auto* int_lit = cast<Syntax::IntegerLiteralExpr>(node);
            return sema_tree_.emplace<IntegerLiteralExpr>(int_lit->value_);
        }

        case ASTNodeKind::FloatLiteralExpr: {
            auto* float_lit = cast<Syntax::FloatLiteralExpr>(node);
            return sema_tree_.emplace<FloatLiteralExpr>(float_lit->value_);
        }

        case ASTNodeKind::CharLiteralExpr: {
            auto* char_lit = cast<Syntax::CharLiteralExpr>(node);
            return sema_tree_.emplace<CharLiteralExpr>(char_lit->value_);
        }

        case ASTNodeKind::StringLiteralExpr: {
            auto* str_lit = cast<Syntax::StringLiteralExpr>(node);
            return sema_tree_.emplace<StringLiteralExpr>(str_lit->value_);
        }

        case ASTNodeKind::BooleanLiteralExpr: {
            auto* bool_lit = cast<Syntax::BooleanLiteralExpr>(node);
            return sema_tree_.emplace<BooleanLiteralExpr>(bool_lit->value_);
        }

        case ASTNodeKind::UnaryExpr: {
            auto* unary = cast<Syntax::UnaryExpr>(node);
            return sema_tree_.emplace<UnaryExpr>(std::move(unary->op_), cast<Expr>(build_sema_node(unary->operand_)), unary->is_postfix_, unary->source_);
        }

        case ASTNodeKind::BinaryExpr: {
            auto* binary = cast<Syntax::BinaryExpr>(node);

            auto* left = build_sema_node(binary->left_);
            auto* right = build_sema_node(binary->right_);

            return sema_tree_.emplace<BinaryExpr>(std::move(binary->op_), cast<Expr>(left), cast<Expr>(right), binary->source_);
        }

        case ASTNodeKind::ReferenceExpr: {
            auto* ref = cast<Syntax::ReferenceExpr>(node);

            auto* symbol = ctx_.symbol_table_.lookup(ref->name_);
            
            if (!symbol) {
                ctx_.diagnostics_.register_error(std::format("use of undeclared identifier '{}'", ref->name_), ref->source_);

                // invalid reference state -> invalid symbol
                return sema_tree_.emplace<ReferenceExpr>(nullptr, ref->name_, ref->source_);
            }

            return sema_tree_.emplace<ReferenceExpr>(symbol, symbol->identifier_, ref->source_);
        }

        case ASTNodeKind::CallExpr: {
            auto* call = cast<Syntax::CallExpr>(node);

            auto* callee = build_sema_node(call->callee_);
            
            std::vector<SemaNode*> args;
            for (auto* arg : call->args_)
                args.push_back(cast<Expr>(build_sema_node(arg)));
            
            return sema_tree_.emplace<CallExpr>(cast<Expr>(callee), std::move(args), call->source_);
        }

        case ASTNodeKind::MemberExpr: {
            auto* expr = cast<Syntax::MemberExpr>(node);
            auto* base = build_sema_node(expr->base_);

            return sema_tree_.emplace<MemberExpr>(cast<Expr>(base), std::move(expr->member_), expr->is_arrow_, expr->source_);
        }

        case ASTNodeKind::ArraySubscriptExpr: {
            auto* arr_expr = cast<Syntax::ArraySubscriptExpr>(node);

            auto* base = build_sema_node(arr_expr->base_);
            auto* index = build_sema_node(arr_expr->index_);

            return sema_tree_.emplace<ArraySubscriptExpr>(cast<Expr>(base), cast<Expr>(index), arr_expr->source_);
        }

        case ASTNodeKind::InitListExpr: {
            auto* init_list = cast<Syntax::InitListExpr>(node);

            std::vector<SemaNode*> values;
            for (auto* v : init_list->init_values_)
                values.push_back(cast<Expr>(build_sema_node(v)));

            return sema_tree_.emplace<InitListExpr>(std::move(values));
        }

        default:
            error_exit("build_sema_node()");
    }
}

Type* SemanticAnalyzer::check_type(SemaNode* node)
{
    switch (node->kind_) {
        case SemaNodeKind::ModuleDecl: {
            auto* module = cast<ModuleDecl>(node);

            for (auto* decl : module->decls_)
                check_type(decl);

            return nullptr;
        }

        case SemaNodeKind::VarDecl: {
            auto* var = cast<VarDecl>(node);
            
            if (var->init_) {
                auto* init_type = check_type(var->init_);

                if (init_type == nullptr)
                    return nullptr;

                if (var->type() != init_type) {
                    ctx_.diagnostics_.register_error(std::format("type mismatch ({} and {})", type_to_str(var->type()), type_to_str(init_type)), var->source_);
                    return nullptr;
                }

                return init_type;
            }

            return nullptr;
        }

        // make sure return type and return statment type match
        case SemaNodeKind::FuncDecl: {
            auto* func = cast<FuncDecl>(node);

            auto* return_type = cast<FunctionType>(func->type())->return_type_;

            check_type(func->body_);

            return nullptr;
        }

        case SemaNodeKind::CompoundStmt: {
            auto* compound = cast<CompoundStmt>(node);

            for (auto* c : compound->children_)
                check_type(c);

            return nullptr;
        }

        case SemaNodeKind::ReturnStmt: {
            auto* return_stmt = cast<ReturnStmt>(node);
            auto* return_type = check_type(return_stmt->value_);
            
            if (return_type == nullptr)
                return nullptr;

            return return_type;
        }

        case SemaNodeKind::IfStmt: {
            auto* if_stmt = cast<IfStmt>(node);
            
            auto* cond_type = check_type(if_stmt->cond_);
            if (cond_type == nullptr) 
                return nullptr;

            if (!convertible_to_boolean(cond_type)) {
                ctx_.diagnostics_.register_error(std::format("condition expression must be of boolean or scalar type"), if_stmt->cond_->source_);
                return nullptr;
            }

            check_type(if_stmt->then_stmt_);
            
            if (if_stmt->else_stmt_) {
                check_type(if_stmt->else_stmt_);
            }

            return nullptr;
        }

        case SemaNodeKind::WhileStmt: {
            auto* while_stmt = cast<WhileStmt>(node);

            auto* cond_type = check_type(while_stmt->cond_);
            if (cond_type == nullptr)
                return nullptr;
            
            if (!convertible_to_boolean(cond_type)) {
                ctx_.diagnostics_.register_error(std::format("condition expression must be of boolean or scalar type"), while_stmt->cond_->source_);
                return nullptr;
            }

            return nullptr;
        }

        case SemaNodeKind::IntegerLiteralExpr: {
            return ctx_.type_table_.builtin_map_["int"];
        }

        case SemaNodeKind::FloatLiteralExpr: {
            return ctx_.type_table_.builtin_map_["float"];
        }

        case SemaNodeKind::CharLiteralExpr: {
            return ctx_.type_table_.builtin_map_["char"];
        }

        case SemaNodeKind::BooleanLiteralExpr: {
            return ctx_.type_table_.builtin_map_["bool"];
        }

        case SemaNodeKind::UnaryExpr: {
            auto* unary = cast<UnaryExpr>(node);
            
            auto* operand_type = check_type(unary->operand_);
            if (operand_type == nullptr) 
                return nullptr;

            if (unary->op_ == "&") {
                return ctx_.type_table_.get_or_create<PointerType>(operand_type);
            } else if (unary->op_ == "*") {
                if (!isa<PointerType>(operand_type)) {
                    ctx_.diagnostics_.register_error(std::format("dereferencing non-pointer type {}", type_to_str(operand_type)), unary->source_);
                    return nullptr;
                }
                // dereference -> return underlying type
                return cast<PointerType>(operand_type)->inner_type_;
            } else if (unary->op_ == "!") {
                if (!convertible_to_boolean(operand_type)) {
                    ctx_.diagnostics_.register_error(std::format("invalid argument type '{}' to unary expression '!'", type_to_str(operand_type)), unary->source_);
                    return nullptr;
                }
                return ctx_.type_table_.builtin_map_["bool"];
            } else if (unary->op_ == "~") {
                if (!isa<IntegerType>(operand_type)) {
                    ctx_.diagnostics_.register_error(std::format("non-integral type '{}' to unary expression '~'", type_to_str(operand_type)), unary->source_);
                    return nullptr;
                }
            } else if (unary->op_ == "++" || unary->op_ == "--") {
                if (!is_scalar(operand_type) || is_const(operand_type)) {
                    ctx_.diagnostics_.register_error(std::format("can't '{}' value of type ({})", unary->op_, type_to_str(operand_type)), unary->source_);
                    return nullptr;
                }
            }

            return operand_type;
        }

        // FIX BIN OP RETURN TYPES (probably need a table for pattern matching...)
        case SemaNodeKind::BinaryExpr: {
            auto* binary = cast<BinaryExpr>(node);

            auto* left_type = check_type(binary->left_);
            auto* right_type = check_type(binary->right_);
            
            if (left_type == nullptr || right_type == nullptr)
                return nullptr;

            if (left_type != right_type) {
                ctx_.diagnostics_.register_error(std::format("type mismatch ({} and {})", type_to_str(left_type), type_to_str(right_type)), binary->source_);
                return nullptr;
            }

            /*
            if (is_bitwise_op(binary.op_)) {
                if (!is_integral(*left_type) || !is_integral(*right_type)) {
                    auto err = TypeError{std::format("bitwise ops require integral type operands", type_to_str(ctx_, *left_type), type_to_str(ctx_, *right_type)), binary.source_loc_};
                    ctx_.diagnostics_.register_error(err);
                    return ERROR_TYPE;
                }
            } else if (is_logical_op(binary.op_)) {
                return BOOL;
            } else if (is_relational_op(binary.op_)) {
                
            }
            */

            return left_type;
        }

        case SemaNodeKind::ReferenceExpr: {
            auto* ref = cast<ReferenceExpr>(node);
            return ref->type();
        }

        case SemaNodeKind::CallExpr: {
            auto* call = cast<CallExpr>(node);

            auto* func_type = cast<FunctionType>((cast<ReferenceExpr>(call->callee_)->type()));

            if (func_type->params_.size() != call->args_.size()) {
                ctx_.diagnostics_.register_error(std::format("expected '{}' arguments in call expression", func_type->params_.size()), call->source_);
                return nullptr;
            }

            for (auto [param_type, arg] : std::views::zip(func_type->params_, call->args_)) {
                auto* arg_type = check_type(arg);
                if (arg_type == nullptr)
                    return nullptr;

                if (param_type != arg_type) {
                    ctx_.diagnostics_.register_error(std::format("passing '{}' to parameter of incompatible type '{}'", type_to_str(arg_type), type_to_str(param_type)), call->source_);
                    return nullptr;
                }
            }

            return func_type->return_type_;
        }

        case SemaNodeKind::MemberExpr: {
            auto* expr = cast<MemberExpr>(node);

            auto* record_type = cast<RecordType>((cast<ReferenceExpr>(expr->base_)->type()));

            auto* field = record_type->lookup_field(expr->member_);
            if (field == nullptr) {
                ctx_.diagnostics_.register_error(std::format("no member named '{}' in '{}'", expr->member_, type_to_str(record_type)), expr->source_);
                return nullptr;
            }

            return field->type_;
        }

        case SemaNodeKind::ArraySubscriptExpr: {
            auto* expr = cast<ArraySubscriptExpr>(node);           
            auto* elem_type = cast<ArrayType>(cast<ReferenceExpr>(expr->base_)->type())->inner_type_;

            return elem_type;
        }

        case SemaNodeKind::InitListExpr: {
            return nullptr;
        }

        default:
            return nullptr;
    }
}





























SemaNode* for_to_while(SemaTree& tree, ForStmt* for_stmt)
{
    cast<CompoundStmt>(for_stmt->body_)->children_.push_back(for_stmt->update_);
    
    auto* while_stmt = tree.emplace<WhileStmt>(for_stmt->cond_, for_stmt->body_);

    std::vector<SemaNode*> children;
    children.push_back(for_stmt->init_);
    children.push_back(while_stmt);

    return tree.emplace<CompoundStmt>(std::move(children));
}

bool is_compound_assign(std::string_view op)
{
    static const std::unordered_set<std::string_view> compound_ops { "+=", "-=", "*=", "/=", "%=" };
    return compound_ops.contains(op);
}

SemaNode* expand_compound_assignment(SemaTree& tree, BinaryExpr* expr)
{
    auto op = expr->op_[0];
    auto* right = tree.emplace<BinaryExpr>(std::string{ op }, expr->left_, expr->right_);
    return tree.emplace<BinaryExpr>("=", expr->left_, right);
}

SemaNode* arr_idx_to_ptr_arithmetic(SemaTree& tree, ArraySubscriptExpr* expr) 
{
    auto* bin_op = tree.emplace<BinaryExpr>("+", expr->base_, expr->index_);
    return tree.emplace<UnaryExpr>("*", bin_op, false);
}











// PASSING POINTERS BY VALUE MAKES MUTATION OF THE TREE INCORRECT

void SemanticAnalyzer::desugar(SemaNode*& node)
{
    switch (node->kind_) {
        case SemaNodeKind::ModuleDecl: {
            auto* module = cast<ModuleDecl>(node);
            for (auto*& decl : module->decls_)
                desugar(decl);
            break;
        }

        case SemaNodeKind::FuncDecl: {
            auto* func = cast<FuncDecl>(node);
            desugar(func->body_);
            break;
        }

        case SemaNodeKind::CompoundStmt: {
            auto* cmpd_stmt = cast<CompoundStmt>(node);
            for (auto*& c : cmpd_stmt->children_)
                desugar(c);
            break;
        }

        case SemaNodeKind::ReturnStmt: {
            auto* return_stmt = cast<ReturnStmt>(node);
            desugar(return_stmt->value_);
            break;
        }

        case SemaNodeKind::IfStmt: {
            auto* if_stmt = cast<IfStmt>(node);
            desugar(if_stmt->cond_);
            desugar(if_stmt->then_stmt_);
            if (if_stmt->else_stmt_)
                desugar(if_stmt->else_stmt_);
            break;
        }

        case SemaNodeKind::WhileStmt: {
            auto* while_stmt = cast<WhileStmt>(node);
            desugar(while_stmt->cond_);
            desugar(while_stmt->body_);
            break;
        }

        // for -> while
        case SemaNodeKind::ForStmt: {
            auto* for_stmt = cast<ForStmt>(node);

            desugar(for_stmt->body_);

            node = for_to_while(sema_tree_, for_stmt);

            break;
        }
        
        // ++a -> a = a + 1
        case SemaNodeKind::UnaryExpr: {
            auto* unary = cast<UnaryExpr>(node);

            if (unary->op_ == "++" || unary->op_ == "--") {
                auto op = unary->op_[0];

                auto* one = sema_tree_.emplace<IntegerLiteralExpr>(1);
                auto* right = sema_tree_.emplace<BinaryExpr>(std::string{ op }, unary->operand_, one);
                auto* expanded = sema_tree_.emplace<BinaryExpr>("=", unary->operand_, right);

                node = expanded;
            }
                
            break;
        }

        // a += b -> a = a + b
        // a > b -> b < a
        case SemaNodeKind::BinaryExpr: {
            auto* binary = cast<BinaryExpr>(node);

            desugar(binary->left_);
            desugar(binary->right_);

            if (is_compound_assign(binary->op_)) {
                node = expand_compound_assignment(sema_tree_, binary);
            } else if (binary->op_ == ">") {
                binary->op_ = "<";
                std::swap(binary->left_, binary->right_);
            }

            break;
        }

        // a[i] -> *(a + i)
        case SemaNodeKind::ArraySubscriptExpr: {
            auto* expr = cast<ArraySubscriptExpr>(node);

            desugar(expr->index_);

            node = arr_idx_to_ptr_arithmetic(sema_tree_, expr);

            break;
        }

        case SemaNodeKind::InitListExpr: {
            auto* init_list = cast<InitListExpr>(node);
            for (auto*& v : init_list->init_values_)
                desugar(v);
            break;
        }

        default:
            break;
    }
}