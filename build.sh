#!/bin/bash

set -e

cd test_program
riscv64-elf-gcc test.S -o test_program -nostartfiles -nostdlib
cd ..

[ -e build ] && rm -rf build
mkdir build
cd build
cmake -DCMAKE_VERBOSE_MAKEFILE:BOOL=OFF ..
cmake --build .
