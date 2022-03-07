# Adt Modder

This is a rather simple binary that takes an Apple device tree and modifies their nodes according to
a json input file.

## Getting started

Requirements:
  * Compiler with support for C++20
  * CMake 

```bash
cmake -B build -S . -G Ninja
cmake --build build
```

The input is given via a json file that contains a list of operations to perform on the given device 
tree.

The list of supported commands can be seen running `AdtModder` with the `-h` argument like:
```bash
./build/AdtModder -h
```

This should give you a help message similar to the following:

```bash
Usage: adt_modder [options] device_tree operations.json

Positional arguments:
device_tree     	Input ADT to modify
operations.json 	A json file with the operations to perform on the dt

Optional arguments:
-h --help       	shows help message and exits [default: false]
-v --version    	prints version information and exits [default: false]
-o --output     	[default: "modded_adt.bin"]

Supported Adt Modder commands:
Command "zero_out_property": Writes 0s to the given property value
Command "randomize_property": Randomizes a property value in the given adt
Command "replace_property": Replaces the contents of the property by the given value
```

For an example of how to write the json description, check `example.json`.

## Acknowledgements 

The base adt code here was taken from [m1n1](https://github.com/AsahiLinux/m1n1), which is licensed 
under the MIT license.
