#include "ast_nodes.h"

IntegerLiteralExpr::IntegerLiteralExpr(int value) : value{value} {}

FloatLiteralExpr::FloatLiteralExpr(float value) : value{value} {}

CharLiteralExpr::CharLiteralExpr(const char value) : value{value} {}

StringLiteralExpr::StringLiteralExpr(std::string_view value) : value{value} {}

ArrayLiteralExpr::ArrayLiteralExpr(std::vector<std::unique_ptr<Expr>> array_elems) : array_elems{std::move(array_elems)} {}
ArrayLiteralExpr::~ArrayLiteralExpr() {}

ArrayIndexingExpr::ArrayIndexingExpr(std::string_view array, std::unique_ptr<Expr> index) : array{array}, index{std::move(index)} {}
ArrayIndexingExpr::~ArrayIndexingExpr() {}

BooleanExpr::BooleanExpr(bool value) : value{value} {}

UnaryExpr::UnaryExpr(std::string_view op, std::unique_ptr<Expr> arg) : op{op}, arg{std::move(arg)} {}
UnaryExpr::~UnaryExpr() {}

BinaryExpr::BinaryExpr(const char op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs) : op{op}, lhs{std::move(lhs)}, rhs{std::move(rhs)} {}
BinaryExpr::~BinaryExpr() {}

VariableExpr::VariableExpr(std::string_view name) : name{name} {}

IfExpr::IfExpr(std::unique_ptr<Expr> cond, std::vector<std::unique_ptr<Expr>> if_else_exprs) : cond{std::move(cond)}, if_else_exprs{std::move(if_else_exprs)} {}
IfExpr::~IfExpr() {}

AssignExpr::AssignExpr(std::string_view var_name, std::unique_ptr<Expr> value) : var_name{var_name}, value{std::move(value)} {}
AssignExpr::~AssignExpr() {}

CompoundExpr::CompoundExpr(std::vector<std::unique_ptr<Expr>> expressions) : expressions{std::move(expressions)} {}
CompoundExpr::~CompoundExpr() {}

FuncDeclExpr::FuncDeclExpr(std::string_view name, std::string_view return_type, std::vector<std::string_view> param_types) : 
    name{name}, return_type{return_type}, param_types{std::move(param_types)} {}
FuncDeclExpr::~FuncDeclExpr() {}

FuncDefExpr::FuncDefExpr(std::string_view name, std::string_view return_type, std::vector<std::pair<std::string_view, std::string_view>> parameters, std::unique_ptr<Expr> body) : 
    name{name}, return_type{return_type}, parameters{std::move(parameters)}, body{std::move(body)} {}
FuncDefExpr::~FuncDefExpr() {}

FuncCallExpr::FuncCallExpr(std::string_view name, std::vector<std::unique_ptr<Expr>> args) : name{name}, args{std::move(args)} {}
FuncCallExpr::~FuncCallExpr() {}

ReturnExpr::ReturnExpr(std::unique_ptr<Expr> value) : value{std::move(value)} {}
ReturnExpr::~ReturnExpr() {}

WhileExpr::WhileExpr(std::unique_ptr<Expr> cond, std::unique_ptr<Expr> body) : cond{std::move(cond)}, body{std::move(body)} {}
WhileExpr::~WhileExpr() {}

ForExpr::ForExpr(std::unique_ptr<Expr> init, std::unique_ptr<Expr> cond, std::unique_ptr<Expr> update, std::unique_ptr<Expr> body) :
    init{std::move(init)}, cond{std::move(cond)}, update{std::move(update)}, body{std::move(body)} {}
ForExpr::~ForExpr() {}

StructExpr::StructExpr(std::string_view name, std::vector<std::pair<std::string_view, std::string_view>> members) : name{name}, members{std::move(members)} {}
StructExpr::~StructExpr() {}

UnionExpr::UnionExpr(std::string_view name, std::vector<std::string_view> members) : name{name}, members{std::move(members)} {}
UnionExpr::~UnionExpr() {}
