#include "helpers.hpp"

// Unary Ops

TEST_CASE("address-of")
{
    auto ast = parse("parser/src/address_of.w");

    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);
    auto* body = cast<CompoundStmt>(func->body_);

    auto* unary = cast<UnaryExpr>(body->children_[0]);
    
    CHECK(unary->op_ == "&");
    CHECK(isa<ReferenceExpr>(unary->operand_));
    CHECK(unary->is_postfix_ == false);
}

TEST_CASE("pointer dereference")
{
    auto ast = parse("parser/src/pointer_dereference.w");

    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);
    auto* body = cast<CompoundStmt>(func->body_);
    
    auto* unary = cast<UnaryExpr>(body->children_[0]);
    
    CHECK(unary->op_ == "*");
    CHECK(isa<ReferenceExpr>(unary->operand_));
    CHECK(unary->is_postfix_ == false);
}

TEST_CASE("prefix increment")
{
    auto ast = parse("parser/src/increment.w");

    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);
    auto* body = cast<CompoundStmt>(func->body_);
    
    auto* unary = cast<UnaryExpr>(body->children_[0]);

    CHECK(unary->op_ == "++");
    CHECK(unary->is_postfix_ == false);
}

TEST_CASE("prefix decrement")
{
        auto ast = parse("parser/src/decrement.w");

    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);
    auto* body = cast<CompoundStmt>(func->body_);
    
    auto* unary = cast<UnaryExpr>(body->children_[0]);

    CHECK(unary->op_ == "--");
    CHECK(unary->is_postfix_ == false);
}

// Binary Ops

TEST_CASE("assignment")
{
    auto ast = parse("parser/src/assignment.w");

    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);
    auto* body = cast<CompoundStmt>(func->body_);

    auto* binary = cast<BinaryExpr>(body->children_[0]);

    CHECK(binary->op_ == "=");
}

TEST_CASE("compound assignment")
{
    auto ast = parse("parser/src/compound_assignment.w");

    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);
    auto* body = cast<CompoundStmt>(func->body_);

    auto* binary = cast<BinaryExpr>(body->children_[0]);

    CHECK(binary->op_ == "+=");
}

TEST_CASE("a + b")
{
    auto ast = parse("parser/src/add.w");

    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);
    auto* body = cast<CompoundStmt>(func->body_);

    auto* binary = cast<BinaryExpr>(body->children_[0]);

    CHECK(binary->op_ == "+");
}

TEST_CASE("function call no arguments")
{
    auto ast = parse("parser/src/call.w");

    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);
    auto* body = cast<CompoundStmt>(func->body_);

    auto* call = cast<CallExpr>(body->children_[0]);

    CHECK(isa<ReferenceExpr>(call->callee_));
    
    REQUIRE(call->args_.size() == 2);

    CHECK(isa<ReferenceExpr>(call->args_[0]));
    CHECK(isa<ReferenceExpr>(call->args_[1]));
}

TEST_CASE("direct member access")
{
    auto ast = parse("parser/src/member_direct.w");

    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);
    auto* body = cast<CompoundStmt>(func->body_);

    auto* member_expr = cast<MemberExpr>(body->children_[0]);

    CHECK(cast<ReferenceExpr>(member_expr->base_)->name_ == "foo");
    CHECK(member_expr->member_ == "x");
    CHECK(member_expr->is_arrow_ == false);
}

TEST_CASE("indirect member access")
{
    auto ast = parse("parser/src/member_indirect.w");

    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);
    auto* body = cast<CompoundStmt>(func->body_);

    auto* member_expr = cast<MemberExpr>(body->children_[0]);

    CHECK(cast<ReferenceExpr>(member_expr->base_)->name_ == "foo");
    CHECK(member_expr->member_ == "x");
    CHECK(member_expr->is_arrow_ == true);

}

TEST_CASE("array subscript")
{
    auto ast = parse("parser/src/array_subscript.w");

    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);
    auto* body = cast<CompoundStmt>(func->body_);

    auto* arr_sub_expr = cast<ArraySubscriptExpr>(body->children_[0]);

    CHECK(isa<ReferenceExpr>(arr_sub_expr->base_));
    CHECK(isa<ReferenceExpr>(arr_sub_expr->index_));
}

// literals, int, float, char, string, byte(hex)
// type casting
// comments

