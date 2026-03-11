#include "diagnostics.hpp"

void Diagnostics::register_error(const Error& error)
{
    errors_.push_back(error);

    if (errors_.size() >= 20) {
        dump_errors();
        std::println("too many errors emitted, stopping now");
        exit(1);
    }
}

// handle warning counts with -W flags later...
void Diagnostics::add_warning(const Warning& warning)
{
    warnings_.push_back(warning);
}

void Diagnostics::dump_errors()
{
    for (const auto& error : errors_) {
        std::println("{}", error_to_string(error));
    }
}

void Diagnostics::dump_warnings()
{
    for (const auto& warning : warnings_) {
        std::println("{}", warning_to_string(warning));
    }
}