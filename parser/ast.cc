#include "ast.h"

using AST::Expr;

// ************** DECLARATIONS **************



AST::LocalVariableDecl::LocalVariableDecl(std::string type, std::string ident, std::optional<Expr> init, std::optional<bool> is_const) :
    type{ type }, ident{ ident }, init{ std::move(init) }, is_const{ is_const } {}

AST::GlobalVariableDecl::GlobalVariableDecl(std::string type, std::string ident, std::optional<Expr> init, std::optional<bool> is_const) :
    type{ type }, ident{ ident }, init{ std::move(init) }, is_const{ is_const } {}
    
AST::StructDecl::StructDecl(std::string ident, std::vector<std::pair<std::string, std::string>> fields) :
    ident{ ident }, fields{ std::move(fields) } {}

AST::FuncDecl::FuncDecl(std::string ident, std::string return_type, std::vector<std::pair<std::string, std::string>> parameters, Expr body): 
    ident{ ident }, return_type{ return_type }, parameters{ std::move(parameters) }, body{ std::move(body) } {}



// ************** STATEMENTS **************



AST::IfStatement::IfStatement(Expr cond, std::vector<Expr> if_else_exprs) :
    cond{ std::move(cond) }, if_else_exprs{ std::move(if_else_exprs) } {}

AST::ReturnStatement::ReturnStatement(Expr value) :
    value{ std::move(value) } {}

AST::WhileStatement::WhileStatement(Expr cond, Expr body) :
    cond{ std::move(cond) }, body{ std::move(body) } {}

AST::ForStatement::ForStatement(Expr init, Expr cond, Expr update, Expr body) :
    init{std::move(init)}, cond{std::move(cond)}, update{std::move(update)}, body{std::move(body)} {}



// ************** EXPRESSIONS **************



AST::IntegerLiteralExpr::IntegerLiteralExpr(int value) : 
    value{value} {}

AST::FloatLiteralExpr::FloatLiteralExpr(float value) : 
    value{value} {}

AST::CharLiteralExpr::CharLiteralExpr(const char value) : 
    value{value} {}

AST::StringLiteralExpr::StringLiteralExpr(std::string value) : 
    value{value} {}

AST::AssignExpr::AssignExpr(std::string left_name, Expr value) : 
    left_name{ left_name }, value{ std::move(value) } {}

AST::IndexingExpr::IndexingExpr(std::string array, Expr index) :
    array{ array }, index{ std::move(index) } {}

AST::BooleanExpr::BooleanExpr(bool value) :
    value{ value } {}

AST::UnaryExpr::UnaryExpr(std::string op, Expr arg) : 
    op{ op }, arg{ std::move(arg) } {}

AST::BinaryExpr::BinaryExpr(char op, Expr left, Expr right) : 
    op{ op }, left{ std::move(left) }, right{ std::move(right) } {}

AST::VariableExpr::VariableExpr(std::string ident) : 
    ident{ ident } {}

AST::BlockExpr::BlockExpr(std::vector<Expr> expressions) :
    expressions{ std::move(expressions) } {}

AST::FuncCallExpr::FuncCallExpr(std::string ident, std::vector<Expr> args) :
    ident{ ident }, args{ std::move(args) } {}