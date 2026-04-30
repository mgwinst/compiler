#include <string_view>

#include "type_pool.hpp"
#include "types.hpp"

namespace {

std::unordered_map<std::string_view, TypeID> builtin_map = {
    {"void",    VOID   },
    {"byte",    BYTE   },
    {"char",    CHAR   },
    {"bool",    BOOL   },
    {"int8",    INT8   },
    {"int16",   INT16  },
    {"int32",   INT32  },
    {"int",     INT32  },
    {"int64",   INT64  },
    {"uint8",   UINT8  },
    {"uint16",  UINT16 },
    {"uint32",  UINT32 },
    {"uint",    UINT32 },
    {"uint64",  UINT64 },
    {"float16", FLOAT16},
    {"float32", FLOAT32},
    {"float",   FLOAT32},
    {"float64", FLOAT64}
};

}

TypePool::TypePool() noexcept
{
    types_.emplace_back(std::in_place_type<ErrorType>);
    types_.emplace_back(std::in_place_type<VoidType>);
    types_.emplace_back(std::in_place_type<ByteType>);
    types_.emplace_back(std::in_place_type<CharType>);
    types_.emplace_back(std::in_place_type<BoolType>);

    types_.emplace_back(std::in_place_type<IntegerType>, uint16_t{ 8 }, true);
    types_.emplace_back(std::in_place_type<IntegerType>, uint16_t{ 16 }, true);
    types_.emplace_back(std::in_place_type<IntegerType>, uint16_t{ 32 }, true);
    types_.emplace_back(std::in_place_type<IntegerType>, uint16_t{ 64 }, true);

    types_.emplace_back(std::in_place_type<IntegerType>, uint16_t{ 8 }, false);
    types_.emplace_back(std::in_place_type<IntegerType>, uint16_t{ 16 }, false);
    types_.emplace_back(std::in_place_type<IntegerType>, uint16_t{ 32 }, false);
    types_.emplace_back(std::in_place_type<IntegerType>, uint16_t{ 64 }, false);

    types_.emplace_back(std::in_place_type<FloatType>, uint16_t{ 16 });
    types_.emplace_back(std::in_place_type<FloatType>, uint16_t{ 32 });
    types_.emplace_back(std::in_place_type<FloatType>, uint16_t{ 64 });
}

const Type& TypePool::get_type(TypeID ref) const noexcept { return types_[ref]; }
Type& TypePool::get_type(TypeID ref) noexcept { return types_[ref]; }

TypeID TypePool::resolve_type(const ASTNodeID type_expr, const AST& ast) noexcept
{
    const auto& node = ast.nodes_[type_expr];
    
    switch (node.get_kind()) {
        case ASTNodeKind::QualifierTypeExpr: {
            const auto& qual = node.as<Syntax::QualifierTypeExpr>();
            switch(qual.kind_) {
                case QualifierKind::Const: {
                    return get_or_create<QualifierType>(QualifierKind::Const, resolve_type(qual.inner_, ast));
                }

                default:
                    error_exit("qualifier not supported");
            }
        }

        case ASTNodeKind::PointerTypeExpr: {
            const auto& ptr = node.as<Syntax::PointerTypeExpr>();
            return get_or_create<PointerType>(resolve_type(ptr.inner_, ast));
        }

        case ASTNodeKind::ReferenceTypeExpr: {
            const auto& ref = node.as<Syntax::ReferenceTypeExpr>();
            return get_or_create<ReferenceType>(resolve_type(ref.inner_, ast));
        }

        // size might be wrong here
        case ASTNodeKind::ArrayTypeExpr: {
            const auto& arr = node.as<Syntax::ArrayTypeExpr>();

            // temporary hack
            if (!arr.size_)
                error_exit("array must have a size");

            return get_or_create<ArrayType>(resolve_type(arr.inner_, ast), *arr.size_);
        }

        case ASTNodeKind::NamedTypeExpr: {
            const auto& named = node.as<Syntax::NamedTypeExpr>();

            if (auto it = builtin_map.find(named.name_); it != builtin_map.end())
                return it->second;

            if (auto it = user_defined_map_.find(named.name_); it != user_defined_map_.end())
                return it->second;

            error_exit("type does not exist");
        }

        case ASTNodeKind::FuncDecl: {
            const auto& func = node.as<Syntax::FuncDecl>();

            std::vector<TypeID> param_types;
            for (auto p : func.params_) {
                auto param = ast.nodes_[p].as<Syntax::ParamDecl>().type_expr_;
                param_types.push_back(resolve_type(param, ast));
            }

            auto ret_type = resolve_type(func.return_type_, ast);
            
            return get_or_create<FunctionType>(func.name_, param_types, ret_type);
        }

        case ASTNodeKind::RecordDecl: {
            const auto& rec = node.as<Syntax::RecordDecl>();
            
            std::vector<Field> fields;
            for (auto f : rec.fields_) {
                auto& field_node = ast.nodes_[f].as<Syntax::VarDecl>();
                auto field_type_id = resolve_type(field_node.type_expr_, ast);
                fields.push_back(Field{field_node.name_, field_type_id});
            }

            auto type_id = get_or_create<RecordType>(rec.kind_, rec.name_);
            types_[type_id].as<RecordType>().fields_ = std::move(fields);

            user_defined_map_.emplace(rec.name_, type_id);
            return type_id;
        }

        default:
            error_exit("type mismatch during type resolution");
    }
}