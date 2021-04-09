# Parallel Bitonic Sort Example

## Instructions

1. Install OpenCilk from [here](https://github.com/OpenCilk/opencilk-project/releases)
2. Compile with `clang++ ./bitonic_sort.cpp -fopencilk`
3. Run `./a.out [n]` where n is the size of array you want generated and tested

## CilkScale Instructions

CilkScale can measure the parallelism this code exhibits in practice.

2. Compile with `clang++ ./bitonic_sort.cpp -fopencilk -fcilktool=cilkscale`
3. Run `./a.out [n]` where n is the size of array you want generated and tested