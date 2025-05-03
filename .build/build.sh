#!/bin/sh -ef

cd "$(dirname "$0")"/..

if [ ! -d "build" ]
then
     mkdir "build"
fi

cd build

cmake .. -DCMAKE_BUILD_PARALLEL_LEVEL=8 ${@:+"$@"}
cmake --build .

