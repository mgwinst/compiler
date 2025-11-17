#include "parser/error.hpp"

void ParseDiagnostics::add_error(const ParseError& error)
{
    errors.push_back(error);

    if (errors.size() >= 20) {
        dump_errors();
        std::println("too many errors emitted, stopping now");
        exit(1);
    }
}

// handle warning counts with -W flags later...
void ParseDiagnostics::add_warning(const ParseWarning& warning)
{
    warnings.push_back(warning);
}

void ParseDiagnostics::dump_errors()
{
    int count = 1;
    for (const auto& error : errors) {
        std::println("{}: {}", count++, error);
    }
}

void ParseDiagnostics::dump_warnings()
{
    int count = 1;
    for (const auto& warning : warnings) {
        std::println("{}: {}", count++, warning);
    }
}