#!/bin/bash

set -e
set -v

rm -rf build

# Configure with CMake, optionally set build type (default is Debug)
cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build

# Build with all available cores
cmake --build build

# Run unit tests
./build/runUnitTests
