#!/bin/bash
if [ -f build/codify ]; then
    rm -f ./codify
    mv build/codify ./codify
fi
if [ -f build/codify.exe ]; then
    rm -f ./codify.exe
    mv build/codify.exe ./codify.exe
fi
rm -rf build
mkdir build
cd build
cmake ..
make
if [ -f codify ]; then
    rm -f ../codify
    mv codify ../codify
fi
if [ -f codify.exe ]; then
    rm -f ../codify.exe
    mv codify.exe ../codify.exe
fi