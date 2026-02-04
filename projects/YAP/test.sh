#!/bin/bash

echo "Compiling with different optimization levels..."
gcc -O0 -DNO_ZERO_DIVIDE optbench.c -o optbench_O0
gcc -O2 -DNO_ZERO_DIVIDE optbench.c -o optbench_O2
gcc -O3 -DNO_ZERO_DIVIDE optbench.c -o optbench_O3
gcc -Os -DNO_ZERO_DIVIDE optbench.c -o optbench_Os

echo -e "\nBinary sizes:"
ls -lh optbench_*

echo -e "\nExecution times:"
for level in O0 O2 O3 Os; do
    echo -n "optbench_$level: "
    time ./optbench_$level > /dev/null 2>&1
done
