#!/bin/sh
cd ./build
cmake .. && make
cd ..
./build/matrix