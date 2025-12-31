#include <functional>

#include "type_pool.hpp"

std::optional<std::string_view> find_built_in(std::string_view type_str)
{
    static constexpr std::array built_ins = {
        "int"sv, "int8"sv, "int16"sv, "int32"sv, "int64"sv,
        "uint"sv, "uint8"sv, "uint16"sv, "uint32"sv, "uint64"sv,
        "float"sv, "float16"sv, "float32"sv, "float64"sv,
        "byte"sv, "bool"sv, "void"sv
    };

    auto trim = [](std::string_view sv) {
        auto start = sv.find_first_not_of(" \t");
        if (start == sv.npos) return std::string_view{};
        auto end = sv.find_last_not_of(" \t");
        return sv.substr(start, end - start + 1);
    };

    type_str = trim(type_str);

    for (auto built_in : built_ins) {
        if (type_str == built_in)
            return built_in;
    }

    return std::nullopt;
}

namespace Sema
{
    TypePool::TypePool()
    {
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

        types_.emplace_back(std::in_place_type<FloatType>, uint16_t{ 16 }, true);
        types_.emplace_back(std::in_place_type<FloatType>, uint16_t{ 32 }, true);
        types_.emplace_back(std::in_place_type<FloatType>, uint16_t{ 64 }, true);
    }

    TypeRef TypePool::intern_type(std::string_view type_str)
    {
        return parse_type(type_str);
    }

    TypeRef TypePool::parse_type(std::string_view type_str) noexcept
    {
        auto match_prefix = [&](std::string_view prefix) -> std::optional<std::string_view> {
            if (type_str.starts_with(prefix))
                return type_str.substr(prefix.size());
            return std::nullopt;
        };

        if (auto rest = match_prefix("const "sv))
            return get_or_create<QualifierType>(QualifierType::QualKind::Const, parse_type(*rest));

        switch (type_str.back()) {
            case (']'): {
                auto pos = type_str.find_first_of('[');
                return get_or_create<ArrayType>(NodeRef{ 0 }, parse_type(type_str.substr(0, pos))); // fix the size issue
            }

            case ('*'):
                return get_or_create<PointerType>(parse_type(type_str.substr(0, type_str.length() - 1)));

            case ('&'):
                return get_or_create<ReferenceType>(parse_type(type_str.substr(0, type_str.length() - 1)));

            default:
                break;
        }

        if (auto rest = match_prefix("struct "sv)) // you need to capture the struct identifier too! 
            return 10000;

        if (auto rest = match_prefix("enum "sv))
            return 10000;

        if (auto rest = match_prefix("union "sv))
            return 10000;

        static std::unordered_map<std::string_view, std::function<TypeRef()>> builtin_factory = {
            {"byte"sv,    [] { return INT32_INDEX;   }},
            {"bool"sv,    [] { return INT32_INDEX;   }},
            {"int"sv,     [] { return INT32_INDEX;   }},
            {"int8"sv,    [] { return INT8_INDEX;    }},
            {"int16"sv,   [] { return INT16_INDEX;   }},
            {"int32"sv,   [] { return INT32_INDEX;   }},
            {"int64"sv,   [] { return INT64_INDEX;   }},
            {"uint"sv,    [] { return UINT32_INDEX;  }},
            {"uint8"sv,   [] { return UINT8_INDEX;   }},
            {"uint16"sv,  [] { return UINT16_INDEX;  }},
            {"uint32"sv,  [] { return UINT32_INDEX;  }},
            {"uint64"sv,  [] { return UINT64_INDEX;  }},
            {"float"sv,   [] { return FLOAT32_INDEX; }},
            {"float16"sv, [] { return FLOAT16_INDEX; }},
            {"float32"sv, [] { return FLOAT32_INDEX; }},
            {"float64"sv, [] { return FLOAT64_INDEX; }}
        };

        if (auto built_in = find_built_in(type_str); built_in)
            return builtin_factory[*built_in]();
    }

} // namespace Sema

