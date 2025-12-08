#!/bin/bash

if [ ! -d ./build/debug ]; then
    mkdir -p build/debug
fi

CXX=clang++ cmake -B build/debug -S . -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DENABLE_TESTS=OFF \
    -DENABLE_BENCHMARKS=OFF \
    -DCMAKE_COLOR_DIAGNOSTICS=ON
