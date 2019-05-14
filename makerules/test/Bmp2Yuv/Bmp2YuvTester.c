#include <stdio.h>
#include <stdint.h>

int32_t Bmp2Yuv(char *bmp_path, char *yuv_path);

struct Files {
    char *orig;
    char *result;
};

struct Files files[] = {
    {"resources/down24.bmp", "down24.nv21"},
    {"resources/down16.bmp", "down16.nv21"},
    {"resources/down8.bmp",  "down8.nv21"},
    {"resources/down4.bmp",  "down4.nv21"},
    {"resources/down1.bmp",  "down1.nv21"},
};

int32_t main(int argc,char *argv[])
{
    int32_t rc = 0;

    for (uint32_t i = 0; i < sizeof(files) / sizeof(files[0]); i++) {
        printf("==================================================\n");
        printf("Convert bmp file %s to yuv file %s\n",
            files[i].orig, files[i].result);
        rc = Bmp2Yuv(files[i].orig, files[i].result);
        if (rc) {
            printf("%d: Failed to covert bmp %s to yuv %s, exit\n",
                __LINE__, files[i].orig, files[i].result);
            break;
        }
    }

    return rc;
}
