#!/bin/bash

set -e
[ -e build ] && rm -rf build
mkdir build
cd build
cmake -DCMAKE_VERBOSE_MAKEFILE:BOOL=OFF ..
cmake --build .
