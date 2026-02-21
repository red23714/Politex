#!/usr/bin/env bash

PROGRAM=./test   # имя твоего бинарника

# 16 значений от 64 до ~5000 (значительный рост)
sizes=(
10
50
100
500
1000
5000
10000
50000
100000
500000
1000000
5000000
10000000
50000000
100000000
1000000000
)

echo "===== OpenCL Matrix Multiplication Benchmark ====="

for N in "${sizes[@]}"; do
    echo
    echo "===== N = $N ====="
    echo "$N" | $PROGRAM
done

echo
echo "===== DONE ====="
