/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-23
 * @brief       LVGL TTF字库 实验
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
#include "lv_font_ttf.h"


lv_font_t lv_font_ttf_36;

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_freetype_font_init(&lv_font_ttf_36, "0:/SYSTEM/EMWINFONT/TTF/comesinhandy.ttf", 36);
    lv_obj_t * label_36 = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(label_36, &lv_font_ttf_36, 0);
    lv_obj_set_style_text_color(label_36, lv_color_black(), 0);
    lv_label_set_text(label_36, "ALIENTEK LVGL TTF Font comesinhandy.ttf!");
    lv_obj_set_pos(label_36, 0, 0);
}
