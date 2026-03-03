#pragma once

#include <cstdint>

enum class ASTNodeKind : uint8_t 
{
    CompilationUnitDecl,
    VarDecl,
    ParamDecl,
    FuncDecl,
    RecordDecl,

    CompoundStmt,
    ReturnStmt,
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
    NewExpr,
    DeleteExpr,

    QualifierTypeExpr,
    PointerTypeExpr,
    ReferenceTypeExpr,
    ArrayTypeExpr,
    NamedTypeExpr,

    Invalid
};

enum class SemaNodeKind : uint8_t 
{
    CompilationUnitDecl,
    VarDecl,
    ParamDecl,
    FuncDecl,
    RecordDecl,

    CompoundStmt,
    ReturnStmt,
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
    NewExpr,
    DeleteExpr,
    Invalid
};

enum class TypeKind : uint8_t
{
    Void,
    Byte,
    Bool,
    Integer,
    Float,
    Reference,
    Pointer,
    Array,
    Qualifier,
    Function,
    Record,
    Invalid
};

enum class RecordKind 
{ 
    Struct, 
    Union, 
    Enum,
    Unknown 
};

enum class QualifierKind 
{
    Const
};

enum class SymbolKind
{

};

enum class StorageClass
{
    Auto,
    Static,
};

enum class Linkage
{
    Internal,
    External,
    Weak,
    None
};

