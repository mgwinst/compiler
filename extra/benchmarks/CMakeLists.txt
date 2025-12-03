cmake_minimum_required(VERSION 4.0)

find_package(benchmark QUIET)

if(NOT benchmark_FOUND)
    message(STATUS "Google Benchmark not found -> skipping benchmarks. Install Google Benchmark")
    return()
endif()

add_executable(compiler_benchmarks PRIVATE
    # add benchmark.cpp files here
)

target_link_libraries(compiler_benchmarks PRIVATE
    compiler_lib
    benchmark::benchmark_main
)

# finish this config once we start benchmarking