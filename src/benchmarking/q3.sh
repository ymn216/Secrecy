#!/bin/bash

cd build

# 循环运行 mpirun 命令，参数从 1024 到 32768
for ((i=1024; i<=32768; i*=2)); do
    echo "Running with parameter: $i"
    mpirun --allow-run-as-root -np 3 ./exp_q3 $i $i $(expr $i / 16)
done