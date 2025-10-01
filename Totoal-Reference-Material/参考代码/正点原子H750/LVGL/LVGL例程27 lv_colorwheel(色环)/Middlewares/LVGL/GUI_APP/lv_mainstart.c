/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_colorwheel(色环) 实验
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

static lv_obj_t* obj;

/**
 * @brief  色环事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void colorwheel_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);                                          /* 获取触发源 */

    lv_obj_set_style_bg_color(obj, lv_colorwheel_get_rgb(target), LV_PART_MAIN);        /* 设置基础对象背景颜色 */
}

/**
 * @brief  色环部件实例
 * @param  无
 * @return 无
 */
static void lv_example_colorwheel(void)
{
    /* 色环（用于选择颜色） */
    lv_obj_t *colorwheel = lv_colorwheel_create(lv_scr_act(), true);                    /* 创建色环 */
    lv_obj_set_size(colorwheel, scr_act_height() * 2 / 3, scr_act_height()* 2 / 3);     /* 设置大小 */
    lv_obj_center(colorwheel);                                                          /* 设置位置 */
    lv_obj_set_style_arc_width(colorwheel, scr_act_height() * 0.1, LV_PART_MAIN);       /* 设置色环圆弧宽度 */
    lv_colorwheel_set_mode_fixed(colorwheel, true);                                     /* 固定色环模式 */

    /* 基础对象（用于显示所选颜色） */
    obj = lv_obj_create(lv_scr_act());                                                  /* 创建基础对象 */
    lv_obj_set_size(obj, scr_act_height() / 3, scr_act_height() / 3);                   /* 设置大小 */
    lv_obj_align_to(obj, colorwheel, LV_ALIGN_CENTER, 0, 0);                            /* 设置位置 */
    lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, LV_PART_MAIN);                       /* 设置圆角 */
    lv_obj_set_style_bg_color(obj, lv_colorwheel_get_rgb(colorwheel), LV_PART_MAIN);    /* 设置背景颜色 */
    lv_obj_add_event_cb(colorwheel, colorwheel_event_cb, LV_EVENT_VALUE_CHANGED, NULL); /* 设置色环事件回调 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_colorwheel();
}


