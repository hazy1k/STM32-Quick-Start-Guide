/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_bar(进度条) 实验
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

static int16_t val = 0;                         /* 当前值 */

static const lv_font_t *font;                   /* 定义字体 */

static lv_obj_t *label_load;                    /* 加载标题标签 */
static lv_obj_t *label_per;                     /* 百分比标签 */
static lv_obj_t *bar;                           /* 进度条 */

/**
 * @brief  定时器回调
 * @param  *timer ：该定时器相关的数据
 * @return 无
 */
static void timer_cb(lv_timer_t *timer)
{
    if(val < 100)                                                           /* 当前值小于100 */
    {
        val ++;
        lv_bar_set_value(bar, val, LV_ANIM_ON);                             /* 设置当前值 */
        lv_label_set_text_fmt(label_per, "%d %%", lv_bar_get_value(bar));   /* 获取当前值，更新显示 */
    }
    else                                                                    /* 当前值大于等于100 */
    {
        lv_label_set_text(label_per, "finished!");                          /* 加载完成 */
    }
}

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
    lv_obj_align(label_load, LV_ALIGN_CENTER, 0, -scr_act_height() / 10 );

    /* 百分比标签 */
    label_per = lv_label_create(lv_scr_act());
    lv_label_set_text(label_per, "%0");
    lv_obj_set_style_text_font(label_per, font, LV_STATE_DEFAULT);
    lv_obj_align(label_per, LV_ALIGN_CENTER, 0, scr_act_height() / 10 );
}

/**
 * @brief  加载进度条
 * @param  无
 * @return 无
 */
static void lv_example_bar(void)
{
    bar = lv_bar_create(lv_scr_act());                            /* 创建进度条 */
    lv_obj_set_align(bar, LV_ALIGN_CENTER);                       /* 设置位置 */
    lv_obj_set_size(bar, scr_act_width() * 3 / 5, 20);            /* 设置大小 */
    lv_obj_set_style_anim_time(bar, 100, LV_STATE_DEFAULT);       /* 设置动画时间 */
    lv_timer_create(timer_cb, 100, NULL);                         /* 初始化定时器 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_label();                                           /* 加载提示标签 */
    lv_example_bar();                                             /* 加载进度条 */
}
