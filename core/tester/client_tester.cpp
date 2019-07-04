#include <iostream>
#include "ServiceCore.h"

using namespace uranium;
int main(int argc, char **argv)
{
    ServiceCore serCore(TRAN_CLINET, "/tmp/lhb/clinet/");
    serCore.construct();
    serCore.initialize();
    serCore.start();
    while (1) {
        sleep(2);
    }
    return 0;
}