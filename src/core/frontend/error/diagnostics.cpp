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
void Diagnostics::register_warning(const Warning& warning)
{
    warnings_.push_back(warning);
}

void Diagnostics::dump_errors() const
{
    for (const auto& error : errors_)
        std::println("{}", error_to_string(error));
    std::println("{} error(s) generated", errors_.size());
}

void Diagnostics::dump_warnings() const
{
    for (const auto& warning : warnings_) {
        std::println("{}", warning_to_string(warning));
    }
}

bool Diagnostics::contains_errors() const { return errors_.size() > 0; }
bool Diagnostics::contains_warnings() const { return warnings_.size() > 0; }

// since diagnostics is unique to module -> add flush mechanism for warnings