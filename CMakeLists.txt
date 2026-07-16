cmake_minimum_required(VERSION 4.0)

project(winc VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # don't silently fall back to older standard
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED)

add_library(winc_lib STATIC
    winlang/core/driver/compile.cpp
    winlang/core/utils/print/print.cpp
    winlang/core/frontend/lexer/lexer.cpp 
    winlang/core/frontend/parser/parser.cpp 
    winlang/core/frontend/error/diagnostics.cpp
    winlang/core/frontend/sema/types/type_table.cpp
    winlang/core/frontend/sema/symbol.cpp
    winlang/core/frontend/sema/analyzer.cpp
    winlang/core/middleend/ir/BasicBlock.cpp
    winlang/core/middleend/ir/Constant.cpp
    winlang/core/middleend/ir/Function.cpp
    winlang/core/middleend/ir/Instruction.cpp
    winlang/core/middleend/ir/Program.cpp
    winlang/core/middleend/ir/Value.cpp
    winlang/core/middleend/lowering/LoweringEngine.cpp
    winlang/core/middleend/analysis/escape.cpp
    winlang/core/middleend/transforms/trivial_dce.cpp
    winlang/core/middleend/transforms/dse.cpp
    winlang/core/middleend/transforms/cleanup_cfg.cpp
    winlang/core/middleend/transforms/mem2reg.cpp
)

target_include_directories(winc_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/winlang/core
)

target_link_libraries(winc_lib PUBLIC Boost::boost)

add_executable(${PROJECT_NAME} winlang/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE winc_lib)

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
