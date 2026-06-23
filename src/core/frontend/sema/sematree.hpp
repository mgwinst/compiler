#pragma once

#include <cstdint>
#include <vector>
#include <optional>
#include <utility>
#include <vector>
#include <memory>
#include <ranges>

#include "frontend/error/source.hpp"
#include "frontend/sema/symbol.hpp"
#include "utils/structures/arena.hpp"
#include "utils/enums.hpp"

enum class SemaNodeKind : uint8_t 
{
    ModuleDecl,
    VarDecl,
    ParamDecl,
    FuncDecl,
    RecordDecl,

    CompoundStmt,
    ReturnStmt,
    BreakStmt,
    ContinueStmt,
    IfStmt,
    WhileStmt,
    ForStmt,

    IntegerLiteralExpr,
    FloatLiteralExpr,
    CharLiteralExpr,
    StringLiteralExpr,
    BooleanLiteralExpr,

    UnaryExpr,
    BinaryExpr,
    ReferenceExpr,
    CallExpr,
    MemberExpr,
    ArraySubscriptExpr,
    InitListExpr,
    ExplicitCastExpr,
    ImplicitCastExpr,
    Invalid
};

namespace Sema {

struct SemaNode
{
    SemaNodeKind kind_;
    Source source_;

    explicit SemaNode(SemaNodeKind kind, Source source = {}) :
        kind_{ kind },
        source_{ source } {}
};

struct Decl : SemaNode
{
    Symbol* symbol_;

    Decl(SemaNodeKind kind, Symbol* symbol = nullptr, Source source = {}) :
        SemaNode{kind, source},
        symbol_{ symbol } {}

    Type* type()
    {
        assert(kind_ != SemaNodeKind::ModuleDecl);
        assert(symbol_ != nullptr);

        return symbol_->type_;
    }

    std::string name()
    {
        assert(kind_ != SemaNodeKind::ModuleDecl);
        assert(symbol_ != nullptr);

        return symbol_->identifier_;
    }
};

struct Stmt : SemaNode
{
    using SemaNode::SemaNode;
};

struct Expr : Stmt
{
    using Stmt::Stmt;
};

struct ModuleDecl : Decl
{
    std::string name_;
    std::vector<SemaNode*> decls_;

    ModuleDecl(std::string name) :
        Decl{ SemaNodeKind::ModuleDecl },
        name_{ std::move(name) },
        decls_{} {}
};

struct VarDecl : Decl
{
    SemaNode* init_;

    VarDecl(Symbol* symbol, SemaNode* init = nullptr, Source source = {}) :
        Decl{SemaNodeKind::VarDecl, symbol, source},
        init_{ init } {}
};

struct ParamDecl : Decl
{
    // SemaNode* init = default value

    ParamDecl(Symbol* symbol, Source source = {}) :
        Decl{SemaNodeKind::ParamDecl, symbol, source} {}
};

struct FuncDecl : Decl
{
    std::vector<SemaNode*> params_;
    SemaNode* body_;

    FuncDecl(Symbol* symbol, std::vector<SemaNode*> params, SemaNode* body, Source source = {}) :
        Decl{SemaNodeKind::FuncDecl, symbol, source},
        params_{ std::move(params) },
        body_{ body } {}
};

struct RecordDecl : Decl
{
    RecordKind kind_;
    std::vector<SemaNode*> fields_;

    RecordDecl(RecordKind kind, Symbol* symbol, std::vector<SemaNode*> fields, Source source = {}) :
        Decl{SemaNodeKind::RecordDecl, symbol, source},
        kind_{ kind },
        fields_{ std::move(fields) } {}
};

struct CompoundStmt : Stmt
{
    std::vector<SemaNode*> children_; // exprs/decls/returns

    CompoundStmt(std::vector<SemaNode*> children) :
        Stmt{ SemaNodeKind::CompoundStmt },
        children_{ std::move(children) } {}
};

struct ReturnStmt : Stmt
{
    SemaNode* value_;

    ReturnStmt(SemaNode* value = nullptr) :
        Stmt{ SemaNodeKind::ReturnStmt },
        value_{ value } {}
};

struct BreakStmt : Stmt
{
    BreakStmt() :
        Stmt{ SemaNodeKind::BreakStmt } {}
};

struct ContinueStmt : Stmt
{
    ContinueStmt() :
        Stmt{ SemaNodeKind::ContinueStmt } {}
};

struct IfStmt : Stmt {
    SemaNode* cond_;
    SemaNode* then_stmt_; 
    SemaNode* else_stmt_; // point to another IfStmt node for elif chaining

    IfStmt(SemaNode* cond, SemaNode* then_stmt, SemaNode* else_stmt = nullptr) :
        Stmt{ SemaNodeKind::IfStmt },
        cond_{ cond },
        then_stmt_{ then_stmt },
        else_stmt_{ else_stmt } {}
};

struct WhileStmt : Stmt
{
    SemaNode* cond_;
    SemaNode* body_;

    WhileStmt(SemaNode* cond, SemaNode* body) :
        Stmt{ SemaNodeKind::WhileStmt },
        cond_{ cond },
        body_{ body } {}
};

struct ForStmt : Stmt
{
    SemaNode* init_;
    SemaNode* cond_;
    SemaNode* update_;
    SemaNode* body_;

    ForStmt(SemaNode* init, SemaNode* cond, SemaNode* update, SemaNode* body) :
        Stmt{ SemaNodeKind::ForStmt },
        init_{ init },
        cond_{ cond },
        update_{ update },
        body_{ body } {}
};

struct IntegerLiteralExpr : Expr
{
    int64_t value_;

    IntegerLiteralExpr(int64_t value) :
        Expr{ SemaNodeKind::IntegerLiteralExpr },
        value_{ value } {}
};

struct FloatLiteralExpr : Expr
{
    double value_;

    FloatLiteralExpr(double value) :
        Expr{ SemaNodeKind::FloatLiteralExpr },
        value_{ value } {}
};

struct CharLiteralExpr : Expr
{
    char value_;

    CharLiteralExpr(char value) :
        Expr{ SemaNodeKind::CharLiteralExpr },
        value_{ value } {}
};

struct StringLiteralExpr : Expr
{
    std::string value_;

    StringLiteralExpr(std::string value) :
        Expr{ SemaNodeKind::StringLiteralExpr },
        value_{ std::move(value) } {}
};

struct BooleanLiteralExpr : Expr
{
    bool value_;

    BooleanLiteralExpr(bool value) :
        Expr{ SemaNodeKind::BooleanLiteralExpr },
        value_{ value } {}
};

struct UnaryExpr : Expr
{
    std::string op_;
    SemaNode* operand_;
    bool is_postfix_ = false;

    UnaryExpr(std::string op, SemaNode* operand, bool is_postfix, Source source = {}) :
        Expr{SemaNodeKind::UnaryExpr, source},
        op_{ std::move(op) },
        operand_{ operand },
        is_postfix_{ is_postfix } {}
};

struct BinaryExpr : Expr
{
    std::string op_;
    SemaNode *left_, *right_;

    BinaryExpr(std::string op, SemaNode* left, SemaNode* right, Source source = {}) :
        Expr{SemaNodeKind::BinaryExpr, source},
        op_{ std::move(op) },
        left_{ left },
        right_{ right } {}
};

struct ReferenceExpr : Expr
{
    Symbol* symbol_;
    std::string name_;

    ReferenceExpr(Symbol* symbol, std::string name, Source source = {}) : 
        Expr{SemaNodeKind::ReferenceExpr, source},
        symbol_{ symbol },
        name_{ std::move(name) } {}

    Type* type()
    {
        assert(symbol_);
        return symbol_->type_;
    }

    std::string name()
    {
        assert(symbol_);
        return symbol_->identifier_;
    }
};

struct CallExpr : Expr
{
    SemaNode* callee_; // callee is a reference expression
    std::vector<SemaNode*> args_;

    CallExpr(SemaNode* callee, std::vector<SemaNode*> args, Source source = {}) :
        Expr{SemaNodeKind::CallExpr, source},
        callee_{ callee },
        args_{ std::move(args) } {}
};

struct MemberExpr : Expr
{
    SemaNode* base_;
    std::string member_;
    bool is_arrow_;
    
    MemberExpr(SemaNode* base, std::string member, bool is_arrow = false, Source source = {}) :
        Expr{SemaNodeKind::MemberExpr, source},
        base_{ base },
        member_{ std::move(member) },
        is_arrow_{ is_arrow } {}
};

struct ArraySubscriptExpr : Expr
{
    SemaNode* base_;
    SemaNode* index_;

    ArraySubscriptExpr(SemaNode* base, SemaNode* index, Source source = {}) :
        Expr{SemaNodeKind::ArraySubscriptExpr, source},
        base_{ base },
        index_{ index } {}
};

struct InitListExpr : Expr
{
    std::vector<SemaNode*> init_values_;
    
    InitListExpr(std::vector<SemaNode*> init_values, Source source = {}) :
        Expr{SemaNodeKind::InitListExpr, source},
        init_values_{ std::move(init_values) } {}
};

struct ExplicitCastExpr : Expr
{
    ExplicitCastExpr() :
        Expr(SemaNodeKind::ExplicitCastExpr) {}
};

} // namespace Sema

template <typename T>
inline constexpr SemaNodeKind sema_node_kind_v = SemaNodeKind::Invalid;

template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::ModuleDecl>            = SemaNodeKind::ModuleDecl;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::VarDecl>               = SemaNodeKind::VarDecl;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::ParamDecl>             = SemaNodeKind::ParamDecl;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::FuncDecl>              = SemaNodeKind::FuncDecl;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::RecordDecl>            = SemaNodeKind::RecordDecl;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::CompoundStmt>          = SemaNodeKind::CompoundStmt;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::ReturnStmt>            = SemaNodeKind::ReturnStmt;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::BreakStmt>             = SemaNodeKind::BreakStmt;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::ContinueStmt>          = SemaNodeKind::ContinueStmt;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::IfStmt>                = SemaNodeKind::IfStmt;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::WhileStmt>             = SemaNodeKind::WhileStmt;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::ForStmt>               = SemaNodeKind::ForStmt;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::IntegerLiteralExpr>    = SemaNodeKind::IntegerLiteralExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::FloatLiteralExpr>      = SemaNodeKind::FloatLiteralExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::CharLiteralExpr>       = SemaNodeKind::CharLiteralExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::StringLiteralExpr>     = SemaNodeKind::StringLiteralExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::BooleanLiteralExpr>    = SemaNodeKind::BooleanLiteralExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::UnaryExpr>             = SemaNodeKind::UnaryExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::BinaryExpr>            = SemaNodeKind::BinaryExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::ReferenceExpr>         = SemaNodeKind::ReferenceExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::CallExpr>              = SemaNodeKind::CallExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::MemberExpr>            = SemaNodeKind::MemberExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::ArraySubscriptExpr>    = SemaNodeKind::ArraySubscriptExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::InitListExpr>          = SemaNodeKind::InitListExpr;
template <> inline constexpr SemaNodeKind sema_node_kind_v<Sema::ExplicitCastExpr>      = SemaNodeKind::ExplicitCastExpr;

template <typename T>
concept DerivedFromSemaNode = std::derived_from<T, Sema::SemaNode>;

template <typename T>
concept AbstractNode = 
    std::same_as<T, Sema::Decl> || 
    std::same_as<T, Sema::Stmt> || 
    std::same_as<T, Sema::Expr>;

class SemaTree
{
public:
    Sema::SemaNode* root_ = nullptr;

    SemaTree() = default;

    SemaTree(const SemaTree&) = delete;
    SemaTree& operator=(const SemaTree&) = delete;
    SemaTree(SemaTree&&) = default;
    SemaTree& operator=(SemaTree&&) = default;
    ~SemaTree() = default;

    template <DerivedFromSemaNode T, typename... Args>
        requires std::is_constructible_v<T, Args...>
    [[nodiscard]] T* emplace(Args&&... args)
    {
        return arena.emplace<T>(std::forward<Args>(args)...);
    }

private:
    Arena arena;
};