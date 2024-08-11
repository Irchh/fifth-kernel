#!/bin/bash

set -e
./build.sh
cd build
qemu-system-riscv64 -machine virt -bios none -kernel CKERN -display none -serial stdio -smp 1 #-s -S
