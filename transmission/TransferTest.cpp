#include <iostream>
#include "TransferCore.h"

namespace  uranium
{

int __TransferMain(int argc, char **argv)
{

    TransferCore core(TRANS_SERVER);

    core.construct();
    core.send("/mnt/d/lenvov_wokspace/source/androidQ");
    sleep(3);
    core.destruct();

    return 1;
}

};

