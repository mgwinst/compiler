#include "test_parser.hpp"

TEST_CASE("variable declaration")
{
    auto ast = parse("parser/srctest/variable.w");

    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);
    auto* body = cast<CompoundStmt>(func->body_);

    REQUIRE(body->children_.size() == 1);

    auto* var = cast<VarDecl>(body->children_[0]);

    CHECK(var->name_ == "x");
    CHECK(cast<NamedTypeExpr>(var->type_expr_)->name_ == "int");
    CHECK(var->init_ == nullptr);
}

TEST_CASE("variable initialization")
{
    auto ast = parse("parser/srctest/var_init_with_var.w");

    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);
    auto* body = cast<CompoundStmt>(func->body_);

    REQUIRE(body->children_.size() == 2);

    auto* var1 = cast<VarDecl>(body->children_[0]);
    auto* var2 = cast<VarDecl>(body->children_[1]);

    CHECK(var1->name_ == "x");
    CHECK(var2->name_ == "y");

    CHECK(cast<ReferenceExpr>(var2->init_)->name_ == "x");
}

TEST_CASE()