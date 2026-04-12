#include "sema_tree_builder.hpp"

bool exists(std::integral auto x) 
{
    return x != -1;    
}

using namespace Sema;

SemaTree SemaTreeBuilder::run()
{
    build_sema_node(ast_.root());
    return std::move(sema_tree_);
}

SemaNodeID SemaTreeBuilder::build_sema_node(ASTNodeID ast_node_id)
{
    const auto& ast_node = ast_.nodes_[ast_node_id];

    switch (ast_node.get_kind()) {
        case ASTNodeKind::ModuleDecl: {
            SemaNodeID sema_root = sema_tree_.emplace<ModuleDecl>(std::string{ "source file" });

            ctx_.symbol_table_.enter_scope();

            const auto& module = ast_node.as<Syntax::ModuleDecl>();
            for (auto decl : module.decls_) {
                auto sema_node = build_sema_node(decl);
                sema_tree_.nodes_[sema_root].as<ModuleDecl>().decls_.push_back(sema_node);
            }

            ctx_.symbol_table_.exit_scope();

            return sema_root;
        }

        case ASTNodeKind::VarDecl: {
            const auto& var = ast_node.as<Syntax::VarDecl>();

            auto var_type_id = ctx_.type_pool_.resolve_type(var.type_expr_, ast_);

            if (ctx_.symbol_table_.exists_in_scope(var.name_)) {
                const auto err = RedefinitionError{std::format("redefinition of '{}'", var.name_), var.source_loc_};
                ctx_.diagnostics_.register_error(err);
            }
            
            auto symbol = ctx_.symbol_table_.insert(var, var_type_id);

            if (var.init_) {
                auto init = build_sema_node(*var.init_);
                return sema_tree_.emplace<VarDecl>(symbol, var_type_id, init, var.source_loc_);
            }

            return sema_tree_.emplace<VarDecl>(symbol, var_type_id, std::nullopt, var.source_loc_);
        }

        case ASTNodeKind::ParamDecl: {
            const auto& param = ast_node.as<Syntax::ParamDecl>();
            
            auto param_type_id = ctx_.type_pool_.resolve_type(param.type_expr_, ast_);

            if (ctx_.symbol_table_.exists_in_scope(param.name_)) {
                const auto err = RedefinitionError{std::format("redefinition of parameter '{}'", param.name_), param.source_loc_};
                ctx_.diagnostics_.register_error(err);
            }

            auto symbol = ctx_.symbol_table_.insert(param, param_type_id);
            
            return sema_tree_.emplace<ParamDecl>(symbol, param_type_id, param.source_loc_);
        }


        case ASTNodeKind::FuncDecl: {
            const auto& func = ast_node.as<Syntax::FuncDecl>();

            auto func_type_id = ctx_.type_pool_.resolve_type(ast_node_id, ast_);
            auto& func_type = ctx_.type_pool_.get_type(func_type_id).as<FunctionType>();
            
            // handle overload resolution
            auto func_symbol_id = ctx_.symbol_table_.lookup(func.name_);
            if (exists(func_symbol_id)) {
                auto& existing_func_symbol = ctx_.symbol_table_.get_symbol(func_symbol_id);
                auto& existing_func_type = ctx_.type_pool_.get_type(existing_func_symbol.type_id_).as<FunctionType>();

                if (func_type_id == existing_func_symbol.type_id_) {
                    const auto err = RedefinitionError{std::format("redefinition of '{}'", func.name_), func.source_loc_};
                    ctx_.diagnostics_.register_error(err);
                } else if (func_type.params_ == existing_func_type.params_ && func_type.return_type_ != existing_func_type.return_type_) {
                    const auto err = RedefinitionError{std::format("ambiguous redefinition of '{}' | prototype differs only in return type", func.name_), func.source_loc_};
                    ctx_.diagnostics_.register_error(err);
                }
            }

            auto func_symbol = ctx_.symbol_table_.insert(func, func_type_id);

            ctx_.symbol_table_.enter_scope();

            std::vector<SemaNodeID> params;
            for (auto p : func.params_)
                params.push_back(build_sema_node(p));

            auto body = build_sema_node(func.body_);

            ctx_.symbol_table_.exit_scope();

            return sema_tree_.emplace<FuncDecl>(func_symbol, func_type_id, std::move(params), body, func.source_loc_);
        }

        case ASTNodeKind::RecordDecl: {
            const auto& rec = ast_node.as<Syntax::RecordDecl>();

            auto rec_type_id = ctx_.type_pool_.resolve_type(ast_node_id, ast_); // 'unknown <T>'

            if (ctx_.symbol_table_.exists_in_scope(rec.name_)) {
                const auto err = RedefinitionError{std::format("redefinition of '{}'", rec.name_), rec.source_loc_};
                ctx_.diagnostics_.register_error(err);
            }

            auto rec_symbol = ctx_.symbol_table_.insert(rec, rec_type_id);

            ctx_.symbol_table_.enter_scope();               

            std::vector<SemaNodeID> fields;
            for (auto p : rec.fields_)
                fields.push_back(build_sema_node(p));

            ctx_.symbol_table_.exit_scope();

            return sema_tree_.emplace<RecordDecl>(rec_symbol, rec_type_id, std::move(fields), rec.source_loc_);
        }

        case ASTNodeKind::CompoundStmt: {
            const auto& cmpd_stmt = ast_node.as<Syntax::CompoundStmt>();
            
            std::vector<SemaNodeID> children;
            for (auto c : cmpd_stmt.children_)
                children.push_back(build_sema_node(c));

            return sema_tree_.emplace<CompoundStmt>(std::move(children));
        }

        case ASTNodeKind::ReturnStmt: {
            const auto& ret = ast_node.as<Syntax::ReturnStmt>();
            return sema_tree_.emplace<ReturnStmt>(build_sema_node(ret.value_));
        }

        case ASTNodeKind::BreakStmt: {
            return sema_tree_.emplace<BreakStmt>();
        }

        case ASTNodeKind::ContinueStmt: {
            return sema_tree_.emplace<ContinueStmt>();
        }

        // fix hacky empty else case -1
        case ASTNodeKind::IfStmt: {
            const auto& if_stmt = ast_node.as<Syntax::IfStmt>();

            ctx_.symbol_table_.enter_scope();
            
            auto cond = build_sema_node(if_stmt.cond_);
            auto then_stmt = build_sema_node(if_stmt.then_stmt_);

            auto else_stmt = if_stmt.else_stmt_ ? build_sema_node(*if_stmt.else_stmt_) : -1;

            ctx_.symbol_table_.exit_scope();

            if (else_stmt == -1)
                return sema_tree_.emplace<IfStmt>(cond, then_stmt);
            else
                return sema_tree_.emplace<IfStmt>(cond, then_stmt, else_stmt);
        }

        case ASTNodeKind::WhileStmt: {
            const auto& while_stmt = ast_node.as<Syntax::WhileStmt>();
            
            ctx_.symbol_table_.enter_scope();

            auto cond = build_sema_node(while_stmt.cond_);
            auto body = build_sema_node(while_stmt.body_);

            ctx_.symbol_table_.exit_scope();
            
            return sema_tree_.emplace<WhileStmt>(cond, body);
        }

        case ASTNodeKind::ForStmt: {
            const auto& for_stmt = ast_node.as<Syntax::ForStmt>();
            
            ctx_.symbol_table_.enter_scope();

            auto init   = build_sema_node(for_stmt.init_);
            auto cond   = build_sema_node(for_stmt.cond_);
            auto update = build_sema_node(for_stmt.update_);
            auto body   = build_sema_node(for_stmt.body_);

            ctx_.symbol_table_.exit_scope();
            
            return sema_tree_.emplace<ForStmt>(init, cond, update, body);
        }

        case ASTNodeKind::IntegerLiteralExpr: {
            const auto& int_lit = ast_node.as<Syntax::IntegerLiteralExpr>();
            return sema_tree_.emplace<IntegerLiteralExpr>(int_lit.value_);
        }

        case ASTNodeKind::FloatLiteralExpr: {
            const auto& float_lit = ast_node.as<Syntax::FloatLiteralExpr>();
            return sema_tree_.emplace<FloatLiteralExpr>(float_lit.value_);
        }

        case ASTNodeKind::CharLiteralExpr: {
            const auto& char_lit = ast_node.as<Syntax::CharLiteralExpr>();
            return sema_tree_.emplace<CharLiteralExpr>(char_lit.value_);
        }

        case ASTNodeKind::StringLiteralExpr: {
            const auto& str_lit = ast_node.as<Syntax::StringLiteralExpr>();
            return sema_tree_.emplace<StringLiteralExpr>(str_lit.value_);
        }

        case ASTNodeKind::BooleanLiteralExpr: {
            const auto& bool_lit = ast_node.as<Syntax::BooleanLiteralExpr>();
            return sema_tree_.emplace<BooleanLiteralExpr>(bool_lit.value_);
        }

        case ASTNodeKind::UnaryExpr: {
            const auto& unary = ast_node.as<Syntax::UnaryExpr>();
            return sema_tree_.emplace<UnaryExpr>(std::move(unary.op_), build_sema_node(unary.operand_), unary.is_postfix_, unary.source_loc_);
        }

        case ASTNodeKind::BinaryExpr: {
            const auto& binary = ast_node.as<Syntax::BinaryExpr>();
            auto left = build_sema_node(binary.left_);
            auto right = build_sema_node(binary.right_);
            return sema_tree_.emplace<BinaryExpr>(binary.op_, left, right, binary.source_loc_);
        }

        case ASTNodeKind::ReferenceExpr: {
            const auto& ref = ast_node.as<Syntax::ReferenceExpr>();

            auto target_symbol = ctx_.symbol_table_.lookup(ref.name_);
            if (target_symbol == -1) {
                const auto err = UndeclaredIDentiferError{std::format("use of undeclared identifier '{}'", ref.name_), ref.source_loc_};
                ctx_.diagnostics_.register_error(err);

                // fix: invalid reference state is just -1 for symbol and type references
                return sema_tree_.emplace<ReferenceExpr>(static_cast<SymbolID>(-1), static_cast<TypeID>(-1), std::move(ref.name_), ref.source_loc_);
            }

            auto target_type = ctx_.symbol_table_.symbol_pool_[target_symbol].type_id_;
            auto target_ident = ctx_.symbol_table_.symbol_pool_[target_symbol].identifier_; 

            return sema_tree_.emplace<ReferenceExpr>(target_symbol, target_type, std::move(target_ident), ref.source_loc_);
        }

        case ASTNodeKind::CallExpr: {
            const auto& call = ast_node.as<Syntax::CallExpr>();
            auto callee = build_sema_node(call.callee_);
            
            std::vector<SemaNodeID> args;
            for (auto arg : call.args_)
                args.push_back(build_sema_node(arg));
            
            return sema_tree_.emplace<CallExpr>(callee, std::move(args), call.source_loc_);
        }

        case ASTNodeKind::MemberExpr: {
            const auto& expr = ast_node.as<Syntax::MemberExpr>();
            auto base = build_sema_node(expr.base_);

            return sema_tree_.emplace<MemberExpr>(base, std::move(expr.member_), expr.is_arrow_, expr.source_loc_);
        }

        case ASTNodeKind::ArraySubscriptExpr: {
            const auto& arr_sub = ast_node.as<Syntax::ArraySubscriptExpr>();

            auto base = build_sema_node(arr_sub.base_);
            auto index = build_sema_node(arr_sub.index_);

            return sema_tree_.emplace<ArraySubscriptExpr>(base, index, arr_sub.source_loc_);
        }

        case ASTNodeKind::InitListExpr: {
            const auto& init_list = ast_node.as<Syntax::InitListExpr>();

            std::vector<SemaNodeID> values;
            for (auto v : init_list.init_values_)
                values.push_back(build_sema_node(v));

            return sema_tree_.emplace<InitListExpr>(std::move(values));
        }

        case ASTNodeKind::ExplicitCastExpr:

        default:
            std::println("{}", (int)ast_node.get_kind());
            error_exit("build_sema_node()");
    }
}