/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL BMP图片读取实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 Mini Pro H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */
 
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>


typedef struct
{
    char *img_path;
    char *label_text;
}img_info_t;

static const img_info_t draw_img[] =
{
    {"0:/PICTURE/BMP/camera.bmp", "camera"},
    {"0:/PICTURE/BMP/ALIENTEKLOGO.bmp", "ALIENTEKLOGO"},
};

#define img_Num sizeof(draw_img) / sizeof(draw_img[0])

lv_obj_t *img;

/**
 * @brief       lv_mainstart
 * @param       无
 * @retval      无
 */
void lv_mainstart()
{
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label,"BMP_Decoder");
    lv_obj_set_style_text_color(label,lv_palette_main(LV_PALETTE_RED),LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label,&lv_font_montserrat_32,LV_STATE_DEFAULT);
    lv_obj_align_to(label,NULL,LV_ALIGN_TOP_MID,0,0);
    lv_obj_set_style_bg_color(lv_scr_act(),lv_palette_main(LV_PALETTE_BLUE),LV_STATE_DEFAULT);

    lv_bmp_init();                                    /* BMP解码器初始化 */
    img = lv_img_create(lv_scr_act());                /* 创建图像 */
    lv_img_set_src(img, draw_img[0].img_path);        /* 设置图像源 */
    lv_obj_align_to(img, NULL, LV_ALIGN_CENTER, 0, 0);/* 设置对齐模式 */
}
