#include <string_view>

#include "type_pool.hpp"
#include "types.hpp"

namespace
{
    std::unordered_map<std::string_view, TypeId> builtin_map = {
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

namespace Sema
{
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

    const Type& TypePool::get_type(TypeId ref) const noexcept { return types_[ref]; }
    Type& TypePool::get_type(TypeId ref) noexcept { return types_[ref]; }

    TypeId TypePool::resolve_type(const ASTNodeId type_expr, const AST& ast) noexcept
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

                if (auto it = user_def_type_map.find(named.name_); it != user_def_type_map.end())
                    return it->second;

                error_exit("type does not exist");
            }

            case ASTNodeKind::FuncDecl: {
                const auto& func = node.as<Syntax::FuncDecl>();

                std::vector<TypeId> param_types;
                for (auto p : func.params_) {
                    auto param = ast.nodes_[p].as<Syntax::ParamDecl>().type_expr_;
                    param_types.push_back(resolve_type(param, ast));
                }

                auto ret_type = resolve_type(func.return_type_, ast);
                
                return get_or_create<FunctionType>(func.name_, param_types, ret_type);
            }

            case ASTNodeKind::RecordDecl: {
                const auto& rec = node.as<Syntax::RecordDecl>();

                TypeId id;

                switch (rec.kind_) {
                    case RecordKind::Struct: {
                        id = get_or_create<RecordType>(RecordKind::Struct, rec.name_);
                        break;
                    }

                    case RecordKind::Union: {
                        id = get_or_create<RecordType>(RecordKind::Union, rec.name_);
                        break;
                    }

                    case RecordKind::Enum: {
                        id = get_or_create<RecordType>(RecordKind::Enum, rec.name_);
                        break;
                    }

                    default:
                        error_exit("Record type must be struct/union/enum");
                }

                user_def_type_map.emplace(rec.name_, id);
                return id;
            }

            default: {
                error_exit("type mismatch during type resolution");
            }
        }
    }

} // namespace Sema