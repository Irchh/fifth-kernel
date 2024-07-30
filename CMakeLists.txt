cmake_minimum_required(VERSION 3.30)
project(CKERN)
enable_language(C ASM)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_C_FLAGS "-g -Wall -std=c99")
set(CMAKE_ASM_COMPILER "/usr/bin/clang")

file(GLOB C_SRCS src/*.c)
file(GLOB ASM_SRCS src/asm/*.S)

add_executable(CKERN ${ASM_SRCS} ${C_SRCS})
