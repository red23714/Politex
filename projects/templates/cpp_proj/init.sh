#!/usr/bin/env bash

set -e

echo "==> Creating build directory..."
mkdir -p build

echo "==> Entering build directory..."
cd build

echo "==> Running CMake (Clang + Ninja)..."
cmake -G Ninja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug ..
# cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug ..

echo "==> Copying compile_commands.json to project root..."
cp compile_commands.json ..

echo "==> Done!"
