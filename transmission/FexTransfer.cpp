#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <time.h>

#include "common.h"
#include "TransferInterface.h"
#include "FexTransfer.h"

namespace uranium
{

FexTransfer::FexTransfer(TRANSFER_STATUS_E tranDirct):
    mModule(MODULE_ENCRYPT),
    mTranDirct(tranDirct)
{

}

FexTransfer::~FexTransfer()
{

}

uint32_t FexTransfer::pushData(TRANSFER_BUFFER_T &cmd)
{
    int32_t rc = 0;
    std::string filePath = DIRPATH;
    std::string cmdStr = "fex -u ";

    if (SUCCEED(rc)) {
        if (cmd.mode != TRAN_MODE_FEX) {
            rc = -1;
            LOGE(mModule, "Mode mismatching!");
        }
    }
    if (SUCCEED(rc)) {
        cmdStr = "mkdir -p ";
        cmdStr += DIRPATH;
        system(cmdStr.c_str());
    }

    if (SUCCEED(rc)) {
        cmdStr = "cp -ar ";
        cmdStr += (char *)cmd.buffer;
        cmdStr += " ";
        switch (mTranDirct) {
            case TRAN_CLINET:
                filePath += CLINETFILE;
                break;
            case TRANS_SERVER:
                filePath += SERVERFILE;
                break;
            default:
                rc = -1;
                LOGE(mModule, "Not support!");
                break;
        }

    }

    if (SUCCEED(rc)) {
        cmdStr += filePath;
        std::cout << "LHB DEBUG" << cmdStr << std::endl;
        /* copy orig file to fex used files */
        rc = system((const char *) cmdStr.c_str());
    }

    if (SUCCEED(rc)) {

        cmdStr = ("fex -u ");
        cmdStr += filePath;
        rc = system((const char *) cmdStr.c_str());
    }

    return rc;
}

uint32_t FexTransfer::pullData(TRANSFER_BUFFER_T &cmd)
{
    int32_t rc = 0;
    uint8_t fileBuf[128];
    uint32_t fileBufSize = 128;
    std::string fexStr;
    std::string cmdStr = "fex -l 1 > ";
    cmdStr += DIRPATH;
    cmdStr += "cli.bin";
    std::string timeTemp;

    if (SUCCEED(rc)) {
        rc = system((const char *) cmdStr.c_str());
        if (FAILED(rc)) {
            LOGE(mModule, "Runing system failed\n");
        }
    }

    if (SUCCEED(rc)) {
        cmdStr = DIRPATH;
        cmdStr += "cli.bin";
        memset(fileBuf, 0, sizeof(fileBuf));
        rc = readFile(cmdStr, fileBuf, fileBufSize);
    }


    if (SUCCEED(rc)) {
        fexStr = (char *) fileBuf;
        int32_t pos = fexStr.find(SERVERFILE);
        // int32_t pos = fexStr.find("test");
        if (-1 != pos) {
            std::cout << "Size = " << rc << std::endl;
            std::string cliStr = fexStr.substr(pos);
            std::cout << cliStr << std::endl;
            pos = cliStr.find("| ");
            if (-1 != pos) {
                pos += 2;
                timeTemp = cliStr.substr(pos);
                // mFileLastTime = time;
                std::cout << "time value = " << timeTemp << std::endl;
            } else {
                rc = -1;
            }
        } else {
            rc = -1;
        }
    }

    if (SUCCEED(rc)) {
        /* judge  frist runing */
        if (mFileLastTime.empty()) {
            mFileLastTime = timeTemp;
            rc = 0;
        } else {
            time_t newTime = str_to_time_t(timeTemp);
            time_t lastTime = str_to_time_t(mFileLastTime);
            if (newTime <= lastTime) {
                rc = -1;
            }
        }
    }

    if (SUCCEED(rc)) {

        cmdStr = "fex -d ";
        cmdStr += DIRPATH;
        rc = system(cmdStr.c_str());

    }

    if (SUCCEED(rc)) {
        cmdStr = DIRPATH;
        cmdStr += SERVERFILE;
        memset(cmd.buffer, 0, cmd.length);
        strcpy((char *)cmd.buffer, cmdStr.c_str());
#if 0
        rc = readFile(cmdStr, (uint8_t*) cmd.buffer, cmd.length);
        if (FAILED(rc)) {
            LOGE(mModule, "read file %s feiled!", cmdStr.c_str());
        }
#endif
    }

    return rc;
}

uint32_t FexTransfer::readFile(const std::string &filePath, uint8_t *pBuffer, uint32_t &length)
{
    int32_t rc = 0;
    FILE    *pFile = NULL;
    uint32_t retSize;

    /* check file is exited */
    if (SUCCEED(rc)) {
        rc = access(filePath.c_str(), F_OK);
        if (FAILED(rc)) {
            LOGE(mModule, "File %s not exit\n", filePath.c_str());
        }
    }

    if (SUCCEED(rc)) {
        /* open file use read  only mode */
        pFile = fopen(filePath.c_str(), "r");
        if (ISNULL(pFile)) {
            rc = -3;
            LOGE(mModule, "Open file failed\n");
        }
        /* get file length */
        fseek(pFile, 0, SEEK_END);
        retSize = ftell(pFile);
        rewind(pFile);
    }

    if (SUCCEED(rc)) {
        if (retSize > length) {
            rc = -1;
            LOGE(mModule, "buffer is smaler!\n");
        }
    }

    if (SUCCEED(rc)) {
        /* read datas */
        length = fread(pBuffer, 1, retSize, pFile);
        if (retSize != length) {
            rc = -1;
            LOGE(mModule, "read length smaller than file size\n");
        }

    }

    if (NOTNULL(pFile)) {
        fclose(pFile);
    }

    return rc;
}

uint32_t FexTransfer::writeFile(const std::string &filePath, const uint8_t *buffer, uint32_t length)
{
    int32_t rc = 0;

    FILE    *pFile = NULL;

    if (SUCCEED(rc)) {
        /* open file use read  only mode */
        pFile = fopen(filePath.c_str(), "w+");
        if (ISNULL(pFile)) {
            rc = -1;
            LOGE(mModule, "Open file failed\n");
        }
        fwrite(buffer, length, 1, pFile);
    }

    if (NOTNULL(pFile)) {
        fclose(pFile);
    }

    return rc;
}

time_t FexTransfer::str_to_time_t(const std::string& ATime, const std::string& AFormat)
{
    struct tm tm_Temp;
    time_t time_Ret;
    try {
        sscanf(ATime.c_str(), AFormat.c_str(),// "%d/%d/%d %d:%d:%d" ,
               & (tm_Temp.tm_year),
               &(tm_Temp.tm_mon),
               &(tm_Temp.tm_mday),
               &(tm_Temp.tm_hour),
               &(tm_Temp.tm_min),
               &(tm_Temp.tm_sec),
               &(tm_Temp.tm_wday),
               &(tm_Temp.tm_yday));

        tm_Temp.tm_isdst = 0;
        time_Ret = mktime(&tm_Temp);
        return time_Ret;
    } catch (...) {
        return 0;
    }
}

};