cmake_minimum_required(VERSION 3.9)

project(c-playground C)

add_compile_options(
        -Wall
        -Wextra
        -Wstrict-prototypes
        -Wmissing-prototypes
        -Wmissing-declarations
        -O3
        -flto)
include_directories(include)

add_subdirectory(posix-shared-memory)
