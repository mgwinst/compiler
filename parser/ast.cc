#include <variant>
#include <vector>
#include <memory>
#include <print>
#include <span>
#include <iostream>

struct Expr;

struct IntegerLiteralExpr {
    int value;

    IntegerLiteralExpr(int value) : value{value} {}
};

struct FloatLiteralExpr {
    float value;

    FloatLiteralExpr(float value) : value{value} {}
};

struct CharLiteralExpr {
    const char value;

    CharLiteralExpr(const char value) : value{value} {}
};

struct StringLiteralExpr {
    std::string_view value;

    StringLiteralExpr(std::string_view value) : value{value} {}
};

struct ArrayLiteralExpr {
    std::vector<std::unique_ptr<Expr>> array_elems;

    ArrayLiteralExpr(std::vector<std::unique_ptr<Expr>> array_elems) :
        array_elems{std::move(array_elems)} {}
};

struct ArrayIndexingExpr {
    std::string_view array;
    std::unique_ptr<Expr> index;

    ArrayIndexingExpr(std::string_view array, std::unique_ptr<Expr> index) : 
        array{array}, index{std::move(index)} {}
};

struct BooleanExpr {
    bool value;

    BooleanExpr(bool value) : value{value} {}
};

struct UnaryExpr {
    std::string_view op;
    std::unique_ptr<Expr> arg;

    UnaryExpr(std::string_view op, std::unique_ptr<Expr> arg) :
        op{op}, arg{std::move(arg)} {}
};

struct BinaryExpr {
    const char op;
    std::unique_ptr<Expr> lhs, rhs;

    BinaryExpr(const char op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs) :
        op{op}, lhs{std::move(lhs)}, rhs{std::move(rhs)} {}
};

struct VariableExpr {
    std::string_view name;

    VariableExpr(std::string_view name) : name{name} {}
};

struct IfExpr {
    std::unique_ptr<Expr> cond_expr;
    std::vector<std::unique_ptr<Expr>> if_else_exprs;

    IfExpr(std::unique_ptr<Expr> cond_expr, std::vector<std::unique_ptr<Expr>> if_else_exprs) : 
        cond_expr{std::move(cond_expr)}, if_else_exprs{std::move(if_else_exprs)} {}
};

struct AssignExpr {
    std::string_view var_name;
    std::unique_ptr<Expr> value;

    AssignExpr(std::string_view var_name, std::unique_ptr<Expr> value) : 
        var_name{var_name}, value{std::move(value)} {}
};

struct CompoundExpr {
    std::vector<std::unique_ptr<Expr>> expressions;

    CompoundExpr(std::vector<std::unique_ptr<Expr>> expressions) : expressions{std::move(expressions)} {}
};

// types only
struct FuncDeclExpr {
    std::string_view name, return_type;
    std::vector<std::string_view> param_types;

    FuncDeclExpr(std::string_view name, std::string_view return_type, std::vector<std::string_view> param_types) : 
        name{name}, return_type{return_type}, param_types{std::move(param_types)} {}
};

struct FuncDefExpr {
    std::string_view name, return_type;
    std::vector<std::pair<std::string_view, std::string_view>> parameters;
    std::unique_ptr<Expr> body;

    FuncDefExpr(std::string_view name, std::string_view return_type, std::vector<std::pair<std::string_view, std::string_view>> parameters, std::unique_ptr<Expr> body) : 
        name{name}, return_type{return_type}, parameters{std::move(parameters)}, body{std::move(body)} {}
};

struct FuncCallExpr {
    std::string_view name;
    std::vector<std::unique_ptr<Expr>> args;

    FuncCallExpr(std::string_view name, std::vector<std::unique_ptr<Expr>> args) : 
        name{name}, args{std::move(args)} {}
};

struct ReturnExpr {
    std::unique_ptr<Expr> value;

    ReturnExpr(std::unique_ptr<Expr> value) : value{std::move(value)} {}
};

struct WhileExpr {
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Expr> body;

    WhileExpr(std::unique_ptr<Expr> cond, std::unique_ptr<Expr> body) : 
        cond{std::move(cond)}, body{std::move(body)} {}

};

struct ForExpr {
    std::unique_ptr<Expr> init, cond, update, body;

    ForExpr(std::unique_ptr<Expr> init, std::unique_ptr<Expr> cond, std::unique_ptr<Expr> update, std::unique_ptr<Expr> body) :
        init{std::move(init)}, cond{std::move(cond)}, update{std::move(update)}, body{std::move(body)} {}
};

struct StructExpr {
    std::string_view name;
    std::vector<std::pair<std::string_view, std::string_view>> members;

    StructExpr(std::string_view name, std::vector<std::pair<std::string_view, std::string_view>> members) : 
        name{name}, members{std::move(members)} {}
};

struct UnionExpr {
    std::string_view name;
    std::vector<std::string_view> members;

    UnionExpr(std::string_view name, std::vector<std::string_view> members) : 
        name{name}, members{std::move(members)} {}
};

struct Expr {
    std::variant<
        IntegerLiteralExpr,
        FloatLiteralExpr,
        CharLiteralExpr,
        StringLiteralExpr,
        ArrayLiteralExpr,
        ArrayIndexingExpr,
        BooleanExpr,
        BinaryExpr,
        VariableExpr,
        IfExpr,
        AssignExpr,
        CompoundExpr,
        FuncDeclExpr,
        FuncDefExpr,
        FuncCallExpr,
        ReturnExpr,
        WhileExpr,
        ForExpr,
        StructExpr,
        UnionExpr
    > kind;
};

int main() {
    
}



