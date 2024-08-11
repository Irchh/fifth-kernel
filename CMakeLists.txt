cmake_minimum_required(VERSION 3.30)
project(CKERN)
enable_language(C ASM)

include_directories("include")

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_C_FLAGS "-O0 -g -Wall -Wextra -Wno-unused-command-line-argument -mcmodel=medany -std=c23 -fuse-ld=lld -fno-builtin -nostdlib -ffreestanding -target riscv64-unknown-none-elf -march=rv64imafdc -mabi=lp64d -Wl,-T ${CMAKE_SOURCE_DIR}/linker.ld")

set(CMAKE_ASM_FLAGS "-nostdinc -nostdlib -target riscv64-unknown-linux-gnu -march=rv64imafdc -mabi=lp64d")
set(CMAKE_ASM_COMPILER "/usr/bin/clang")

file(GLOB C_SRCS "src/*.c")
file(GLOB ASM_SRCS "src/asm/*.S")

add_executable(CKERN ${ASM_SRCS} ${C_SRCS})

set_target_properties(CKERN PROPERTIES LINK_DEPENDS "${CMAKE_SOURCE_DIR}/linker.ld")
