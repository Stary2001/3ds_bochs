#!/bin/bash
cd 3ds
mkdir build
cp ../bochs output/bochs.elf
make
cp output/bochs.cia ..
cd ..