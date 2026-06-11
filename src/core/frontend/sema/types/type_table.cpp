#include "type_table.hpp"
#include "utils/casting.hpp"

TypeTable::TypeTable()
{
    // error type is not included in this build, probably can just use a nullptr to signal error type

    builtin_map_.emplace("void", arena.emplace<VoidType>());
    builtin_map_.emplace("byte", arena.emplace<ByteType>());
    builtin_map_.emplace("char", arena.emplace<CharType>());
    builtin_map_.emplace("bool", arena.emplace<BoolType>());

    builtin_map_.emplace("int8", arena.emplace<IntegerType>(uint16_t{ 8 }, true));
    builtin_map_.emplace("int16", arena.emplace<IntegerType>(uint16_t{ 16 }, true));
    builtin_map_.emplace("int32", arena.emplace<IntegerType>(uint16_t{ 32 }, true));
    builtin_map_.emplace("int", builtin_map_["int32"]);
    builtin_map_.emplace("int64", arena.emplace<IntegerType>(uint16_t{ 64 }, true));

    builtin_map_.emplace("uint8", arena.emplace<IntegerType>(uint16_t{ 8 }, false));
    builtin_map_.emplace("uint16", arena.emplace<IntegerType>(uint16_t{ 16 }, false));
    builtin_map_.emplace("uint32", arena.emplace<IntegerType>(uint16_t{ 32 }, true));
    builtin_map_.emplace("uint", builtin_map_["uint32"]);
    builtin_map_.emplace("uint32", arena.emplace<IntegerType>(uint16_t{ 32 }, false));
    builtin_map_.emplace("uint64", arena.emplace<IntegerType>(uint16_t{ 64 }, false));

    builtin_map_.emplace("float16", arena.emplace<FloatType>(uint16_t{ 16 }));
    builtin_map_.emplace("float32", arena.emplace<FloatType>(uint16_t{ 32 }));
    builtin_map_.emplace("float", builtin_map_["float32"]);
    builtin_map_.emplace("float64", arena.emplace<FloatType>(uint16_t{ 64 }));
}

using namespace Syntax;

Type* TypeTable::resolve_type(ASTNode* type_expr)
{
    switch (type_expr->kind_) {
        case ASTNodeKind::QualifierTypeExpr: {
            auto* qual = cast<QualifierTypeExpr>(type_expr);

            if (qual->kind_ == QualifierKind::Const) {
                return get_or_create<QualifierType>(QualifierKind::Const, resolve_type(qual->inner_));
            }

            error_exit("qualifier not supported");
        }

        case ASTNodeKind::PointerTypeExpr: {
            auto* ptr = cast<PointerTypeExpr>(type_expr);
            return get_or_create<PointerType>(resolve_type(ptr->inner_));
        }

        case ASTNodeKind::ArrayTypeExpr: {
            auto* arr = cast<ArrayTypeExpr>(type_expr);

            if (!arr->size_)
                error_exit("array must have a size");

            if (auto* size = dyn_cast<IntegerLiteralExpr>(arr->size_)) {
                return get_or_create<ArrayType>(resolve_type(arr->inner_), static_cast<uint64_t>(size->value_));
            } else {
                error_exit("array size must be Integer constant");
            }
        }

        case ASTNodeKind::NamedTypeExpr: {
            auto* named = cast<NamedTypeExpr>(type_expr);

            if (auto it = builtin_map_.find(named->name_); it != builtin_map_.end())
                return it->second;

            if (auto it = user_defined_map_.find(named->name_); it != user_defined_map_.end())
                return it->second;

            error_exit("type does not exist");
        }

        case ASTNodeKind::FuncDecl: {
            auto* func = cast<FuncDecl>(type_expr);

            std::vector<Type*> param_types;
            for (auto* param : static_cast_view<ParamDecl>(func->params_)) {
                param_types.push_back(resolve_type(param->type_expr_));
            }

            auto* ret_type = resolve_type(func->return_type_);
            
            return get_or_create<FunctionType>(func->name_, param_types, ret_type);
        }

        case ASTNodeKind::RecordDecl: {
            auto* record = cast<RecordDecl>(type_expr);
            
            std::vector<Field> fields;
            for (auto* field : static_cast_view<VarDecl>(record->fields_)) {
                Type* field_type = resolve_type(field->type_expr_);
                fields.emplace_back(field->name_, field_type);
            }

            auto* record_type = get_or_create<RecordType>(record->kind_, record->name_);
            cast<RecordType>(record_type)->fields_ = std::move(fields);

            user_defined_map_.emplace(record->name_, record_type);
            return record_type;
        }

        default:
            error_exit("type mismatch during type resolution");
    }
}