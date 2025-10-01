/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_spinner(加载器) 实验
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


/* 获取当前活动屏幕的宽高 */
#define scr_act_width() lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

static const lv_font_t *font;                   /* 定义字体 */
static lv_obj_t *spinner;                       /* 加载器 */
static lv_obj_t *label_load;                    /* 加载标题标签 */

/**
 * @brief  加载提示标签
 * @param  无
 * @return 无
 */
static void lv_example_label(void)
{
    /* 根据活动屏幕宽度选择字体 */
    if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
    }
    else
    {
        font = &lv_font_montserrat_20;
    }
    
    /* 加载标题标签 */
    label_load = lv_label_create(lv_scr_act());
    lv_label_set_text(label_load, "LOADING...");
    lv_obj_set_style_text_font(label_load, font, LV_STATE_DEFAULT);
    lv_obj_align(label_load, LV_ALIGN_CENTER, 0, scr_act_height() / 10 );
}

/**
 * @brief  加载器显示
 * @param  无
 * @return 无
 */
static void lv_example_spinner(void)
{
    spinner = lv_spinner_create(lv_scr_act(), 1000, 60);                            /* 创建加载器 */
    lv_obj_align(spinner, LV_ALIGN_CENTER, 0, -scr_act_height() / 15 );             /* 设置位置 */
    lv_obj_set_size(spinner, scr_act_height() / 5, scr_act_height() / 5);           /* 设置大小 */
    lv_obj_set_style_arc_width(spinner, scr_act_height() / 35, LV_PART_MAIN);       /* 设置主体圆弧宽度 */
    lv_obj_set_style_arc_width(spinner, scr_act_height() / 35, LV_PART_INDICATOR);  /* 设置指示器圆弧宽度 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_label();            /* 加载提示标签 */
    lv_example_spinner();          /* 加载器显示 */
}



