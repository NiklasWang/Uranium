#!/bin/sh

if [ "$0" != "-bash" ];then
	echo "Runing with:"
	echo "            . env.mk"
fi
PWDPATH=/mnt/d/lenvov_wokspace/source/Uranium

export LD_LIBRARY_PATH=${PWDPATH}/linux_release:/mnt/d/lenvov_wokspace/source/Uranium/monitor/lib:${LD_LIBRARY_PATH}

