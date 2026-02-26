#!/usr/bin/env bash

PROGRAM=./main   # имя твоего бинарника

# 16 значений от 64 до ~5000 (значительный рост)
sizes=(
10
20
40
80
160
320
640
1280
2560
5120
10240
20480
)

echo "===== OpenCL Matrix Multiplication Benchmark ====="

for N in "${sizes[@]}"; do
    echo
    echo "===== N = $N ====="
    echo "$N" | $PROGRAM
done

echo
echo "===== DONE ====="
