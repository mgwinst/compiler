#!/bin/bash
clang++ -std=c++23 ast.cc ast_nodes.cc -o ast && ./ast
