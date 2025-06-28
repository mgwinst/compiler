#!/bin/bash

export COMPILER_HOME=/Users/matteospatola/compiler/
clang++ -std=c++23 -g -I$COMPILER_HOME ../lexer/lexer.cc ../parser/parser.cc ../parser/ast_nodes.cc \
    main.cc -o main

