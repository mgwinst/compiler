#pragma once

enum class RecordKind 
{ 
    Struct, 
    Union, 
    Enum,
};

enum class QualifierKind
{
    Const
};

enum class BinaryOp
{
    Invalid,
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



