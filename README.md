# Adt Modder

This is a rather simple binary that takes a device tree and removes sensitive information regarding the specific machine.

## Getting started

Requirements:
  * Compiler with support for C++20
  * CMake 

```bash
cmake -B build -S . -G Ninja
cmake --build build
```

## Acknowledgements 

The base adt code here was taken from [m1n1](https://github.com/AsahiLinux/m1n1), which is licensed under the MIT license.
