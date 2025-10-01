/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL JPEG图片显示实验
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


/**
 * @brief       lvgl程序入口
 * @param       无
 * @retval      无
 */
void lv_mainstart()
{
    /* 创建标签 */
    lv_obj_t *label = lv_label_create(lv_scr_act());
    /* 设置标签颜色 */
    lv_label_set_text(label,"JPEG_Decoder");
    /* 设置文本颜色 */
    lv_obj_set_style_text_color(label,lv_palette_main(LV_PALETTE_RED),LV_STATE_DEFAULT);
    /* 设置文本字体 */
    lv_obj_set_style_text_font(label,&lv_font_montserrat_32,LV_STATE_DEFAULT);
    /* 设置顶部中间对齐 */
    lv_obj_align(label,LV_ALIGN_TOP_MID,0,0);
    /* 设置背景颜色 */
    lv_obj_set_style_bg_color(lv_scr_act(),lv_palette_main(LV_PALETTE_BLUE),LV_STATE_DEFAULT);
    /* jpeg解码 */
    lv_split_jpeg_init();
    /* 创建image控件 */
    lv_obj_t * img = lv_img_create(lv_scr_act());
    /* 设置图像源 */
    lv_img_set_src(img, "0:/PICTURE/JPEG/SIM900A.jpg");
    /* 中间对齐 */
    lv_obj_align(img,LV_ALIGN_CENTER,0,0);
}
