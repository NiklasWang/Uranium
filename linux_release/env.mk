#!/bin/sh

BASH=`pwd`
LIBPATH=${BASH}:${BASH}/x86_64-linux-gnu:
export LD_LIBRARY_PATH=${LIBPATH}:${LD_LIBRARY_PATH}

