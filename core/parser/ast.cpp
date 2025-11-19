#include "parser/ast.hpp"

// ************** DECLARATIONS **************

VarDecl::VarDecl(std::string type, std::string name, std::optional<ExprRef> init) : 
    type_{ type }, name_{ name }, init_{ init } {}

ConstVarDecl::ConstVarDecl(std::string type, std::string name, ExprRef init) : 
    type_{ type }, name_{ name }, init_{ init } {}

ParamDecl::ParamDecl(bool is_const, std::string type, std::string name) : 
    is_const_{ is_const }, type_{ type }, name_{ name } {}

StructDecl::StructDecl(std::string name, std::vector<std::pair<std::string, std::string>> fields) :
    name_{ name }, fields_{ std::move(fields) } {}

FuncDecl::FuncDecl(std::string name, std::string return_type, std::vector<DeclRef> params, ExprRef body) :
    name_{ name }, return_type_{ return_type }, params_{ std::move(params) }, body_{ body } {}

// ************** STATEMENTS **************

CompoundStmt::CompoundStmt(std::vector<DeclRef> decls, std::vector<ExprRef> exprs) :
    decls_{ std::move(decls) }, exprs_{ std::move(exprs) } {}

ReturnStmt::ReturnStmt(ExprRef value) :
    value_{ value } {}

IfStmt::IfStmt(ExprRef cond, std::vector<ExprRef> if_else_exprs) :
    cond_{ cond }, if_else_exprs_{ std::move(if_else_exprs) } {}

WhileStmt::WhileStmt(ExprRef cond, ExprRef body) :
    cond_{ cond }, body_{ body } {}

ForStmt::ForStmt(ExprRef init, ExprRef cond, ExprRef update, ExprRef body) :
    init_{ init }, cond_{ cond }, update_{ update }, body_{ body } {}

// ************** EXPRESSIONS **************

IntegerLiteralExpr::IntegerLiteralExpr(int32_t value) :
    value_{ value } {}

FloatLiteralExpr::FloatLiteralExpr(float value) :
    value_{ value } {}

CharLiteralExpr::CharLiteralExpr(const char value) : 
    value_{ value } {}

StringLiteralExpr::StringLiteralExpr(std::string value) :
    value_{ value } {}

BooleanExpr::BooleanExpr(bool value) :
    value_{ value } {}

UnaryExpr::UnaryExpr(std::string op, ExprRef arg) :
    op_{ op }, arg_{ arg } {}

BinaryExpr::BinaryExpr(std::string op, ExprRef left, ExprRef right) :
    op_{ op }, left_{ left }, right_{ right } {}

RefExpr::RefExpr(std::string name) :
    name_{ name } {}

IndexExpr::IndexExpr(std::string array, ExprRef index) :
    array_{ array }, index_{ index } {}

CallExpr::CallExpr(std::string name, std::vector<ExprRef> args) :
    name_{ name }, args_{ std::move(args) } {}