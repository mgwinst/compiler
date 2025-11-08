#include "ast.h"

AST::IntegerLiteralExpr::IntegerLiteralExpr(int value) : value{value} {}

AST::FloatLiteralExpr::FloatLiteralExpr(float value) : value{value} {}

AST::CharLiteralExpr::CharLiteralExpr(const char value) : value{value} {}

AST::StringLiteralExpr::StringLiteralExpr(std::string_view value) : value{value} {}

AST::IndexingExpr::IndexingExpr(std::string_view array, std::unique_ptr<Expr> index) : array{array}, index{std::move(index)} {}
AST::IndexingExpr::~IndexingExpr() {}

AST::BooleanExpr::BooleanExpr(bool value) : value{value} {}

AST::UnaryExpr::UnaryExpr(std::string_view op, std::unique_ptr<Expr> arg) : op{op}, arg{std::move(arg)} {}
AST::UnaryExpr::~UnaryExpr() {}

AST::BinaryExpr::BinaryExpr(const char op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right) : op{op}, left{std::move(left)}, right{std::move(right)} {}
AST::BinaryExpr::~BinaryExpr() {}

AST::VariableExpr::VariableExpr(std::string_view ident) : ident{ident} {}
AST::VariableDeclExpr::VariableDeclExpr(std::string_view type, std::string_view ident) : type{type}, ident{ident} {}

AST::IfExpr::IfExpr(std::unique_ptr<Expr> cond, std::vector<std::unique_ptr<Expr>> if_else_exprs) : cond{std::move(cond)}, if_else_exprs{std::move(if_else_exprs)} {}
AST::IfExpr::~IfExpr() {}

AST::AssignExpr::AssignExpr(std::string_view var_name, std::unique_ptr<Expr> value) : var_name{var_name}, value{std::move(value)} {}
AST::AssignExpr::~AssignExpr() {}

AST::BlockExpr::BlockExpr(std::vector<std::unique_ptr<Expr>> expressions) : expressions{std::move(expressions)} {}
AST::BlockExpr::~BlockExpr() {}

AST::FuncDeclExpr::FuncDeclExpr(std::string_view ident, std::string_view return_type, std::vector<std::string_view> param_types) : 
    ident{ident}, return_type{return_type}, param_types{std::move(param_types)} {}
AST::FuncDeclExpr::~FuncDeclExpr() {}

AST::FuncDefExpr::FuncDefExpr(std::string_view ident, std::string_view return_type, std::vector<std::pair<std::string_view, std::string_view>> parameters, std::unique_ptr<Expr> body) : 
    ident{ident}, return_type{return_type}, parameters{std::move(parameters)}, body{std::move(body)} {}
AST::FuncDefExpr::~FuncDefExpr() {}

AST::FuncCallExpr::FuncCallExpr(std::string_view ident, std::vector<std::unique_ptr<Expr>> args) : ident{ident}, args{std::move(args)} {}
AST::FuncCallExpr::~FuncCallExpr() {}

AST::ReturnExpr::ReturnExpr(std::unique_ptr<Expr> value) : value{std::move(value)} {}
AST::ReturnExpr::~ReturnExpr() {}

AST::WhileExpr::WhileExpr(std::unique_ptr<Expr> cond, std::unique_ptr<Expr> body) : cond{std::move(cond)}, body{std::move(body)} {}
AST::WhileExpr::~WhileExpr() {}

AST::ForExpr::ForExpr(std::unique_ptr<Expr> init, std::unique_ptr<Expr> cond, std::unique_ptr<Expr> update, std::unique_ptr<Expr> body) :
    init{std::move(init)}, cond{std::move(cond)}, update{std::move(update)}, body{std::move(body)} {}
AST::ForExpr::~ForExpr() {}
