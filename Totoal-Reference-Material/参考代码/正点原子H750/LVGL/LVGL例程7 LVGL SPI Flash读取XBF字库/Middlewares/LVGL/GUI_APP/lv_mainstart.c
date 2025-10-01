/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-23
 * @brief       LVGL SPI Flash读取XBF字库 实验
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

LV_FONT_DECLARE(myFont14)
LV_FONT_DECLARE(myFont24)
LV_FONT_DECLARE(Font12)
LV_FONT_DECLARE(Font24)
LV_FONT_DECLARE(Font36)

lv_style_t  *font_style;
lv_obj_t    *font_label;

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_style_init(font_style);
    lv_style_set_text_font(font_style, &Font12);
    lv_style_set_text_color(font_style, lv_color_black());
    
    font_label = lv_label_create(lv_scr_act());
    lv_obj_add_style(font_label, font_style, 0);
    lv_label_set_text(font_label, "LVGL 正点原子XBF字库读取");
    lv_obj_align(font_label, LV_ALIGN_TOP_MID, 0, 0);
}
