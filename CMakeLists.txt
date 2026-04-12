cmake_minimum_required(VERSION 4.0)

project(compiler VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # don't silently fall back to older standard
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED)

add_library(compiler_lib STATIC
    src/core/driver/pipeline.cpp
    src/core/frontend/lexer/lexer.cpp 
    src/core/frontend/parser/parser.cpp 
    src/core/frontend/error/diagnostics.cpp
    src/core/frontend/sema/types/type_pool.cpp
    src/core/frontend/sema/types/type_checker.cpp
    src/core/frontend/sema/sema_tree_builder.cpp
    src/core/frontend/sema/symbol.cpp
    src/core/frontend/sema/desugar.cpp
    src/core/lowering/LoweringEngine.cpp
    src/core/utils/print/print.cpp
)

target_include_directories(compiler_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
)

target_link_libraries(compiler_lib PUBLIC Boost::boost)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE compiler_lib)

target_compile_options(${PROJECT_NAME} PRIVATE
    -march=native -fcolor-diagnostics -Wall
    -Wextra -Wpedantic -Wconversion -Wsign-conversion
    -Wshadow -Wundef -Werror=return-type # -Werror
)

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:
        -O0 -g3
        -fno-omit-frame-pointer
        -fsanitize=address,undefined #leak
        -fsanitize-address-use-after-scope
        -fno-sanitize-recover=all
    >
    $<$<CONFIG:Release>:
        -O3
        -flto=thin # faster incremental builds
        -DNDEBUG
    >
)

target_link_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:
        -fsanitize=address,undefined #leak
        -fsanitize-address-use-after-scope
        -fno-sanitize-recover=all
    >
    $<$<CONFIG:Release>:
        -flto=thin
    >
)

# ************ TESTS ************

option(ENABLE_TESTS "Build unit tests" ON)

if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# this makes ninja check alias for ctest compiler_tests ?
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS compiler_tests
    USES_TERMINAL
    COMMENT "Running all tests..."
)

# ************ BENCHMARKS ************

option(ENABLE_BENCHMARKS "Build Google Benchmark suite" OFF)

if(ENABLE_BENCHMARKS)
    add_subdirectory(extra/benchmark)
endif()
