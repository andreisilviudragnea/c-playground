cmake_minimum_required(VERSION 3.9)

add_compile_options(
        -Wall
        -Wextra
        -Wstrict-prototypes
        -Wmissing-prototypes
        -Wmissing-declarations
        -O3
        -flto)
include_directories(include)

add_subdirectory(shared-memory)
