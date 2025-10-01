/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_label(标签) 实验
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


#if (1 != LV_FONT_MONTSERRAT_10 || 1 != LV_FONT_MONTSERRAT_14 || 1 != LV_FONT_MONTSERRAT_20)
#error Please Make Sure Enable LV_FONT_MONTSERRAT_10 & LV_FONT_MONTSERRAT_14 & LV_FONT_MONTSERRAT_20
#endif

/* 获取当前活动屏幕的宽高 */
#define scr_act_width() lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

static const lv_font_t *font;

/**
 * @brief  例1
 * @param  无
 * @return 无
 */
static void lv_example_label_1(void)
{
    if (scr_act_width() <= 320)
        font = &lv_font_montserrat_10;
    else if (scr_act_width() <= 480)
        font = &lv_font_montserrat_14;
    else
        font = &lv_font_montserrat_20;

    lv_obj_t* label = lv_label_create(lv_scr_act());                                /* 定义并创建标签 */
    lv_label_set_text(label, "#0000ff Re-color# #ff00ff words# #ff0000 of a# "      /* 设置标签文本 */
                             "label, align the lines to the center"
                             "and  wrap long text automatically.");
    lv_label_set_recolor(label, true);                                              /* 启用标签文本重新着色 */
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);                          /* 设置标签文本字体 */
    lv_obj_set_width(label, scr_act_width() / 3);                                   /* 设置标签宽度 */
    lv_obj_align(label, LV_ALIGN_CENTER, -scr_act_width() / 3, 0);                  /* 设置标签位置 */
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);         /* 设置标签文本对齐方式 */
}

/**
 * @brief  例2
 * @param  无
 * @return 无
 */
static void lv_example_label_2(void)
{
    lv_obj_t* label = lv_label_create(lv_scr_act());                                /* 定义并创建标签 */
    lv_label_set_text(label, "It is a circularly scrolling text. ");                /* 设置标签文本 */
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);                          /* 设置标签文本字体 */
    lv_obj_set_width(label, scr_act_width() / 3);                                   /* 设置标签宽度 */
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);                   /* 设置标签长模式：循环滚动 */
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);                                     /* 设置标签位置 */
}

/**
 * @brief  例3
 * @param  无
 * @return 无
 */
static void lv_example_label_3(void)
{
    lv_obj_t* label = lv_label_create(lv_scr_act());                                /* 定义并创建标签 */
    lv_label_set_text_fmt(label, "Label can set text like %s", "printf");           /* 设置标签文本 */
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);                          /* 设置标签文本字体 */
    lv_obj_set_width(label, scr_act_width() / 3);                                   /* 设置标签宽度 */
    lv_obj_align(label, LV_ALIGN_CENTER, scr_act_width() / 3, 0);                   /* 设置标签位置 */
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);         /* 设置标签文本对齐方式 */

    lv_obj_t* label_shadow = lv_label_create(lv_scr_act());                         /* 定义并创建阴影标签 */
    lv_label_set_text(label_shadow, lv_label_get_text(label));                      /* 设置标签文本 */
    lv_obj_set_style_text_font(label_shadow, font, LV_PART_MAIN);                   /* 设置标签文本字体 */
    lv_obj_set_width(label_shadow, scr_act_width() / 3);                            /* 设置标签宽度 */
    lv_obj_set_style_text_opa(label_shadow, LV_OPA_30, LV_PART_MAIN);               /* 设置标签文本透明度 */
    lv_obj_set_style_text_color(label_shadow, lv_color_black(), LV_PART_MAIN);      /* 设置标签文本颜色 */
    lv_obj_set_style_text_align(label_shadow, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);  /* 设置标签文本对齐方式 */
    lv_obj_align_to(label_shadow, label, LV_ALIGN_TOP_LEFT, 3, 3);                  /* 设置标签位置 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_label_1();
    lv_example_label_2();
    lv_example_label_3();
}



