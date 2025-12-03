cmake_minimum_required(VERSION 4.0)

project(compiler VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # tells cmake to not silently fall back to an older standard
set(CMAKE_CXX_EXTENSIONS OFF)


target_compile_options(${PROJECT_NAME} PRIVATE
    -march=native
    -fcolor-diagnostics
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wshadow
    -Wundef
    -Werror=return-type
    # -Werror
)

# use -DCMAKE_BUILD_TYPE=Debug/Release

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:
        -O0
        -g3
        -fno-omit-frame-pointer
        -fsanitize=address,undefined,leak
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
        -fsanitize=address,undefined,leak
        -fsanitize-address-use-after-scope
        -fno-sanitize-recover=all
    >
    $<$<CONFIG:Release>:
        -flto=thin
    >
)

add_library(compiler_lib STATIC
    core/lexer/lexer.cpp 
    core/parser/parser.cpp 
    core/parser/ast.cpp 
    core/parser/error.cpp
)

target_include_directories(compiler_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core
)

add_executable(${PROJECT_NAME} core/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE compiler_lib)

# cmake -DENABLE_TESTS=OFF/ON
option(ENABLE_TESTS "Build and run unit tests" ON)

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()