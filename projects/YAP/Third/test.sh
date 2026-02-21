#!/usr/bin/env bash

PROGRAM=./main   # имя твоего бинарника

# 16 значений от 64 до ~5000 (значительный рост)
sizes=(
64
96
128
192
256
384
512
768
1024
1536
2048
2560
3072
3584
4096
4608
)

echo "===== OpenCL Matrix Multiplication Benchmark ====="

for N in "${sizes[@]}"; do
    echo
    echo "===== N = $N ====="
    echo "$N" | $PROGRAM
done

echo
echo "===== DONE ====="
