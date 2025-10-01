/**
 * @file lv_bmp.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"
#if LV_USE_BMP

#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t reserved;
}rgbquad_t;

typedef struct {
    lv_fs_file_t f;
    unsigned int px_offset;
    int px_width;
    int px_height;
    unsigned int bpp;
    int row_size_bytes;
    rgbquad_t palette[256];
} bmp_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t decoder_info(lv_img_decoder_t * decoder, const void * src, lv_img_header_t * header);
static lv_res_t decoder_open(lv_img_decoder_t * dec, lv_img_decoder_dsc_t * dsc);


static lv_res_t decoder_read_line(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc,
                                                 lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf);

static void decoder_close(lv_img_decoder_t * dec, lv_img_decoder_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_bmp_init(void)
{

    lv_img_decoder_t * dec = lv_img_decoder_create();
    lv_img_decoder_set_info_cb(dec, decoder_info);
    lv_img_decoder_set_open_cb(dec, decoder_open);
    lv_img_decoder_set_read_line_cb(dec, decoder_read_line);
    lv_img_decoder_set_close_cb(dec, decoder_close);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Get info about a PNG image
 * @param src can be file name or pointer to a C array
 * @param header store the info here
 * @return LV_RES_OK: no error; LV_RES_INV: can't get the info
 */
static lv_res_t decoder_info(lv_img_decoder_t * decoder, const void * src, lv_img_header_t * header)
{
    (void) decoder; /*Unused*/
     lv_img_src_t src_type = lv_img_src_get_type(src);          /*Get the source type*/

     /*If it's a BMP file...*/
     if(src_type == LV_IMG_SRC_FILE) {
         const char * fn = src;
         if(!strcmp(&fn[strlen(fn) - 3], "bmp")) {              /*Check the extension*/
             /*Save the data in the header*/
             lv_fs_file_t f;
             lv_fs_res_t res = lv_fs_open(&f, src, LV_FS_MODE_RD);
             if(res != LV_FS_RES_OK) return LV_RES_INV;
             uint8_t headers[54];

             lv_fs_read(&f, headers, 54, NULL);
             uint32_t w;
             uint32_t h;
             memcpy(&w, headers + 18, 4);
             memcpy(&h, headers + 22, 4);
             header->w = w;
             header->h = h;
             header->always_zero = 0;
             lv_fs_close(&f);
#if LV_COLOR_DEPTH == 32
             uint16_t bpp;
             memcpy(&bpp, headers + 28, 2);
             header->cf = bpp == 32 ? LV_IMG_CF_TRUE_COLOR_ALPHA : LV_IMG_CF_TRUE_COLOR;
#else
             header->cf = LV_IMG_CF_TRUE_COLOR;
#endif
             return LV_RES_OK;
         }
     }
     /* BMP file as data not supported for simplicity.
      * Convert them to LVGL compatible C arrays directly. */
     else if(src_type == LV_IMG_SRC_VARIABLE) {
         return LV_RES_INV;
     }

     return LV_RES_INV;         /*If didn't succeeded earlier then it's an error*/
}


/**
 * Open a PNG image and return the decided image
 * @param src can be file name or pointer to a C array
 * @param style style of the image object (unused now but certain formats might use it)
 * @return pointer to the decoded image or  `LV_IMG_DECODER_OPEN_FAIL` if failed
 */
static lv_res_t decoder_open(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc)
{

    (void) decoder; /*Unused*/

    /*If it's a PNG file...*/
    if(dsc->src_type == LV_IMG_SRC_FILE) {
        const char * fn = dsc->src;

        if(strcmp(&fn[strlen(fn) - 3], "bmp")) return LV_RES_INV;       /*Check the extension*/

        bmp_dsc_t b;
        memset(&b, 0x00, sizeof(b));

        lv_fs_res_t res = lv_fs_open(&b.f, dsc->src, LV_FS_MODE_RD);
        if(res == LV_RES_OK) return LV_RES_INV;

        uint8_t header[54];
        lv_fs_read(&b.f, header, 54, NULL);

        if (0x42 != header[0] || 0x4d != header[1]) {
            return LV_RES_INV;
        }

        memcpy(&b.px_offset, header + 10, 4);
        memcpy(&b.px_width, header + 18, 4);
        memcpy(&b.px_height, header + 22, 4);
        memcpy(&b.bpp, header + 28, 2);
        b.row_size_bytes = (b.bpp * b.px_width) / 8;

        dsc->user_data = lv_mem_alloc(sizeof(bmp_dsc_t));
        LV_ASSERT_MALLOC(dsc->user_data);
        if(dsc->user_data == NULL) return LV_RES_INV;
        memcpy(dsc->user_data, &b, sizeof(b));

        dsc->img_data = NULL;
        return LV_RES_OK;
    }
    /* BMP file as data not supported for simplicity.
     * Convert them to LVGL compatible C arrays directly. */
    else if(dsc->src_type == LV_IMG_SRC_VARIABLE) {
        return LV_RES_INV;
    }

    return LV_RES_INV;    /*If not returned earlier then it failed*/
}


static lv_res_t decoder_read_line(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc,
                                                 lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf)
{

    bmp_dsc_t * b = dsc->user_data;
    y = (b->px_height - 1) - y; /*BMP images are stored upside down*/
    uint32_t p = b->px_offset + b->row_size_bytes * y;
    p += x * (b->bpp / 8);
    lv_fs_seek(&b->f, p, LV_FS_SEEK_SET);
    lv_fs_read(&b->f, buf, len * (b->bpp / 8), NULL);

    if((b->bpp >= 16) && (b->bpp == (sizeof(lv_color_t) * 8))) {
        lv_fs_read(&b->f, buf, len * (b->bpp / 8), NULL);
#if LV_COLOR_DEPTH == 32
        lv_color32_t * bufc = (lv_color32_t*)buf;
        for(uint32_t i = 0; i < len; i++) {
            /* Byte0:blue, byte1:green, byte2:red, byte3:alpha */
            uint8_t *p = (uint8_t *)&bufc[i];
            bufc[i].ch.red = p[2];
            bufc[i].ch.green = p[1];
            bufc[i].ch.blue = p[0];
            bufc[i].ch.alpha = 0xff; /*Amazing*/
        }
#endif
    }
    else {
        lv_color_t *color_p = (lv_color_t *)buf;
        uint8_t color_buf[4];
        switch(b->bpp)
        {
            case 32:
                for(int i = 0; i < len; i++)
                {
                    lv_fs_read(&b->f, color_buf, 4, NULL);
                    color_p[i] = lv_color_make(color_buf[2], color_buf[1], color_buf[0]);
                }
                break;
            case 24:
                for(int i = 0; i < len; i++)
                {
                    lv_fs_read(&b->f, color_buf, 3, NULL);
                    color_p[i] = lv_color_make(color_buf[2], color_buf[1], color_buf[0]);
                }
                break;
            case 16:
                for(int i = 0; i < len; i++)
                {
                    lv_fs_read(&b->f, color_buf, 2, NULL);
                    uint16_t color16 = *(uint16_t *)color_buf;

                    #define RGB565_R5(rgb565) ((rgb565 >> 11) & 0x1f)
                    #define RGB565_G6(rgb565) ((rgb565 >> 5) & 0x3f)
                    #define RGB565_B5(rgb565) ((rgb565 >> 0) & 0x1f)
                    #define R5_2_R8(r5) (r5 << 3)
                    #define G6_2_G8(g6) (g6 << 2)
                    #define B5_2_B8(b5) (b5 << 3)

                    color_p[i] = lv_color_make(R5_2_R8(RGB565_R5(color16)), 
                    G6_2_G8(RGB565_G6(color16)), B5_2_B8(RGB565_B5(color16)));
                }
                break;
            case 8:
                for(int i = 0; i < len; i++)
                {
                    lv_fs_read(&b->f, color_buf, 1, NULL);

                    uint8_t index = color_buf[0];
                    rgbquad_t color = b->palette[index];
                    color_p[i] = lv_color_make(color.red, color.green, color.blue);
                }
                break;
            case 4:
                for(int i = 0; i < len;)
                {
                    lv_fs_read(&b->f, color_buf, 1, NULL);

                    for(int j = 0; (j < 8) && (i < len); j += 4, i++) {
                        uint8_t index = (color_buf[0] >> (4 - j)) & 0x0f;
                        rgbquad_t color = b->palette[index];
                        color_p[i] = lv_color_make(color.red, color.green, color.blue);
                    }
                }
                break;
            case 1:
                for(int i = 0; i < len;) {
                    lv_fs_read(&b->f, color_buf, 1, NULL);

                    for(int j = 0; (j < 8) && (i < len); j++, i++) {
                        uint8_t index = (color_buf[0] >> (7 - j)) & 0x01;
                        rgbquad_t color = b->palette[index];
                        color_p[i] = lv_color_make(color.red, color.green, color.blue);               
                    }
                }
                break;
            default:
                break;
        }
    }

    return LV_RES_OK;
}


/**
 * Free the allocated resources
 */
static void decoder_close(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc)
{
    (void) decoder; /*Unused*/
    bmp_dsc_t * b = dsc->user_data;
    lv_fs_close(&b->f);
    lv_mem_free(dsc->user_data);

}

#endif /*LV_USE_BMP*/
