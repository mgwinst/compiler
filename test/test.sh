#!/bin/bash

clang++ -std=c++23 \
    -I../src/core \
    test_main.cpp \
    lexer/test_lexer.cpp \
    parser/test_functions.cpp \
    parser/test_variables.cpp \
    ../build/debug/libwinc_lib.a \
    && ./a.out
