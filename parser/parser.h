#pragma once

#include "ast.h"

struct Parser {
    Lexer lexer;
    
    Parser(std::string source_text) : lexer{source_text} {}

};
