#!/bin/bash
rm -rf .bin
mkdir .bin
cd .bin
cmake ..
make

if [ $? != 0 ]; then
    exit 1
fi

./UnitTest

if [ $? != 0 ]; then
    exit 2
else
    exit 0
fi