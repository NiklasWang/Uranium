#include "unistd.h"
#include "stdint.h"
#include "string.h"
#include "stdio.h"

#define NO_ERROR 0

static int32_t rotate_90_location(uint32_t w, uint32_t h,
    uint32_t x, uint32_t y, uint32_t *ox, uint32_t *oy)
{
    *ox = h - y - 1;
    *oy = x;
    return NO_ERROR;
}

static int32_t rotate_180_location(uint32_t w, uint32_t h,
    uint32_t x, uint32_t y, uint32_t *ox, uint32_t *oy)
{
    *ox = w - x - 1;
    *oy = h - y - 1;
    return NO_ERROR;
}

static int32_t rotate_270_location(uint32_t w, uint32_t h,
    uint32_t x, uint32_t y, uint32_t *ox, uint32_t *oy)
{
    *ox = y;
    *oy = w - x - 1;
    return NO_ERROR;
}

static int32_t rotate_360_location(uint32_t w, uint32_t h,
    uint32_t x, uint32_t y, uint32_t *ox, uint32_t *oy)
{
    *ox = x;
    *oy = y;
    return NO_ERROR;
}

typedef enum {
    ROTATE_90,
    ROTATE_180,
    ROTATE_270,
    ROTATE_360
} rotate_type;

const char * const rotate_string[] = {
    "rotate 90",   // ROTATE_90
    "rotate 180",  // ROTATE_180
    "rotate 270",  // ROTATE_270
    "rotate 360",  // ROTATE_360
};

static int32_t (*rotate_location[])
    (uint32_t w, uint32_t h,
    uint32_t x, uint32_t y, uint32_t *ox, uint32_t *oy) = {
    rotate_90_location,  // ROTATE_90
    rotate_180_location, // ROTATE_180
    rotate_270_location, // ROTATE_270
    rotate_360_location, // ROTATE_360
};

#define EXCHANGE_DATA(a, b) \
    typeof(a) _a = a; \
    typeof(b) _b = b; \
    (void)(&_a == &_b); \
    a = _b; \
    b = _a;

static int32_t rotate_dimension(rotate_type type,
    uint32_t w, uint32_t h, uint32_t *ow, uint32_t *oh) {
    switch (type) {
        case ROTATE_90:
        case ROTATE_270:
            *ow = h;
            *oh = w;
            break;
        case ROTATE_180:
        case ROTATE_360:
            *ow = w;
            *oh = h;
            break;
        default:
            *ow = 0;
            *oh = 0;
            break;
    };

    return NO_ERROR;
}

static int32_t rotate_matrix(rotate_type type,
    int32_t *matrix, int32_t *output, uint32_t w, uint32_t h)
{
    uint32_t i, j;
    uint32_t x, y;
    uint32_t ow, oh;

    rotate_dimension(type, w, h, &ow, &oh);

    for (i = 0; i < w; i++) {
        for (j = 0; j < h; j++) {
            (rotate_location[type])(w, h, i, j, &x, &y);
            //printf("a[%d][%d] ==> b[%d][%d]\n", i, j, x, y);
            *(output + y * ow + x) = *(matrix + j * w + i);
        }
    }

    return NO_ERROR;
}

static int32_t output_matrix(int32_t *matrix, uint32_t w, uint32_t h)
{
    uint32_t i, j;

    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            printf("%3d,", *(matrix + i * w + j));
        }
        printf("\n");
    }

    return NO_ERROR;
}

int32_t main(int argc, char **argv)
{
    int32_t run_test_case(int32_t *matrix, uint32_t w, uint32_t h);

    {
        const uint32_t w = 4;
        const uint32_t h = 5;
        int32_t matrix[] = {
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,
            13, 14, 15, 16,
            17, 18, 19, 20,
        };
        run_test_case(matrix, w, h);
    }

    {
        const uint32_t w = 5;
        const uint32_t h = 2;
        int32_t matrix[] = {
             1,  2,  3,  4,  5,
             6,  7,  8,  9, 10,
        };
        run_test_case(matrix, w, h);
    }

    {
        const uint32_t w = 5;
        const uint32_t h = 1;
        int32_t matrix[] = {
             1,  2,  3,  4,  5,
        };
        run_test_case(matrix, w, h);
    }

    {
        const uint32_t w = 1;
        const uint32_t h = 5;
        int32_t matrix[] = {
             1,
             2,
             3,
             4,
             5,
        };
        run_test_case(matrix, w, h);
    }

    {
        const uint32_t w = 2;
        const uint32_t h = 2;
        int32_t matrix[] = {
             1,  2,
             3,  4,
        };
        run_test_case(matrix, w, h);
    }

    {
        const uint32_t w = 1;
        const uint32_t h = 2;
        int32_t matrix[] = {
             1,
             2,
        };
        run_test_case(matrix, w, h);
    }

    {
        const uint32_t w = 2;
        const uint32_t h = 1;
        int32_t matrix[] = {
             1,  2,
        };
        run_test_case(matrix, w, h);
    }

    {
        const uint32_t w = 1;
        const uint32_t h = 1;
        int32_t matrix[] = {
             1,
        };
        run_test_case(matrix, w, h);
    }

    /*{
        const uint32_t w = 17;
        const uint32_t h = 13;
        int32_t matrix[w * h];
        for (uint32_t i = 0; i < w * h; i++) {
            matrix[i] = i + 1;
        }
        run_test_case(matrix, w, h);
    }*/

    return NO_ERROR;
}

int32_t run_case_package(int32_t *matrix,
    rotate_type rotate, uint32_t w, uint32_t h)
{
    int32_t output[sizeof(int32_t) * w * h];
    uint32_t ow, oh;

    printf("-------------------------------- \n");
    printf("Matrix %s: \n", rotate_string[rotate]);
    memset(output, 0, sizeof(output));
    rotate_dimension(rotate, w, h, &ow, &oh);
    rotate_matrix(rotate, matrix, output, w, h);
    output_matrix(output, ow, oh);

    return NO_ERROR;
}

int32_t run_test_case(int32_t *matrix, uint32_t w, uint32_t h)
{
    printf("Original Matrix: \n");
    output_matrix(matrix, w, h);

    run_case_package(matrix, ROTATE_90, w, h);
    run_case_package(matrix, ROTATE_180, w, h);
    run_case_package(matrix, ROTATE_270, w, h);
    run_case_package(matrix, ROTATE_360, w, h);

    printf("================================ \n");
    return NO_ERROR;
}

