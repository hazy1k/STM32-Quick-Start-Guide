#include <stddef.h>
#include "lvgl/lvgl.h"
#include "ft2build.h"
#include "lvgl/src/font/lv_font.h"
#include "lv_font_ttf.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_CACHE_H


/**
 * ttf字体文件的存储空间
 */
uint8_t  ttf_memory[TTF_SIZE_MEMORY];

const uint8_t * lv_font_ttf_get_bitmap(const lv_font_t * font, uint32_t unicode_letter);
int16_t lv_font_ttf_get_width(const lv_font_t * font, uint32_t unicode_letter);

static FT_Library library;
static FTC_Manager cache_manager;
//static FTC_CMapCache cmap_cache;
/*static FTC_ImageCache image_cache;*/
//static FTC_SBitCache sbit_cache;
//static FTC_SBit sbit;

#if USE_CACHE_MANGER

static FT_Error  font_Face_Requester(FTC_FaceID  face_id,
                                     FT_Library  library,
                                     FT_Pointer  req_data,
                                     FT_Face*    aface)
{
    *aface = face_id;

    return FT_Err_Ok;
}
static bool get_glyph_dsc_cache_cb(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next)
{
    if(unicode_letter < 0x20) {
        dsc_out->adv_w = 0;
        dsc_out->box_h = 0;
        dsc_out->box_w = 0;
        dsc_out->ofs_x = 0;
        dsc_out->ofs_y = 0;
        dsc_out->bpp = 0;
        return true;
    }

    if(lv_font_get_glyph_dsc(&lv_font_montserrat_20, dsc_out, unicode_letter, unicode_letter_next) == true)
        return true;

    FT_UInt glyph_index;
    FT_UInt charmap_index;
    FT_Face face;
    lv_font_fmt_freetype_dsc_t * dsc = (lv_font_fmt_freetype_dsc_t *)(font->user_data);
    face = dsc->face;
    FTC_ImageTypeRec desc_sbit_type;

    desc_sbit_type.face_id = (FTC_FaceID)face;
    desc_sbit_type.flags = FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL;
    desc_sbit_type.height = dsc->font_size;
    desc_sbit_type.width = 0;

    /*FTC_Manager_LookupFace(cache_manager, face, &get_face);*/
    charmap_index = FT_Get_Charmap_Index(face->charmap);
    glyph_index = FTC_CMapCache_Lookup(cmap_cache, face, charmap_index, unicode_letter);
    FTC_SBitCache_Lookup(sbit_cache, &desc_sbit_type, glyph_index, &sbit, NULL);

    dsc_out->adv_w = sbit->xadvance;
    dsc_out->box_h = sbit->height;         /*Height of the bitmap in [px]*/
    dsc_out->box_w = sbit->width;          /*Width of the bitmap in [px]*/
    dsc_out->ofs_x = sbit->left;           /*X offset of the bitmap in [pf]*/
    dsc_out->ofs_y = sbit->top - sbit->height;         /*Y offset of the bitmap measured from the as line*/
    dsc_out->bpp = 8;           /* Bit per pixel: 1/2/4/8*/

    return true;                /* true: glyph found; false: glyph was not found*/
}

/* Get the bitmap of `unicode_letter` from `font`. */
static const uint8_t * get_glyph_bitmap_cache_cb(const lv_font_t * font, uint32_t unicode_letter)
{
    uint8_t *symbol = lv_font_get_glyph_bitmap(&lv_font_montserrat_20, unicode_letter);
    if( symbol != NULL)
        return(const uint8_t *)symbol;

    return (const uint8_t *)sbit->buffer;
}
#else
static bool get_glyph_dsc_cb(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next)
{
    if(unicode_letter < 0x20)
    {
        dsc_out->adv_w = 0;
        dsc_out->box_h = 0;
        dsc_out->box_w = 0;
        dsc_out->ofs_x = 0;
        dsc_out->ofs_y = 0;
        dsc_out->bpp = 0;
        return true;
    }

    int error;
    FT_Face face;
    lv_font_fmt_freetype_dsc_t * dsc = (lv_font_fmt_freetype_dsc_t *)(font->user_data);
    face = dsc->face;

    FT_UInt glyph_index = FT_Get_Char_Index( face, unicode_letter );

    error = FT_Load_Glyph(
            face,               /* 面向对象句柄 */
            glyph_index,        /* 字形索引 */
            FT_LOAD_DEFAULT );  /* 加载标志，见右边 */ /* FT_LOAD_MONOCHROME|FT_LOAD_NO_AUTOHINTING */
    
    if ( error )
    {
        printf("Error in FT_Load_Glyph: %d\n", error);
        return error;
    }

    error = FT_Render_Glyph( face->glyph,   /* 字形槽 */
            FT_RENDER_MODE_NORMAL );        /* 渲染模式 */

    if ( error )
    {
        printf("Error in FT_Render_Glyph: %d\n", error);
        return error;
    }

    dsc_out->adv_w = (face->glyph->metrics.horiAdvance >> 6);
    dsc_out->box_h = face->glyph->bitmap.rows;          /* 位图的高度，单位为[px] */
    dsc_out->box_w = face->glyph->bitmap.width;         /* 位图宽度[px] */
    dsc_out->ofs_x = face->glyph->bitmap_left;          /* X位图在[pf]中的偏移量 */
    dsc_out->ofs_y = face->glyph->bitmap_top - face->glyph->bitmap.rows; /* 从线测量的位图的Y偏移量 */
    dsc_out->bpp = 8;         /*Bit per pixel: 1/2/4/8*/

    return true;
}

/* 从' font '获得' unicode_letter '的位图 */
static const uint8_t * get_glyph_bitmap_cb(const lv_font_t * font, uint32_t unicode_letter)
{
    FT_Face face;
    lv_font_fmt_freetype_dsc_t * dsc = (lv_font_fmt_freetype_dsc_t *)(font->user_data);
    face = dsc->face;
    return (const uint8_t *)(face->glyph->bitmap.buffer);
}
#endif

/**
* @brief 初始化Freetype字库
* @param max_faces 此缓存管理的打开的@FT_Face对象的最大数目
* @retval FT_Error
* 例如:如果你有两个面，max_faces应该>= 2
*/
int lv_freetype_init(uint8_t max_faces)
{
    FT_Error error;
    error = FT_Init_FreeType(&library);
    if ( error )
    {
        printf("Error in FT_Init_FreeType: %d\n", error);
        return error;
    }
#if USE_CACHE_MANGER
    error = FTC_Manager_New(library, max_faces, 1, 0, font_Face_Requester, NULL, &cache_manager);
    if(error)
    {
        printf("Failed to open cache manager\r\n");
        return error;
    }

    error = FTC_CMapCache_New(cache_manager, &cmap_cache);
    if(error)
    {
        printf("Failed to open Cmap Cache\r\n");
        return error;
    }

    error = FTC_ImageCache_New(cache_manager, &image_cache);
    if(error)
    {
        printf("Failed to open image cache\r\n");
        return error;
    }

    error = FTC_SBitCache_New(cache_manager, &sbit_cache);
    if(error)
    {
        printf("Failed to open sbit cache\r\n");
        return error;
    }
#endif
    return FT_Err_Ok;
}

FIL TTFFontFile;
/**
 * @brief       加载TTF字库
 * @param       pash : 字库路径
 * @retval      无
 */
void lv_load_ttffont(const char * pash)
{
    int result;
    uint16_t bread;
    result = f_open(&TTFFontFile, pash, FA_READ);  /* 打开字库文件 */ 
  
    if (result != FR_OK)
    {
        printf("打开字库文件失败\r\n");
    }


    result = f_read(&TTFFontFile,(char *)ttf_memory, sizeof(ttf_memory), (UINT *)&bread); /* 读取数据 */


    if (result != FR_OK) 
    {
        printf("文件读取失败，跳出\r\n"); /* 文件打开失败，跳出 */
    }


    f_close(&TTFFontFile); /* 关闭TTFFointFile文件*/
}

/**
* @brief初始化字体结构体
* @param font 指向字体的指针
* @param font_path 字体路径
* @param font_size 字体高度
* @retval FT_Error
*/
int lv_freetype_font_init(lv_font_t * font,const char * font_path, uint16_t font_size)
{
    FT_Error error;
    lv_load_ttffont(font_path);
    
    lv_font_fmt_freetype_dsc_t * dsc = lv_mem_alloc(sizeof(lv_font_fmt_freetype_dsc_t));
//    LV_ASSERT_MEM(dsc);
    if(dsc == NULL) return FT_Err_Out_Of_Memory;

    dsc->font_size = font_size;

//    error = FT_New_Face(library, font_path, 0, &dsc->face);
//  
//    if ( error )
//    {
//        printf("Error in FT_New_Face: %d\n", error);
//        return error;
//    }
//    error = FT_Set_Pixel_Sizes(dsc->face, 0,font_size);
//    if ( error )
//    {
//        printf("Error in FT_Set_Char_Size: %d\n", error);
//        return error;
//    }

    error = FT_Init_FreeType(&library);

    if(error)
    {
        printf("Failed to init FreeType library: %d\n", error);
        goto err1;
    }

    error = FT_New_Memory_Face(library, ttf_memory, TTF_SIZE_MEMORY, 0,
                               &dsc->face);
    if(error)
    {
        printf("Failed to open font file: %d\n", error);
        goto err2;
    }

    error = FT_Set_Pixel_Sizes(dsc->face, 0, font_size);

    if(error)
    {
        printf("Failed to Set Pixel Sizes: %d\n", error);
        goto err3;
    }
    
#if USE_CACHE_MANGER
    font->get_glyph_dsc = get_glyph_dsc_cache_cb;        /* 设置回调以获取关于字形的信息 */
    font->get_glyph_bitmap = get_glyph_bitmap_cache_cb;  /* 设置回调函数来获取glyp的位图 */
#else
    font->get_glyph_dsc = get_glyph_dsc_cb;        /* 设置回调以获取关于字形的信息 */
    font->get_glyph_bitmap = get_glyph_bitmap_cb;  /* 设置回调函数来获取glyp的位图 */
#endif

#ifndef LV_USE_USER_DATA
#error "lv_freetype : user_data is required.Enable it lv_conf.h(LV_USE_USER_DATA 1)"
#endif
    font->user_data = dsc;
    font->line_height = (dsc->face->size->metrics.height >> 6);
    font->base_line = -(dsc->face->size->metrics.descender >> 6);  /* 基线从line - height的顶部开始测量 */
    font->subpx = LV_FONT_SUBPX_NONE;

    return FT_Err_Ok;
    
    FTC_Manager_Done(cache_manager);
err3:
    FT_Done_Face(dsc->face);
err2:
    FT_Done_FreeType(library);
err1:
    return -1;
}
