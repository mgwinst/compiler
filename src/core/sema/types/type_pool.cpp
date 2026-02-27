#include <algorithm>
#include <string_view>

#include "type_pool.hpp"
#include "types.hpp"

static std::unordered_map<std::string_view, TypeRef> builtin_map = {
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

namespace Sema
{
    TypeRef TypePool::resolve_type(const ASTNodeRef type_expr, const AST& ast) noexcept
    {
        const auto& node = ast.nodes_[type_expr];
        
        switch (node.get_kind()) {
            case ASTNodeKind::QualifierTypeExpr: {
                const auto& qual = node.as<SyntaxTree::QualifierTypeExpr>();
                // if (qual.qualifier_ == SyntaxTree::QualifierTypeExpr::QualType::Const)
                    return get_or_create<QualifierType>(QualifierType::QualKind::Const, resolve_type(qual.inner_, ast)); // only qualifier right now
            }

            case ASTNodeKind::PointerTypeExpr: {
                const auto& ptr = node.as<SyntaxTree::PointerTypeExpr>();
                return get_or_create<PointerType>(resolve_type(ptr.inner_, ast));
            }

            case ASTNodeKind::ReferenceTypeExpr: {
                const auto& ref = node.as<SyntaxTree::ReferenceTypeExpr>();
                return get_or_create<ReferenceType>(resolve_type(ref.inner_, ast));
            }

            // size might be wrong here
            case ASTNodeKind::ArrayTypeExpr: {
                const auto& arr = node.as<SyntaxTree::ArrayTypeExpr>();

                // temporary
                if (!arr.size_)
                    error_exit("array must have a size");

                return get_or_create<ArrayType>(resolve_type(arr.inner_, ast), *arr.size_);
            }

            case ASTNodeKind::NamedTypeExpr: {
                const auto& named = node.as<SyntaxTree::NamedTypeExpr>();

                if (auto it = builtin_map.find(named.name_); it != builtin_map.end())
                    return it->second;

                return get_or_create<RecordType>(named.name_);
            }

            // should this be here??
            case ASTNodeKind::FuncDecl: {
                const auto& func = node.as<SyntaxTree::FuncDecl>();

                auto ret_type = resolve_type(func.return_type_, ast);
                
                return get_or_create<FunctionType>(func.name_, ret_type);
            }

            default: {
                std::println("{}", (int)node.get_kind());
                error_exit("type mismatch during type resolution");

            }
        }
    }

} // namespace Sema