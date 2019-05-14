#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#define Y_DENOISE_LOW_THRES   16
#define Y_DENOISE_HIGH_THRES 235
#define U_DENOISE_LOW_THRES   16
#define U_DENOISE_HIGH_THRES 240
#define V_DENOISE_LOW_THRES   16
#define V_DENOISE_HIGH_THRES 240

#define BI_RGB 0
#define BMP_FILE_INDENTIFIER 0x4D42

#define NO_ERROR   0
#define NOT_READY -1
#define NO_MEMORY -2
#define INVALID_PARM -3
#define TRUE  (1 == 1)
#define FALSE (!TRUE)
#define SUCCEED(rc) ((rc) == NO_ERROR)
#define ISNULL(p)   ((p) == NULL)
#define SECURE_FREE(ptr) \
    do { \
        if (!ISNULL(ptr)) { \
            free(ptr); \
            (ptr) = NULL; \
        } \
    } while(0)

typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint32_t LONG;
typedef uint8_t  BYTE;

// struct definition from Microsoft Windows,
// Keep same name with origin copy.
typedef struct BITMAPFILEHEADER {
    WORD  bfType;
    DWORD bfSize;
    WORD  bfReserved1;
    WORD  bfReserved2;
    DWORD bfOffBits;
} __attribute__((packed)) BITMAPFILEHEADER;

// struct definition from Microsoft Windows,
// Keep same name with origin copy.
typedef struct BITMAPINFOHEADER {
    DWORD biSize;
    LONG  biWidth;
    LONG  biHeight;
    WORD  biPlanes;
    WORD  biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG  biXPelsPerMeter;
    LONG  biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} __attribute__((packed)) BITMAPINFOHEADER;

// struct definition from Microsoft Windows,
// Keep same name with origin copy.
typedef struct tagRGBQUAD {
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;
} __attribute__((packed)) RGBQUAD;

void showBmpHead(BITMAPFILEHEADER *pHead) {
    printf("Bitmap header:\n");
    printf("  type: 0x%x\n", pHead->bfType);
    printf("  size: %d\n", pHead->bfSize);
    printf("  reserved1: 0x%x\n", pHead->bfReserved1);
    printf("  reserved2: 0x%x\n", pHead->bfReserved2);
    printf("  data offset: %d\n", pHead->bfOffBits);
}

void showBmpInfoHead(BITMAPINFOHEADER *pInforHead) {
    printf("Bitmap info\n");
    printf("  struct size: %d\n", pInforHead->biSize);
    printf("  width: %d\n", pInforHead->biWidth);
    printf("  height: %d\n", pInforHead->biHeight);
    printf("  planes: %d\n", pInforHead->biPlanes);
    printf("  bits of color: %d\n", pInforHead->biBitCount);
    printf("  compression type: %d\n", pInforHead->biCompression);
    printf("  data sizes: %d\n", pInforHead->biSizeImage);
    printf("  X pixels per meter: %d\n", pInforHead->biXPelsPerMeter);
    printf("  Y pixels per meter: %d\n", pInforHead->biYPelsPerMeter);
    printf("  number of colors in file: %d\n", pInforHead->biClrUsed);
    printf("  number of import colors: %d\n", pInforHead->biClrImportant);
}

int32_t getBmpWidthHeight(BITMAPFILEHEADER *file,
    BITMAPINFOHEADER *info, uint32_t *w_out, uint32_t *h_out)
{
    // If the width is not times of 4,
    // Need to fill with 0 at the end of each line
    // Since Windows OS have minimum scanning for 4 bytes each time
    if (((info->biWidth * info->biBitCount / 8) % 4) == 0) {
        *w_out = info->biWidth;
    } else {
        *w_out = (info->biWidth * info->biBitCount + 31) / 32 * 4;
    }

    if ((info->biHeight % 2) == 0) {
        *h_out = info->biHeight;
    } else {
        *h_out = info->biHeight + 1;
    }

    return NO_ERROR;
}

uint32_t getSizeofRgbQuad(BITMAPINFOHEADER *info)
{
    return sizeof(RGBQUAD) * pow(2.0, info->biBitCount);
}

int8_t makePalette(uint8_t *bmp, BITMAPFILEHEADER *file,
    BITMAPINFOHEADER *info, RGBQUAD *quad)
{
    int8_t rc = FALSE;

    uint32_t size = getSizeofRgbQuad(info);
    if (size == (file->bfOffBits - sizeof(BITMAPFILEHEADER) - info->biSize)) {
        uint8_t *quad = bmp + sizeof(BITMAPFILEHEADER) + info->biSize;
        memcpy(quad, quad, size);
        rc = TRUE;
    }

    return rc;
}

int32_t parse24bitRgbData(uint8_t *bmp,
    BITMAPFILEHEADER *file, BITMAPINFOHEADER *info,
    uint8_t *orig, uint8_t *result, uint32_t w, uint32_t h)
{
    memcpy(result, orig, w * h);
    return NO_ERROR;
}

int32_t parse16bitRgbData(uint8_t *bmp,
    BITMAPFILEHEADER *file, BITMAPINFOHEADER *info,
    uint8_t *orig, uint8_t *result, uint32_t w, uint32_t h)
{
    if(info->biCompression == BI_RGB) {
        for (uint32_t i = 0; i < w * h; i += 2) {
            *result = (orig[i] & 0x1F) << 3;
            *(result + 1) = ((orig[i] & 0xE0) >> 2) + ((orig[i+1] & 0x03) << 6);
            *(result + 2) = (orig[i+1] & 0x7C)<<1;
            result += 3;
        }
    }

    return NO_ERROR;
}

int32_t parseBelow16bitRgbData(uint8_t *bmp,
    BITMAPFILEHEADER *file, BITMAPINFOHEADER *info,
    uint8_t *orig, uint8_t *result, uint32_t w, uint32_t h)
{
    int32_t  rc = NO_ERROR;
    RGBQUAD *quad = NULL;
    uint32_t num = 0;
    uint8_t  mask = 0;

    if (SUCCEED(rc)) {
        num = getSizeofRgbQuad(info);
        quad = (RGBQUAD *)malloc(sizeof(RGBQUAD) * num);
        if (ISNULL(quad)) {
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        if (!makePalette(bmp, file, info, quad)) {
            printf("%d: Failed to find the rgb quad\n", __LINE__);
            rc = INVALID_PARM;
        }
    }

    if (SUCCEED(rc)) {
        for (uint32_t i = 0; i < w * h; i++) {
            mask = (info->biBitCount == 1) ? 0x80 : mask;
            mask = (info->biBitCount == 2) ? 0xC0 : mask;
            mask = (info->biBitCount == 4) ? 0xF0 : mask;
            mask = (info->biBitCount == 8) ? 0xFF : mask;

            int32_t shiftCnt = 1;
            while (mask) {
                int8_t index = 0;
                if (mask == 0xFF) {
                    index = orig[i];
                } else {
                    index = (orig[i] & mask) >> (8 - shiftCnt * info->biBitCount);
                }

                *result = quad[index].rgbBlue;
                *(result + 1) = quad[index].rgbGreen;
                *(result + 2) = quad[index].rgbRed;

                if(info->biBitCount == 8) {
                    mask =0;
                } else {
                    mask >>= info->biBitCount;
                }
                result += 3;
                shiftCnt++;
            }
        }
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        SECURE_FREE(quad);
    }

    return rc;
}

int32_t parseRgb(uint8_t *bmp, BITMAPFILEHEADER *file,
    BITMAPINFOHEADER *info, uint8_t *rgbDataOut)
{
    uint32_t width = 0, height = 0;
    uint8_t *data = NULL;
    uint32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = getBmpWidthHeight(file, info, &width, &height);
        if (SUCCEED(rc)) {
            width = (width * info->biBitCount) / 8;
            height = height;
        } else {
            printf("%d: Failed to get width and height\n", __LINE__);
        }
    }

    if (SUCCEED(rc)) {
        data = (uint8_t *)malloc(width * height);
        if (ISNULL(data)) {
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        uint8_t *orig = bmp + file->bfOffBits;
        for (uint32_t i = 0; i < height; i++) {
            for (uint32_t j = 0; j < width; j++) {
                 data[i * width + j] = orig[(height - i - 1) * width + j];
            }
        }
    }

    if (SUCCEED(rc)) {
        switch(info->biBitCount) {
            case 24: {
                rc = parse24bitRgbData(bmp, file, info,
                    data, rgbDataOut, width, height);
            } break;
            case 16: {
                rc = parse16bitRgbData(bmp, file, info,
                    data, rgbDataOut, width, height);
            } break;
            default: {
                rc = parseBelow16bitRgbData(bmp, file, info,
                    data, rgbDataOut, width, height);
            } break;
        }
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        SECURE_FREE(data);
    }

    return rc;
}

int8_t lookupTableInited = FALSE;
float RGBYUV02990[256], RGBYUV05870[256], RGBYUV01140[256];
float RGBYUV01684[256], RGBYUV03316[256];
float RGBYUV04187[256], RGBYUV00813[256];

void initLookupTable()
{
    if (!lookupTableInited) {
        for (int i = 0; i < 256; i++) {
            RGBYUV02990[i] = (float)0.2990 * i;
            RGBYUV05870[i] = (float)0.5870 * i;
            RGBYUV01140[i] = (float)0.1140 * i;
            RGBYUV01684[i] = (float)0.1684 * i;
            RGBYUV03316[i] = (float)0.3316 * i;
            RGBYUV04187[i] = (float)0.4187 * i;
            RGBYUV00813[i] = (float)0.0813 * i;
        }
        lookupTableInited = TRUE;
    }
}

void denoiseYuv(uint8_t *y, uint8_t *u, uint8_t *v, uint32_t w, uint32_t h)
{
    for (uint32_t i = 0; i< w * h; i++) {
        y[i] = y[i] < Y_DENOISE_LOW_THRES ? Y_DENOISE_LOW_THRES : y[i];
        y[i] = y[i] > Y_DENOISE_HIGH_THRES ? Y_DENOISE_HIGH_THRES : y[i];
    }

    for(uint32_t i = 0; i< h * w / 4; i++) {
        u[i] = u[i] < U_DENOISE_LOW_THRES ? U_DENOISE_LOW_THRES : u[i];
        v[i] = v[i] < V_DENOISE_LOW_THRES ? V_DENOISE_LOW_THRES : v[i];
        u[i] = u[i] > U_DENOISE_HIGH_THRES ? U_DENOISE_HIGH_THRES : u[i];
        v[i] = v[i] > V_DENOISE_HIGH_THRES ? V_DENOISE_HIGH_THRES : v[i];
    }

    return;
}

int32_t covertRgb2Yuv(uint8_t *rgbData,
    uint32_t w, uint32_t h, uint8_t *y, uint8_t *u, uint8_t *v)
{
    int32_t rc = NO_ERROR;
    uint8_t *utemp = NULL;
    uint8_t *vtemp = NULL;

    if (SUCCEED(rc)) {
        initLookupTable();
    }

    if (SUCCEED(rc)) {
        utemp = (uint8_t *)malloc(w * h);
        if (ISNULL(utemp)) {
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        vtemp = (uint8_t *)malloc(w * h);
        if (ISNULL(vtemp)) {
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        for (int32_t i = 0, n = 0; n < w * h * 3; n += 3, i++) {
            uint8_t nb = rgbData[n];
            uint8_t ng = rgbData[n + 1];
            uint8_t nr = rgbData[n + 2];
            y[i] = (uint8_t)(RGBYUV02990[nr] + RGBYUV05870[ng] + RGBYUV01140[nb]);
            utemp[i] = (uint8_t)(-RGBYUV01684[nr] - RGBYUV03316[ng] + nb/2 + 128);
            vtemp[i] = (uint8_t)(nr/2 - RGBYUV04187[ng] - RGBYUV00813[nb] + 128);
        }
    }

    if (SUCCEED(rc)) {
        int32_t k = 0;
        for (int32_t i = 0; i < h; i += 2) {
            for(int32_t j = 0; j < w; j += 2) {
                u[k] = (utemp[i * w + j] + utemp[(i + 1) * w + j] +
                    utemp[i * w + j + 1] + utemp[(i + 1) * w + j + 1]) / 4;
                v[k] = (vtemp[i * w + j] + vtemp[(i + 1) * w + j] +
                    vtemp[i * w + j + 1] + vtemp[(i + 1) * w + j + 1]) / 4;
                k++;
            }
        }
    }

    if (SUCCEED(rc)) {
        denoiseYuv(y, u, v, w, h);
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        SECURE_FREE(utemp);
        SECURE_FREE(vtemp);
    }

    return rc;
}

int32_t readFile(char *path, uint8_t **buf, uint32_t *size);
int32_t writeYuvFile(char *path,
    uint8_t *y, uint8_t *u, uint8_t *v, uint32_t len);

int32_t Bmp2Yuv(char *bmp_path, char *yuv_path)
{
    int32_t rc = NO_ERROR;
    uint8_t *buf = NULL;
    uint8_t *rgb = NULL, *y = NULL, *u = NULL, *v = NULL;
    uint32_t size = 0;
    uint32_t w, h;
    BITMAPFILEHEADER file;
    BITMAPINFOHEADER info;

    if (SUCCEED(rc)) {
        rc = readFile(bmp_path, &buf, &size);
        if (!SUCCEED(rc)) {
            printf("%d: Failed to read bmp file %s, %d\n",
                __LINE__, bmp_path, rc);
        }
    }

    if (SUCCEED(rc)) {
        memcpy(&file, buf, sizeof(BITMAPFILEHEADER));
        showBmpHead(&file);
        if (file.bfType == BMP_FILE_INDENTIFIER) {
            memcpy(&info, buf + sizeof(BITMAPFILEHEADER),
                sizeof(BITMAPINFOHEADER));
            showBmpInfoHead(&info);
        } else {
            rc = INVALID_PARM;
            printf("%d: File %s is not a bmp file, %d\n",
                __LINE__, bmp_path, rc);
        }
    }


    if (SUCCEED(rc)) {
        rc = getBmpWidthHeight(&file, &info, &w, &h);
        if (!SUCCEED(rc)) {
            printf("%d: Failed to get bmp width and height. %d\n",
                __LINE__, rc);
        } else {
        }
    }

    if (SUCCEED(rc)) {
        uint32_t len = w * h * 3;
        rgb = (uint8_t *)malloc(len);
        len = w * h;
        y = (uint8_t *)malloc(len);
        u = (uint8_t *)malloc(len / 4);
        v = (uint8_t *)malloc(len / 4);
        if (ISNULL(rgb) || ISNULL(y) || ISNULL(u) || ISNULL(v)) {
            uint32_t total = (uint32_t)(w * h * 4.5f);
            printf("%d: Failed to alloc WxH %dx%d total %d" \
                "bytes of rgb memory\n", __LINE__, w, h, total);
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = parseRgb(buf, &file, &info, rgb);
        if (!SUCCEED(rc)) {
            printf("%d: Failed to parse rgb data\n", __LINE__);
        }
    }

    if (SUCCEED(rc)) {
        rc = covertRgb2Yuv(rgb, w, h, y, u, v);
        if (!SUCCEED(rc)) {
            printf("%d: Failed to convert rgb data to yuv\n", __LINE__);
        }
    }

    if (SUCCEED(rc)) {
        rc = writeYuvFile(yuv_path, y, u, v, w * h);
        if (!SUCCEED(rc)) {
            printf("%d: Failed to convert rgb data to yuv\n", __LINE__);
        }
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        SECURE_FREE(buf);
        SECURE_FREE(rgb);
        SECURE_FREE(y);
        SECURE_FREE(u);
        SECURE_FREE(v);
    }

    return rc;
}

int32_t readFile(char *path, uint8_t **buf, uint32_t *size)
{
    int32_t rc = NO_ERROR;
    FILE *fp = NULL;
    *size = 0;

    if (SUCCEED(rc)) {
        fp = fopen(path, "rb");
        if (ISNULL(fp)) {
            rc = NOT_READY;
        }
    }

    if (SUCCEED(rc)) {
        long cur_pos = ftell(fp);
        int32_t offset = fseek(fp, 0, SEEK_END);
        if (offset == -1) {
            rc = NOT_READY;
        } else {
            *size = ftell(fp);
            offset = fseek(fp, cur_pos, SEEK_SET);
            if (offset == -1) {
                rc = NOT_READY;
            }
        }
    }

    if (SUCCEED(rc)) {
        *buf = (uint8_t *)malloc(*size);
        if (!SUCCEED(rc)) {
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        size_t read_size = fread(*buf, 1, *size, fp);
        if (read_size != *size) {
            SECURE_FREE(*buf);
            rc = NOT_READY;
        }
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        if (!ISNULL(fp)) {
            fclose(fp);
            fp = NULL;
        }
    }

    return rc;
}

size_t writeFile(char *path, uint8_t *buf, uint32_t size)
{
    int32_t rc = NO_ERROR;
    FILE *fp = NULL;
    size_t written = 0;

    if (SUCCEED(rc)) {
        fp = fopen(path, "wb");
        if (ISNULL(fp)) {
            rc = NOT_READY;
        }
    }

    if (SUCCEED(rc)) {
        written = fwrite(buf, 1, size, fp);
        if (written != size) {
            rc = NOT_READY;
        }
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        if (!ISNULL(fp)) {
            fclose(fp);
            fp = NULL;
        }
    }

    return written;
}

int32_t writeYuvFile(char *path,
    uint8_t *y, uint8_t *u, uint8_t *v, uint32_t size)
{
    int32_t rc = NO_ERROR;
    uint8_t *buf = NULL;
    uint32_t ylen = size;
    uint32_t uvlen = size / 4;
    uint32_t length = ylen + uvlen * 2;

    if (SUCCEED(rc)) {
        buf = (uint8_t *)malloc(length);
        if (ISNULL(buf)) {
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        memcpy(buf, y, ylen);
        for (uint32_t i = 0, j = 0; i < uvlen * 2; i += 2, j++) {
            *(buf + ylen + i) = *(v + j);
            *(buf + ylen + i + 1) = *(u + j);
        }
    }

    if (SUCCEED(rc)) {
        size_t written = writeFile(path, buf, length);
        if (written != length) {
            printf("Failed to write %d bytes to %s\n", length, path);
            rc = NOT_READY;
        }
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        SECURE_FREE(buf);
    }

    return rc;
}

