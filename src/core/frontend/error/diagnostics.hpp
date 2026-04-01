#pragma once

#include <vector>

#include "warning.hpp"
#include "error.hpp"

// each error query is potentially terminating the program

struct Diagnostics
{
public:
    void register_error(const Error& error);
    void register_warning(const Warning& warning);
    void dump_errors() const;
    void dump_warnings() const;
    bool contains_errors() const ;
    bool contains_warnings() const ;

private:
    std::vector<Error> errors_;
    std::vector<Warning> warnings_;
};