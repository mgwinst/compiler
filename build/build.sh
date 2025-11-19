#!/bin/bash

export COMPILER_HOME=~/compiler

clang++ -std=c++23 -g -I$COMPILER_HOME -I$COMPILER_HOME/core \
    ../core/lexer/lexer.cpp ../core/parser/parser.cpp ../core/parser/ast.cpp ../core/parser/error.cpp \
    main.cpp && ./a.out
