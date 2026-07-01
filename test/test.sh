#!/bin/bash

clang++ -std=c++23 \
    -I../src/core \
    test_main.cpp \
    lexer/test_lexer.cpp \
    lowering/test_lowering.cpp \
    ../build/debug/libwinc_lib.a \
    && ./a.out
