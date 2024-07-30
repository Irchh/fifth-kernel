cmake_minimum_required(VERSION 3.30)
project(CKERN)
enable_language(C ASM)

file(GLOB C_SRCS src/*.c)
file(GLOB ASM_SRCS src/asm/*.S)

add_executable(CKERN ${ASM_SRCS} ${C_SRCS})
