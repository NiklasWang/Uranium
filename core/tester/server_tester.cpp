#include <stdio.h>
#include <iostream>
#include "ServiceCore.h"

using namespace uranium;
void helpPrint(const char *funcName)
{
    printf("Usage: %s User_name User_password\n", funcName);
    printf("%s lihb13 testPasswd\n", funcName);
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        helpPrint(argv[0]);
        return -1;
    }

    ServiceCore serCore(TRANS_SERVER, " ", argv[1], argv[2]);
    serCore.construct();
    serCore.initialize();
    serCore.start();
    while (1) {
        sleep(2);
    }
    return 0;
}