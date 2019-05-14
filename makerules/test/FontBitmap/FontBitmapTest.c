#include <stdint.h>
#include <ft2build.h>
#include <freetype.h>
#include <ftglyph.h>
#include <stdio.h>

#define LOGD        printf
#define LOGE        LOGD
#define NO_ERROR    0
#define NO_MEMORY   2
#define SUCCEED(rc) ((rc) == NO_ERROR)
#define ISNULL(ptr) ((ptr) == NULL)

#define FONT_PIXEL_SIZE 50
#define MAX_LINE_LENGTH 150

#if defined(BUILD_LINUX_X86_64)
#define FONT_PATH       "/home/wangliang17/fonts/fzse_gbk.ttf"
#elif defined (BUILD_ANDROID_AP)
#define FONT_PATH       "/system/fonts/ZUKChinese.ttf"
#else
#error Not supported system arch.
#endif

typedef struct {
    uint32_t w;
    uint32_t h;
    int32_t  pitch;
    int32_t  left;
    int32_t  top;
    uint8_t *buffer;
    uint32_t size;
    uint32_t unicode;
} _bitmap_t;

int32_t initLib(FT_Library *lib)
{
    return FT_Init_FreeType(lib);
}

int32_t deinitLib(FT_Library *lib)
{
    return FT_Done_FreeType(*lib);
}

int32_t copyBitmap(_bitmap_t *info, uint8_t *buf, uint8_t *result)
{
    for (uint32_t i = 0; i < info->h; i++) {
        for (int32_t j = 0; j < info->pitch; j++) {
            *(result + i * info->pitch + j) = *(buf + i * info->pitch + j);
        }
    }

    return NO_ERROR;
}

int32_t convertBitmap(_bitmap_t *info, uint8_t *buf, uint8_t *result)
{
    uint32_t w, h, wr, hr;
    int32_t pitch;

    pitch = info->pitch + info->left * 2;
    for (h = 0, hr = info->size - info->h;
        (h < info->h) && (hr < info->size);
        h++, hr++) {
        for (w = 0, wr = info->left;
            (w < (uint32_t)info->pitch) && (wr < info->size);
            w++, wr++) {
            if (*(buf + h * info->pitch + w)) {
                *(result + hr * pitch + wr) = 1;
            }
        }
    }

    info->h = info->size;
    info->pitch = pitch;

    return NO_ERROR;
}

int32_t getBitmap(FT_Library *lib, uint32_t unicode,
    int32_t size, const char *ttf, _bitmap_t *bitmap)
{
    FT_Face face = NULL;
    FT_Glyph glyph = NULL;
    uint8_t *buffer = NULL;
    FT_BBox bbox;
    FT_Error rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = FT_New_Face(*lib, ttf, 0, &face);
        if (!SUCCEED(rc)) {
            LOGE("%s: failed to create face at %d\n", __func__, __LINE__);
        }
    }

    if (SUCCEED(rc)) {
        rc = FT_Set_Pixel_Sizes(face, size, 0);
        if (!SUCCEED(rc)) {
            LOGE("%s: %d failed to set pixel size %d\n",
                __func__, __LINE__, size);
        }
    }

    if (SUCCEED(rc)) {
        rc = FT_Load_Glyph(face,
            FT_Get_Char_Index(face, unicode), FT_LOAD_DEFAULT);
        if (!SUCCEED(rc)) {
            LOGE("%s: %d failed to load glyph of unicode 0x%x\n",
                __func__, __LINE__, unicode);
        }
    }

    if (SUCCEED(rc)) {
        rc = FT_Get_Glyph(face->glyph, &glyph);
        if (!SUCCEED(rc)) {
            LOGE("%s: %d failed to get glyph\n", __func__, __LINE__);
        }
    }

    if (SUCCEED(rc)) {
        FT_Glyph_Get_CBox(glyph, ft_glyph_bbox_pixels, &bbox);
        rc = FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0 , 1);
        if (!SUCCEED(rc)) {
            LOGE("%s: %d failed to convert bitmap\n", __func__, __LINE__);
        }
    }

    if (SUCCEED(rc)) {
        FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

        LOGD("%s: %d Font 0x%x size %d left %d top %d w %d h %d pitch %d "\
            "face matrics ascender %ld descender %ld height %ld max advance %ld "\
            "glyph metrics BearingX %ld BearingY %ld Advance %ld w %ld h %ld "\
            "bbox xMin %ld xMax %ld yMin %ld yMax %ld\n",
            __func__, __LINE__, unicode, size,
            bitmap_glyph->left, bitmap_glyph->top,
            bitmap_glyph->bitmap.width, bitmap_glyph->bitmap.rows,
            bitmap_glyph->bitmap.pitch,
            face->size->metrics.ascender >> 6,
            face->size->metrics.descender >> 6,
            face->size->metrics.height >> 6,
            face->size->metrics.max_advance >> 6,
            face->glyph->metrics.horiBearingX >> 6,
            face->glyph->metrics.horiBearingY >> 6,
            face->glyph->metrics.horiAdvance >> 6,
            face->glyph->metrics.width >> 6,
            face->glyph->metrics.height >> 6,
            bbox.xMin, bbox.xMax, bbox.yMin, bbox.yMax);

        bitmap->w = bitmap_glyph->bitmap.width;
        bitmap->h = bitmap_glyph->bitmap.rows;
        bitmap->pitch = bitmap_glyph->bitmap.pitch;
        bitmap->left = bitmap_glyph->left;
        bitmap->top = bitmap_glyph->top;
        bitmap->size = size;
        bitmap->unicode = unicode;
        buffer = bitmap_glyph->bitmap.buffer;
        uint32_t bufSize = sizeof(uint8_t) * size * size;
        bitmap->buffer = (uint8_t *)calloc(1, bufSize);
        if (ISNULL(bitmap->buffer)) {
            LOGE("%s: %d failed to alloc %d bytes\n",
                __func__, __LINE__, bufSize);
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = copyBitmap(bitmap, buffer, bitmap->buffer);
        if (!SUCCEED(rc)) {
            LOGE("%s: %d failed to copy bitmap\n", __func__, __LINE__);
        }
    }

    if (SUCCEED(rc)) {
        if (!ISNULL(glyph)) {
            FT_Done_Glyph(glyph);
            glyph = NULL;
        }

        if (!ISNULL(face)) {
            FT_Done_Face(face);
            face = NULL;
        }
    }

    return rc;
}

int32_t printBitmap(_bitmap_t *bitmap)
{
    printf("\n    ");
    for (int32_t i = 0; i < bitmap->pitch; i++) {
        printf("%d", (i+ 1) % 10);
    }

    printf("\n");
    for (uint32_t i = 0; i < bitmap->h; i++) {
        printf("%3d ", i + 1);
        for (int32_t j = 0; j < bitmap->pitch; j++) {
            if (*(bitmap->buffer + i * bitmap->pitch + j)) {
                printf("*");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }

    return NO_ERROR;
}

int32_t printBitmapTable(uint32_t num, _bitmap_t *bitmap)
{
    uint32_t max = 0;

    for (uint32_t i = 0; i < num; i++) {
        max = max < bitmap[i].h ? bitmap[i].h : max;
    }

    printf("\n");
    for (uint32_t n = 0; n < num; n++) {
        printf("    ");
        for (int32_t w = 0; w < bitmap[n].pitch; w++) {
            printf("%d", (w + 1) % 10);
        }
    }

    printf("\n");
    for (uint32_t h = 0; h < max; h++) {
        for (uint32_t n = 0; n < num; n++) {
            printf("%3d ", h + 1);
            for (int32_t w = 0; w < bitmap[n].pitch; w++) {
                if (h < bitmap[n].h &&
                    *(bitmap[n].buffer + h * bitmap[n].pitch + w)) {
                    printf("*");
                } else {
                    printf(" ");
                }
            }
        }
        printf("\n");
    }

    return NO_ERROR;
}

int32_t printBitmaps(uint32_t num, _bitmap_t *bitmap)
{
    uint32_t i = 0;
    int32_t width = 0;
    uint32_t count = 0, printed = 0;

    for (i = 0; i < num; i++) {
        if ((width + bitmap[i].pitch) > MAX_LINE_LENGTH) {
            printBitmapTable(count, bitmap + printed);
            printed += count;
            count = 0;
            width = 0;
        }
        count ++;
        width += bitmap[i].pitch;
    }

    if (width > 0) {
        printBitmapTable(count, bitmap + printed);
    }

    return NO_ERROR;
}

int32_t destroyBitmap(uint32_t num, _bitmap_t *bitmap)
{
    for (uint32_t i = 0; i < num; i++) {
        if (!ISNULL(bitmap[i].buffer)) {
            free(bitmap[i].buffer);
        }
    }

    return NO_ERROR;
}

uint32_t chars[] = {
    0x30, // 0
    0x31, // 1
    0x61, // a
    0x62, // b
    0x66, // f
    0x67, // g
    0x4eae, // LIANG
    0x7c21, // JIAN
    0x3042, // A
};

int32_t main(int argc, char **argv)
{
    FT_Library lib = NULL;
    const uint32_t num = sizeof(chars) / sizeof(chars[0]);
    _bitmap_t bitmap[num];

    initLib(&lib);
    for (uint32_t i = 0; i < num; i++) {
        getBitmap(&lib, chars[i], FONT_PIXEL_SIZE, FONT_PATH, &bitmap[i]);
    }
    printBitmaps(num, bitmap);
    destroyBitmap(num, bitmap);
    deinitLib(&lib);

    return NO_ERROR;
}
