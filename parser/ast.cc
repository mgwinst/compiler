#include "ast.h"

// ************** DECLARATIONS **************

GlobalVarDecl::GlobalVarDecl(bool is_const, std::string type, std::string ident, std::optional<ExprRef> init) : 
    is_const_{ is_const }, type_{ type }, ident_{ ident }, init_{ init } {}

LocalVarDecl::LocalVarDecl(bool is_const, std::string type, std::string ident, std::optional<ExprRef> init) : 
    is_const_{ is_const }, type_{ type }, ident_{ ident }, init_{ init } {}

StructDecl::StructDecl(std::string ident, std::vector<std::pair<std::string, std::string>> fields) :
    ident_{ ident }, fields_{ fields } {}

FuncDecl::FuncDecl(std::string ident, std::string return_type, std::vector<std::pair<std::string, std::string>> parameters, StmtRef body) :
    ident_{ ident }, return_type_{ return_type }, parameters_{ parameters }, body_{ body } {}

// ************** STATEMENTS **************

BlockStmt::BlockStmt(std::vector<std::variant<DeclRef, StmtRef, ExprRef>> contents) :
    contents_{ contents } {}

ReturnStmt::ReturnStmt(ExprRef value) :
    value_{ value } {}

IfStmt::IfStmt(ExprRef cond, std::vector<ExprRef> if_else_exprs) :
    cond_{ cond }, if_else_exprs_{ if_else_exprs } {}

WhileStmt::WhileStmt(ExprRef cond, StmtRef body) :
    cond_{ cond }, body_{ body } {}

ForStmt::ForStmt(ExprRef init, ExprRef cond, ExprRef update, StmtRef body) :
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

VariableExpr::VariableExpr(std::string ident) :
    ident_{ ident } {}

AssignExpr::AssignExpr(std::string assignee, ExprRef value) :
    assignee_{ assignee }, value_{ value } {}

IndexExpr::IndexExpr(std::string array, ExprRef index) :
    array_{ array }, index_{ index } {}

CallExpr::CallExpr(std::string ident, std::vector<ExprRef> args) :
    ident_{ ident }, args_{ args } {}