#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>

enum class ASTNodeKind : uint8_t 
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
    NewExpr,
    DeleteExpr,
    Invalid
};

enum class TypeKind : uint8_t
{
    Error,
    Void,
    Byte,
    Char,
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

enum class ImplicitCastKind
{
    IntToFloat,
    FloatToInt,

    Widening,
    Narrowing,

    SignedToUnsigned,
    UnsignedToSigned,
    
    PointerToBoolean,
    IntegralToBoolean,

    ArrayToPointerDecay,
    FunctionToPointerDecay,

    BitCast
};

enum class BinaryOp
{
    Assign,
    Add,
    Sub,
    Mul,
    Div,
    Rem,
    Xor,
    Or,
    And,
    Shl,
    Shr,
    Eq,
    Ne,
    Slt,
};

inline std::unordered_map<std::string, BinaryOp> binary_ops = {
    {"=", BinaryOp::Assign},
    {"+", BinaryOp::Add},
    {"-", BinaryOp::Sub},
    {"*", BinaryOp::Mul},
    {"/", BinaryOp::Div},
    {"%", BinaryOp::Rem},
    {"^", BinaryOp::Xor},
    {"|", BinaryOp::Or},
    {"&", BinaryOp::And},
    {"<<", BinaryOp::Shl},
    {">>", BinaryOp::Shr},
    {"==", BinaryOp::Eq},
    {"!=", BinaryOp::Ne},
    {"<", BinaryOp::Slt},
};

enum class TerminatorKind
{
    Return,
    Branch,
    Switch
};