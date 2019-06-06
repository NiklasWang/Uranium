#!/bin/sh

if [ "$0" != "-bash" ];then
	echo "Runing with:"
	echo "            . env.mk"
fi

export LD_LIBRARY_PATH=/mnt/d/lenvov_wokspace/source/Uranium/monitor/lib:${LD_LIBRARY_PATH}

