#!/bin/bash
if [ -f build/codify ]; then
    mv build/codify ./codify
fi
if [ -f build/codify.exe ]; then
    mv build/codify.exe ./codify.exe
fi
rm -rf build
mkdir build
cd build
cmake ..
make