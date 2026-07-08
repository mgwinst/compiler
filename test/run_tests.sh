#!/bin/bash

clang++ -std=c++23 \
    -I./ \
    -I../src/core \
    -I/opt/homebrew/include \
    test_main.cpp \
    lexer/test_lexer.cpp \
    parser/declarations.cpp \
    parser/statements.cpp \
    parser/expressions.cpp \
    sema/type_interning.cpp \
    sema/type_checker/type_checker.cpp \
    ../build/debug/libwinc_lib.a \
    && ./a.out
