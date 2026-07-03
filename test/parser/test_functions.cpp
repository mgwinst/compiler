#include "test_parser.hpp"

TEST_CASE("function declaration")
{
    auto ast = parse("parser/srctest/function_no_parameter.w");

    auto* module = cast<ModuleDecl>(ast.root_);
    CHECK(module->decls_.size() == 1);

    auto* func = cast<FuncDecl>(module->decls_[0]);
    CHECK(func->name_ == "func");
    CHECK(func->params_.size() == 0);
    CHECK(cast<NamedTypeExpr>(func->return_type_)->name_ == "void");
}

TEST_CASE("single parameter")
{
    auto ast = parse("parser/srctest/function_single_parameter.w");

    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);

    CHECK(func->params_.size() == 1);

    auto* param = cast<ParamDecl>(func->params_[0]);

    CHECK(param->name_ == "x");

    CHECK(cast<NamedTypeExpr>(param->type_expr_)->name_ == "int");
}

TEST_CASE("multiple parameters")
{
    auto ast = parse("parser/srctest/function_multi_parameter.w");

    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);

    CHECK(func->params_.size() == 2);

    auto* param1 = cast<ParamDecl>(func->params_[0]);
    auto* param2 = cast<ParamDecl>(func->params_[1]);

    CHECK(param1->name_ == "x");
    CHECK(param2->name_ == "y");

    CHECK(cast<NamedTypeExpr>(param1->type_expr_)->name_ == "int");
    CHECK(cast<NamedTypeExpr>(param2->type_expr_)->name_ == "int");
}

TEST_CASE("missing 'fn' keyword")
{

}

TEST_CASE("missing return type")
{
    Module module = get_module("parser/srctest/function_missing_return_type.w");
    Parser parser{module};
    AST ast = parser.run();

    REQUIRE(parser.diagnostics_.contains_errors());
    CHECK(parser.diagnostics_.errors_[0].msg_.contains("syntax error: function missing trailing return type"));
}

