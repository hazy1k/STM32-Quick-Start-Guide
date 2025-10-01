/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL 二维码显示
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


const char * data = "Hello ALIENTEK(正点原子) ";

/**
 * @brief  二维码显示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    /* 创建一个标签 */
    lv_obj_t *label_time = lv_label_create(lv_scr_act());
    /* 设置标签的文本 */
    lv_label_set_text(label_time,"ALIENTEK_QR");
    /* 设置标签的文本字体颜色 */
    lv_obj_set_style_text_color(label_time,lv_palette_main(LV_PALETTE_RED),LV_STATE_DEFAULT);
    /* 设置标签的文本字体 */
    lv_obj_set_style_text_font(label_time,&lv_font_montserrat_32,LV_STATE_DEFAULT);
    /* 设置标签的顶部中间对齐 */
    lv_obj_align(label_time,LV_ALIGN_TOP_MID,0,0);
    /* 创建一个lcddev.width/2的二维码 */
    lv_obj_t * qr = lv_qrcode_create(lv_scr_act(), lv_obj_get_width(lv_scr_act())/2, lv_color_hex3(0x33f), lv_color_hex3(0xeef));
    /* 设置数据 */
    lv_qrcode_update(qr, data, strlen(data));
    /* 二维码中间对齐 */
    lv_obj_align(qr,LV_ALIGN_CENTER,0,0);
}
