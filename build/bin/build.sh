#!/bin/sh

MODE=$(echo $1 | tr [A-Z] [a-z]) 

if [ "$MODE" = "debug" ]; then
	MODE="Debug"
else
	MODE="Release"
fi

THREAD_NUM=`getconf _NPROCESSORS_ONLN`
((THREAD_NUM=THREAD_NUM/2))

echo "cd .. && cmake -DCMAKE_BUILD_TYPE=$MODE && make -j$THREAD_NUM && cd bin"

cd .. && cmake -DCMAKE_BUILD_TYPE=$MODE .. && make -j$THREAD_NUM && cd bin
