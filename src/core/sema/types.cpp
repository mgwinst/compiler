#include <functional>

#include "sema/types.hpp"

std::optional<std::string_view> find_built_in(std::string_view type_str)
{
    static constexpr std::array built_ins = {
        "int"sv, "int8"sv, "int16"sv, "int32"sv, "int64"sv,
        "uint"sv, "uint8"sv, "uint16"sv, "uint32"sv, "uint64"sv,
        "float"sv, "float16"sv, "float32"sv, "float64"sv,
        "byte"sv, "bool"sv, "void"sv
    };

    auto it = std::ranges::find_if(built_ins, [&](std::string_view str) {
        return type_str.contains(str);
    });

    if (it != built_ins.end())
        return *it;

    return std::nullopt;
}

auto TypePool::intern_type(std::string_view type_str) -> TypeRef
{
    return parse_type(type_str);
}

auto TypePool::lookup_type(std::string_view type_str) -> std::optional<TypeRef>
{
    if (auto it = type_to_index_.find(type_str.data()); it != type_to_index_.end())
        return it->second;
    return std::nullopt;
}

auto TypePool::parse_type(std::string_view type_str) -> TypeRef
{
    if (auto type = lookup_type(type_str); type)
        return *type;

    switch (type_str.back()) {
        case (']'):
            auto pos = type_str.find_first_of('[');
            return create_type<ArrayType>(type_str, 0, parse_type(type_str.substr(0, pos))); // fix the size issue

        case ('*'):
            return create_type<PointerType>(type_str, parse_type(type_str.substr(0, type_str.length() - 1)));

        case ('&'):
            return create_type<ReferenceType>(type_str, parse_type(type_str.substr(0, type_str.length() - 1)));
    }

    auto match_prefix = [&](std::string_view prefix) -> std::optional<std::string_view> {
        if (type_str.starts_with(prefix))
            return type_str.substr(prefix.size());
        return std::nullopt;
    };

    if (auto rest = match_prefix("const "sv))
        return create_type<QualType>(type_str, QualType::QualKind::Const, parse_type(type_str.substr(6)));

    if (auto rest = match_prefix("struct "sv)) // you need to capture the struct identifier too! 
        return create_type<StructType>(type_str);

    if (auto rest = match_prefix("enum "sv))
        return create_type<EnumType>(type_str);

    if (auto rest = match_prefix("union "sv))
        return create_type<UnionType>(type_str);

    static std::unordered_map<std::string_view, std::function<TypeRef()>> builtin_factory = {
        {"int"sv,     [&]() { return create_type<IntegerType>(type_str, 32, true); }},
        {"int8"sv,    [&]() { return create_type<IntegerType>(type_str, 8, true); }},
        {"int16"sv,   [&]() { return create_type<IntegerType>(type_str, 16, true); }},
        {"int32"sv,   [&]() { return create_type<IntegerType>(type_str, 32, true); }},
        {"int64"sv,   [&]() { return create_type<IntegerType>(type_str, 64, true); }},
        {"uint"sv,    [&]() { return create_type<IntegerType>(type_str, 32, false); }},
        {"uint8"sv,   [&]() { return create_type<IntegerType>(type_str, 8, false); }},
        {"uint16"sv,  [&]() { return create_type<IntegerType>(type_str, 16, false); }},
        {"uint32"sv,  [&]() { return create_type<IntegerType>(type_str, 32, false); }},
        {"uint64"sv,  [&]() { return create_type<IntegerType>(type_str, 64, false); }},
        {"float"sv,   [&]() { return create_type<FloatType>(type_str, 32, false); }},
        {"float16"sv, [&]() { return create_type<FloatType>(type_str, 16, false); }},
        {"float32"sv, [&]() { return create_type<FloatType>(type_str, 32, false); }},
        {"float64"sv, [&]() { return create_type<FloatType>(type_str, 64, false); }}
    };

    if (auto built_in = find_built_in(type_str); built_in)
        return builtin_factory[*built_in]();
}
