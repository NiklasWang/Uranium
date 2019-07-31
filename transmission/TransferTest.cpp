#include <iostream>
#include "TransferCore.h"

namespace  uranium
{

int __TransferMain(int argc, char **argv)
{
    int32_t rc = NO_ERROR;
    TransferCore core(TRANS_SERVER, NULL, "lihb13", "Lhbzyy8629");

    if (SUCCEED(rc)) {
        rc =  core.construct();
    }

    if (SUCCEED(rc)) {

        core.send("/mnt/d/lenvov_wokspace/source/curl-7.50.3.tar.bz2");
        sleep(3);

        core.receive([](std::string & filePath)->int32_t{
            std::cout << "LHB " << filePath << std::endl;
        });

        sleep(3);
    }

    sleep(3);
    core.destruct();

    return 1;
}

};

