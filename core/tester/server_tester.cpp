#include <stdio.h>
#include <iostream>
#include "ServiceCore.h"

using namespace uranium;
int main(int argc, char **argv)
{
    ServiceCore serCore(TRANS_SERVER, " ", "lihb13", "Lhbzyy8629");
    serCore.construct();
    serCore.initialize();
    serCore.start();
    while (1) {
        sleep(2);
    }
    return 0;
}