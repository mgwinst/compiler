#!/bin/bash

export COMPILER_HOME=~/compiler

clang++ -std=c++23 -g -I$COMPILER_HOME ../lexer/lexer.cc ../parser/parser.cc ../parser/ast.cc \
        main.cc && ./a.out


