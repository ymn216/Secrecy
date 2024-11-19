#!/bin/bash

cd build

# 循环运行 mpirun 命令，参数从 1024 到 32768
for ((i=1024; i<=16384; i*=2)); do
    echo "Running with parameter: $i"
    mpirun --allow-run-as-root -np 3 ./exp_tpch_q13 $i $i $i
done