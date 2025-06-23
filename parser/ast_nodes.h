#pragma once

#include <vector>
#include <variant>
#include <memory>
#include <utility>
#include <format>
#include <type_traits>
#include <iostream>

namespace AST {

    struct Expr;

    struct IntegerLiteralExpr {
        int value;
        IntegerLiteralExpr(int value);
    };

    struct FloatLiteralExpr {
        float value;
        FloatLiteralExpr(float value);
    };

    struct CharLiteralExpr {
        const char value;
        CharLiteralExpr(const char value);
    };

    struct StringLiteralExpr {
        std::string_view value;
        StringLiteralExpr(std::string_view value);
    };

    struct BooleanExpr {
        bool value;
        BooleanExpr(bool value);
    };

    struct UnaryExpr {
        std::string_view op;
        std::unique_ptr<Expr> arg;

        UnaryExpr(std::string_view op, std::unique_ptr<Expr> arg);
        ~UnaryExpr();
    };

    struct BinaryExpr {
        const char op;
        std::unique_ptr<Expr> lhs, rhs;

        BinaryExpr(const char op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs);
        ~BinaryExpr();
    };

    struct VariableExpr {
        std::string_view ident;
        VariableExpr(std::string_view);
    };

    struct VariableDeclExpr {
        std::string_view type, ident;
        VariableDeclExpr(std::string_view, std::string_view);
    };

    struct IfExpr {
        std::unique_ptr<Expr> cond;
        std::vector<std::unique_ptr<Expr>> if_else_exprs;

        IfExpr(std::unique_ptr<Expr> cond, std::vector<std::unique_ptr<Expr>> if_else_exprs);
        ~IfExpr();
    };

    struct AssignExpr {
        std::string_view var_name;
        std::unique_ptr<Expr> value;

        AssignExpr(std::string_view var_name, std::unique_ptr<Expr> value);
        ~AssignExpr();
    };

    struct CompoundExpr {
        std::vector<std::unique_ptr<Expr>> expressions;

        CompoundExpr(std::vector<std::unique_ptr<Expr>> expressions);
        ~CompoundExpr();
    };

    // types only
    struct FuncDeclExpr {
        std::string_view ident, return_type;
        std::vector<std::string_view> param_types;

        FuncDeclExpr(std::string_view ident, std::string_view return_type, std::vector<std::string_view> param_types);
        ~FuncDeclExpr();
    };

    struct FuncDefExpr {
        std::string_view ident, return_type;
        std::vector<std::pair<std::string_view, std::string_view>> parameters;
        std::unique_ptr<Expr> body;

        FuncDefExpr(std::string_view ident, std::string_view return_type, std::vector<std::pair<std::string_view, std::string_view>> parameters, std::unique_ptr<Expr> body);
        ~FuncDefExpr();
    };

    struct FuncCallExpr {
        std::string_view ident;
        std::vector<std::unique_ptr<Expr>> args;

        FuncCallExpr(std::string_view ident, std::vector<std::unique_ptr<Expr>> args);
        ~FuncCallExpr();
    };

    struct ReturnExpr {
        std::unique_ptr<Expr> value;

        ReturnExpr(std::unique_ptr<Expr> value);
        ~ReturnExpr();
    };

    struct WhileExpr {
        std::unique_ptr<Expr> cond;
        std::unique_ptr<Expr> body;

        WhileExpr(std::unique_ptr<Expr> cond, std::unique_ptr<Expr> body);
        ~WhileExpr();
    };

    struct ForExpr {
        std::unique_ptr<Expr> init, cond, update, body;

        ForExpr(std::unique_ptr<Expr> init, std::unique_ptr<Expr> cond, std::unique_ptr<Expr> update, std::unique_ptr<Expr> body);
        ~ForExpr();
    };

    struct ArrayIndexingExpr {
        std::string_view array;
        std::unique_ptr<Expr> index;

        ArrayIndexingExpr(std::string_view array, std::unique_ptr<Expr> index);
        ~ArrayIndexingExpr();
    };

    struct Expr {
        std::variant<
            IntegerLiteralExpr,
            FloatLiteralExpr,
            CharLiteralExpr,
            StringLiteralExpr,
            BooleanExpr,
            UnaryExpr,
            BinaryExpr,
            VariableExpr,
            VariableDeclExpr,
            IfExpr
            // AssignExpr,
            // CompoundExpr,
            // FuncDeclExpr,
            // FuncDefExpr,
            // FuncCallExpr,
            // ReturnExpr,
            // WhileExpr,
            // ForExpr,
            // ArrayIndexingExpr,
        > kind;

        template <typename T, typename... Args>
        Expr(std::in_place_type_t<T>, Args&&... args) : 
            kind{std::in_place_type<T>, std::forward<Args>(args)...} {}

        // add other constructors

        ~Expr() = default;

        Expr(const Expr&) = delete;
        Expr& operator=(const Expr&) = delete;
        Expr(Expr&&) noexcept = delete;
        Expr& operator=(Expr&&) noexcept = delete;
    };

    template <typename... Ts>
    struct overloaded : Ts... { using Ts::operator()...; };

    // pretty printing
    [[nodiscard]] auto inline to_string(const Expr& expr) -> std::string {
        return std::visit(overloaded{
            [](const IntegerLiteralExpr& kind) { return std::to_string(kind.value); },
            [](const FloatLiteralExpr& kind) { return std::to_string(kind.value); },
            [](const CharLiteralExpr kind) { return std::string{kind.value}; },
            [](const StringLiteralExpr& kind) { return std::string{kind.value}; },
            [](const BooleanExpr& kind) { return std::format("boolean: {}", (kind.value == true) ? "true" : "false"); },
            [](const UnaryExpr& kind) { return std::format("UnaryExpr: {}\n  {}", kind.op, AST::to_string(*kind.arg)); },
            [](const BinaryExpr& kind) { return std::format("BinaryExpr: {}\n  {}\n  {}\n", kind.op, AST::to_string(*kind.lhs), AST::to_string(*kind.rhs)); },
            [](const VariableExpr& kind) { return std::format("Identifier: {}", std::string{kind.ident}); },
            [](const VariableDeclExpr& kind) { return std::format("VarDecl: {} <{}>", std::string{kind.ident}, std::string{kind.type}); },
            [](const IfExpr& kind) { 
                std::string s{"IfStatement:\n  " + AST::to_string(*kind.cond)};
                for (const auto& expr : kind.if_else_exprs) {
                    s += std::format("{}", AST::to_string(*expr));
                }
                return s;
            },
            // [](const AssignExpr& kind) {},
            // [](const CompoundExpr& kind) {},
            // [](const FuncDeclExpr& kind) {},
            // [](const FuncDefExpr& kind) {},
            // [](const FuncCallExpr& kind) {},
            // [](const ReturnExpr& kind) {},
            // [](const WhileExpr& kind) {},
            // [](const ForExpr& kind) {},
            // [](const ArrayIndexingExpr& kind) {},
        }, expr.kind);
    }

}
