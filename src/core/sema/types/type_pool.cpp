#include <string_view>

#include "type_pool.hpp"
#include "types.hpp"
#include "../../utils/print/print.hpp"

#define NODE_LIMIT (1 << 18)

namespace
{
    std::unordered_map<std::string_view, TypeRef> builtin_map = {
        {"void",    VOID_INDEX   },
        {"byte",    BYTE_INDEX   },
        {"bool",    BOOL_INDEX   },
        {"int8",    INT8_INDEX   },
        {"int16",   INT16_INDEX  },
        {"int32",   INT32_INDEX  },
        {"int",     INT32_INDEX  },
        {"int64",   INT64_INDEX  },
        {"uint8",   UINT8_INDEX  },
        {"uint16",  UINT16_INDEX },
        {"uint32",  UINT32_INDEX },
        {"uint",    UINT32_INDEX },
        {"uint64",  UINT64_INDEX },
        {"float16", FLOAT16_INDEX},
        {"float32", FLOAT32_INDEX},
        {"float",   FLOAT32_INDEX},
        {"float64", FLOAT64_INDEX}
    };
}

namespace Sema
{
    TypePool::TypePool()
    {
        types_.reserve(NODE_LIMIT);

        types_.emplace_back(std::in_place_type<VoidType>);
        types_.emplace_back(std::in_place_type<ByteType>);
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

    TypeRef TypePool::resolve_type(const ASTNodeRef type_expr, const AST& ast) noexcept
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

                return get_or_create<RecordType>(RecordKind::Unknown, named.name_); // resolves during type checking (we shouldn't know its struct/enum/union yet, check the symbol table)
            }

            case ASTNodeKind::FuncDecl: {
                const auto& func = node.as<Syntax::FuncDecl>();

                auto ret_type = resolve_type(func.return_type_, ast);
                
                return get_or_create<FunctionType>(func.name_, ret_type);
            }

            case ASTNodeKind::RecordDecl: {
                const auto& rec = node.as<Syntax::RecordDecl>();

                switch (rec.kind_) {
                    case RecordKind::Struct: {
                        return get_or_create<RecordType>(RecordKind::Struct, rec.name_);
                    }

                    case RecordKind::Union: {
                        return get_or_create<RecordType>(RecordKind::Union, rec.name_);
                    }

                    case RecordKind::Enum: {
                        return get_or_create<RecordType>(RecordKind::Enum, rec.name_);
                    }

                    default:
                        error_exit("Record type must be struct/union/enum");
                }
            }

            default: {
                error_exit("type mismatch during type resolution");
            }
        }
    }

    void TypePool::print() const noexcept
    {
        for (auto i = 0uz; i < types_.size(); i++) {
            std::println("{} [{}]", type_to_str(*this, i), i);
        }
    }

} // namespace Sema