#!/bin/bash

clang++ -std=c++23 \
    -I../src/core \
    -I/opt/homebrew/include \
    test_main.cpp \
    lexer/test_lexer.cpp \
    parser/test_declarations.cpp \
    parser/test_statements.cpp \
    parser/test_expressions.cpp \
    ../build/debug/libwinc_lib.a \
    && ./a.out
