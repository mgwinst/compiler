#!/bin/bash

clang++ -std=c++23 \
    -I../src/core \
    test_main.cpp \
    lexer/test_lexer.cpp \
    ../src/core/frontend/lexer/lexer.cpp \
    && ./a.out
