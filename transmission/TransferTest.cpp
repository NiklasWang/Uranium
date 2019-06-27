#include <iostream>
#include "TransferCore.h"

namespace  uranium
{

int __TransferMain(int argc, char **argv)
{

    TransferCore core(TRANS_SERVER, NULL);

    core.construct();
    core.send("/mnt/d/lenvov_wokspace/source/androidQ");
    core.receive([](std::string & filePath)->int32_t{
        std::cout << "LHB " << filePath << std::endl;
    });
    sleep(3);
    core.destruct();

    return 1;
}

};

