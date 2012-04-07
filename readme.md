# DCPUSim
Cocoa frontend for a VM running [Notch's D-CPU16 CPU](http://0x10c.com/doc/dcpu-16.txt).

# Contents
* DCPU.h & DCPU.c - virtual machine implementation
* Everything else - ObjectiveC simulation of a simple frontend
* test.bin - binary for demo program given in the spec

# Building & Running
Compile in Xcode (I used 4.3.2 on Lion - YMMV with other versions).

# Using
**Files need to be byte swapped if their endianness differs from the host machine. For example, a file containing 0x1234 will be read as 0x3412. (this is true for x86)**
* Load a program with the open button
* Mash the "Step" button until fun happens
* Hit the "Reset" button and repeat.

# Correctness (or Lack Thereof)
The only extensive testing done on the CPU implementation is to run the demo program. Until more testing's done, take the result of computation with a grain of salt.

# Licence
3-clause BSD. See the LICENCE file for the full text.