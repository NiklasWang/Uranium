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
#include "FileManager.h"
#include "md5.h"
int main(int argc, char **argv)
{
    uranium::FileManager fileMang;

    fileMang.fileScanToInis("/mnt/d/lenvov_wokspace/source/Uranium");
    fileMang.fileInfosSave("/tmp/lhb.ini");
    fileMang.fileInfosLoad("/tmp/lhb.ini");
    return 0;
}
