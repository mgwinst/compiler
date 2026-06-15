cmake_minimum_required(VERSION 4.0)

project(winc VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # don't silently fall back to older standard
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED)

add_library(winc_lib STATIC
    src/core/driver/compile.cpp
    src/core/utils/print/print.cpp
    src/core/frontend/lexer/lexer.cpp 
    src/core/frontend/parser/parser.cpp 
    src/core/frontend/error/diagnostics.cpp
    src/core/frontend/sema/types/type_table.cpp
    src/core/frontend/sema/symbol.cpp
    src/core/frontend/sema/analyzer.cpp
    src/core/middleend/ir/BasicBlock.cpp
    src/core/middleend/ir/Constant.cpp
    src/core/middleend/ir/Function.cpp
    src/core/middleend/ir/Instruction.cpp
    src/core/middleend/ir/Program.cpp
    src/core/middleend/ir/Value.cpp
    src/core/middleend/lowering/LoweringEngine.cpp
    src/core/middleend/opt/EarlyOptimizer.cpp
    src/core/middleend/opt/RewriteEngine.cpp
)

target_include_directories(winc_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
)

target_link_libraries(winc_lib PUBLIC Boost::boost)

add_executable(${PROJECT_NAME} src/main.cpp)
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