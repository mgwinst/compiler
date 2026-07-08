#include "helpers.hpp"

TEST_CASE("return statement")
{
    auto ast = parse("parser/src/return.w");

    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);
    auto* body = cast<CompoundStmt>(func->body_);

    REQUIRE(body->children_.size() == 1);

    auto* return_stmt = cast<ReturnStmt>(body->children_[0]);

    CHECK(isa<ReferenceExpr>(return_stmt->value_));
}

TEST_CASE("if statement")
{
    auto ast = parse("parser/src/ifstmt.w");
    
    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);
    auto* body = cast<CompoundStmt>(func->body_);
 
    REQUIRE(body->children_.size() == 1);
    
    auto* ifstmt = cast<IfStmt>(body->children_[0]);

    CHECK(isa<BinaryExpr>(ifstmt->cond_));
    CHECK(isa<CompoundStmt>(ifstmt->then_stmt_));
    CHECK(ifstmt->else_stmt_ == nullptr);
} 

TEST_CASE("if-else")
{
    auto ast = parse("parser/src/ifelse.w");
    
    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);
    auto* body = cast<CompoundStmt>(func->body_);
 
    REQUIRE(body->children_.size() == 1);
    
    auto* ifstmt = cast<IfStmt>(body->children_[0]);

    CHECK(isa<BinaryExpr>(ifstmt->cond_));
    CHECK(isa<CompoundStmt>(ifstmt->then_stmt_));
    CHECK(isa<CompoundStmt>(ifstmt->else_stmt_));
} 

TEST_CASE("else-if")
{
    
}

TEST_CASE("while statement")
{
    auto ast = parse("parser/src/whilestmt.w");

    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);
    auto* body = cast<CompoundStmt>(func->body_);
 
    REQUIRE(body->children_.size() == 1);
    
    auto* whilestmt = cast<WhileStmt>(body->children_[0]);

    CHECK(isa<BinaryExpr>(whilestmt->cond_));
    CHECK(isa<CompoundStmt>(whilestmt->body_));
}

TEST_CASE("for statement")
{
    auto ast = parse("parser/src/forstmt.w");

    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);
    auto* body = cast<CompoundStmt>(func->body_);
 
    REQUIRE(body->children_.size() == 1);
    
    auto* forstmt = cast<ForStmt>(body->children_[0]);

    CHECK(isa<VarDecl>(forstmt->init_));
    CHECK(isa<BinaryExpr>(forstmt->cond_));
    CHECK(isa<UnaryExpr>(forstmt->update_));
    CHECK(isa<CompoundStmt>(forstmt->body_));
}


TEST_CASE("break statment")
{
    auto ast = parse("parser/src/breakstmt.w");

    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);
    auto* body = cast<CompoundStmt>(func->body_);
 
    REQUIRE(body->children_.size() == 1);
    
    auto* forstmt = cast<ForStmt>(body->children_[0]);

    auto* for_body = cast<CompoundStmt>(forstmt->body_);

    REQUIRE(for_body->children_.size() == 1);

    CHECK(isa<BreakStmt>(for_body->children_[0]));

}

TEST_CASE("continue statment")
{
    auto ast = parse("parser/src/continuestmt.w");

    auto* module = cast<ModuleDecl>(ast.root_);
    auto* func = cast<FuncDecl>(module->decls_[0]);
    auto* body = cast<CompoundStmt>(func->body_);
 
    REQUIRE(body->children_.size() == 1);
    
    auto* forstmt = cast<ForStmt>(body->children_[0]);

    auto* for_body = cast<CompoundStmt>(forstmt->body_);

    REQUIRE(for_body->children_.size() == 1);

    CHECK(isa<ContinueStmt>(for_body->children_[0]));
}
