#pragma once

#include <vector>

#include "warning.hpp"
#include "error.hpp"

struct Diagnostics
{
    std::vector<Error> errors_;
    std::vector<Warning> warnings_;

    void add_error(const Error& error);
    void add_warning(const Warning& warning);

    void dump_errors();
    void dump_warnings();
};