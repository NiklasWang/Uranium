/**
 * @file main.cpp
 * @brief
 * @author  lenovo <xxx@lenovo.com>
 * @version 1.0.0
 * @date 2019-06-04
 */
/* Copyright(C) 2009-2019, Lenovo Inc.
 * All right reserved
 *
 */
#include "MonitorCore.h"
#include "FileManager.h"
#include "md5.h"
int main(int argc, char **argv)
{
	int32_t rc = 0;
    uranium::MonitorCore monitCore("tmp/lhb.bin","/mnt/d/lenvov_wokspace/source/Uranium");

    monitCore.construct();
    rc = monitCore.monitorDirStart();
   	std::cout<<std::endl<<"start return = "<<rc <<std::endl;
    //monitCore.monitorDirStop();
    monitCore.monitorLoopProcess();

    // monitCore.destruct();
    while(1){
    	sleep(1);
    }
   #if 0  
    fileMang.fileScanToInis();
    fileMang.fileInfosSave("/tmp/lhb.ini");
    fileMang.fileInfosLoad("/tmp/lhb.ini");
    #endif
    return 0;
}
