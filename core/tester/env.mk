#!/bin/sh

if [ "$0" != "-bash" ];then
	echo "Runing with:"
	echo " . env.mk"
fi
BASH=/mnt/d/lenvov_wokspace/source/Uranium
LIBPATH=${BASH}/memory:${BASH}/utils:${BASH}/:${BASH}/threads:${BASH}/transmission:${BASH}monitor:
LIBPATH+=${BASH}/core:${BASH}/external/fswatch:${BASH}/log:

export LD_LIBRARY_PATH=${LIBPATH}:${LD_LIBRARY_PATH}

