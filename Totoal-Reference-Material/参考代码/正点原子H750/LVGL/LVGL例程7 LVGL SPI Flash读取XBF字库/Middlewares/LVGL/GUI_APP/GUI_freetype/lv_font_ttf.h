#ifndef LV_FONT_TTF_H
#define LV_FONT_TTF_H

#include <stdint.h>
#include "lvgl/src/font/lv_font.h"
#include "ft2build.h"
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "./FATFS/source/ff.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_CACHE_H


#define USE_CACHE_MANGER   0

/**
 * ttf字体文件的大小(字节)
 */
#define TTF_SIZE_MEMORY 30*1024

typedef struct {
    FT_Face     face;      /* 面向对象句柄 */
    uint16_t font_size;    /* 字体高度大小 */
}lv_font_fmt_freetype_dsc_t;

/* ttf字体初始化 */
int lv_font_ttf_init(uint8_t max_faces);
int lv_freetype_font_init(lv_font_t * font,const char * font_path, uint16_t font_size);

#endif
