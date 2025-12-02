cmake_minimum_required(VERSION 4.0)

project(compiler VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME}
    core/main.cpp
    core/lexer/lexer.cpp 
    core/parser/parser.cpp 
    core/parser/ast.cpp 
    core/parser/error.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/core
)

# these compiler flags are always on...
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

# -DCMAKE_BUILD_TYPE=Debug
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:
        -O0
        -g3
        -fno-omit-frame-pointer
    >
)

# -DCMAKE_BUILD_TYPE=Debug
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:
        -fsanitize=address,undefined,leak
        -fsanitize-address-use-after-scope
        -fno-sanitize-recover=all   # stop on first error
    >
)

# -DCMAKE_BUILD_TYPE=Release
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Release>:
        -O3
        -flto=auto # -flto=full
        -DNDEBUG
    >
)
