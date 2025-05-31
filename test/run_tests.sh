#!/bin/bash
clang++ -std=c++23 test_main.cc -I../ ../lexer/lexer.cc test_lexer.cc \
    -lgtest -lgtest_main && ./a.out