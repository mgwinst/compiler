#!/bin/bash
export COMPILER_HOME=/Users/matteospatola/compiler
clang++ -std=c++23 test_main.cc -I$COMPILER_HOME ../lexer/lexer.cc test_lexer.cc \
    -lgtest -lgtest_main && ./a.out
