#include "doctest/doctest.h"

#include "frontend/sema/helpers.hpp"

#include "helpers.hpp"

// assignment

TEST_CASE("assignment accepts same types")
{
    auto [tree, ctx] = analyze("sema/type_checker/src/assign_same_type.w");

    REQUIRE(ctx.diagnostics_.contains_errors() == false);

    auto* module = cast<Sema::ModuleDecl>(tree.root_);
    auto* func = cast<Sema::FuncDecl>(module->decls_[0]);
    auto* body = cast<Sema::CompoundStmt>(func->body_);

    auto* assignment = cast<Sema::BinaryExpr>(body->children_[2]);
    auto* left = cast<Sema::ReferenceExpr>(assignment->left_);
    auto* right = cast<Sema::ReferenceExpr>(assignment->right_);

    CHECK(left->type() == right->type());
}

TEST_CASE("assignment rejects different types")
{
    auto [tree, ctx] = analyze("sema/type_checker/src/assign_diff_type.w");

    REQUIRE(ctx.diagnostics_.contains_errors() == true);

    auto* module = cast<Sema::ModuleDecl>(tree.root_);
    auto* func = cast<Sema::FuncDecl>(module->decls_[0]);
    auto* body = cast<Sema::CompoundStmt>(func->body_);

    auto* assignment = cast<Sema::BinaryExpr>(body->children_[2]);
    auto* left = cast<Sema::ReferenceExpr>(assignment->left_);
    auto* right = cast<Sema::ReferenceExpr>(assignment->right_);

    CHECK(left->type() != right->type());

    CHECK(ctx.diagnostics_.errors_[0].msg_.contains("type mismatch"));
}

TEST_CASE("assign to const fails")
{
    auto [tree, ctx] = analyze("sema/type_checker/src/assign_to_const.w");

    REQUIRE(ctx.diagnostics_.contains_errors() == true);

    auto* module = cast<Sema::ModuleDecl>(tree.root_);
    auto* func = cast<Sema::FuncDecl>(module->decls_[0]);
    auto* body = cast<Sema::CompoundStmt>(func->body_);

    auto* assignment = cast<Sema::BinaryExpr>(body->children_[2]);
    auto* left = cast<Sema::ReferenceExpr>(assignment->left_);

    CHECK(is_const(left->type()));

    CHECK(ctx.diagnostics_.errors_[0].msg_.contains("cannot assign to a const-qualified variable"));
}

TEST_CASE("assign to rvalue fails")
{
    auto [tree, ctx] = analyze("sema/type_checker/src/assign_to_rvalue.w");

    REQUIRE(ctx.diagnostics_.contains_errors() == true);

    auto* module = cast<Sema::ModuleDecl>(tree.root_);
    auto* func = cast<Sema::FuncDecl>(module->decls_[0]);
    auto* body = cast<Sema::CompoundStmt>(func->body_);

    auto* assignment = cast<Sema::BinaryExpr>(body->children_[1]);

    CHECK(!is_lvalue(assignment->left_));

    CHECK(ctx.diagnostics_.errors_[0].msg_.contains("cannot assign to an rvalue"));
}

// addition

TEST_CASE("add accepts scalar types")
{
    auto [tree, ctx] = analyze("sema/type_checker/src/add_same_type.w");

    REQUIRE(ctx.diagnostics_.contains_errors() == false);

    auto* module = cast<Sema::ModuleDecl>(tree.root_);
    auto* func = cast<Sema::FuncDecl>(module->decls_[0]);
    auto* body = cast<Sema::CompoundStmt>(func->body_);

    auto* add = cast<Sema::BinaryExpr>(body->children_[2]);
    auto* left = cast<Sema::ReferenceExpr>(add->left_);
    auto* right = cast<Sema::ReferenceExpr>(add->right_);

    CHECK(left->type() == right->type());
    CHECK(is_scalar(left->type()));
    CHECK(is_scalar(right->type()));
}

TEST_CASE("add rejects non-scalar types")
{
    auto [tree, ctx] = analyze("sema/type_checker/src/add_non_scalar.w");

    REQUIRE(ctx.diagnostics_.contains_errors() == true);

    auto* module = cast<Sema::ModuleDecl>(tree.root_);
    auto* func = cast<Sema::FuncDecl>(module->decls_[0]);
    auto* body = cast<Sema::CompoundStmt>(func->body_);

    auto* add = cast<Sema::BinaryExpr>(body->children_[2]);
    auto* left = cast<Sema::ReferenceExpr>(add->left_);
    auto* right = cast<Sema::ReferenceExpr>(add->right_);

    CHECK(left->type() == right->type());
    CHECK(is_scalar(left->type()) == false);
    CHECK(is_scalar(right->type()) == false);

    CHECK(ctx.diagnostics_.errors_[0].msg_.contains("invalid operands"));
}

TEST_CASE("add rejects different types")
{
    auto [tree, ctx] = analyze("sema/type_checker/src/add_diff_type.w");

    REQUIRE(ctx.diagnostics_.contains_errors() == true);

    auto* module = cast<Sema::ModuleDecl>(tree.root_);
    auto* func = cast<Sema::FuncDecl>(module->decls_[0]);
    auto* body = cast<Sema::CompoundStmt>(func->body_);

    auto* add = cast<Sema::BinaryExpr>(body->children_[2]);
    auto* left = cast<Sema::ReferenceExpr>(add->left_);
    auto* right = cast<Sema::ReferenceExpr>(add->right_);

    CHECK(left->type() != right->type());

    CHECK(ctx.diagnostics_.errors_[0].msg_.contains("type mismatch")); 
}

TEST_CASE("add accepts non-const with const")
{
    auto [tree, ctx] = analyze("sema/type_checker/src/add_const_non_const.w");

    REQUIRE(ctx.diagnostics_.contains_errors() == false);

    auto* module = cast<Sema::ModuleDecl>(tree.root_);
    auto* func = cast<Sema::FuncDecl>(module->decls_[0]);
    auto* body = cast<Sema::CompoundStmt>(func->body_);

    auto* add = cast<Sema::BinaryExpr>(body->children_[2]);
    auto* left = cast<Sema::ReferenceExpr>(add->left_);
    auto* right = cast<Sema::ReferenceExpr>(add->right_);

    CHECK(remove_const_qualifier(left->type()) == remove_const_qualifier(right->type()));
}

TEST_CASE("add returns operand type")
{
    // this type of test requires manual checking of check_type() function
}