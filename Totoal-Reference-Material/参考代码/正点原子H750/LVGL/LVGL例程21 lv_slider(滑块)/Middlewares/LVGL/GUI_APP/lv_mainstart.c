/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_slider(滑块) 实验
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

static const lv_font_t *font;                                                       /* 定义字体 */
static lv_obj_t *slider_label;                                                      /* 进度条百分比标签 */


/**
 * @brief  滑块事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void slider_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);                                      /* 获取触发源 */
    lv_event_code_t code = lv_event_get_code(e);                                    /* 获取事件类型 */

    if(code == LV_EVENT_VALUE_CHANGED)
    {
        lv_label_set_text_fmt(slider_label, "%d%%", lv_slider_get_value(target));   /* 获取当前值，更新进度条百分比 */
    }
}

/**
 * @brief  音量调节滑块
 * @param  无
 * @return 无
 */
static void lv_example_slider(void)
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

    /* 滑块 */
    lv_obj_t * slider = lv_slider_create(lv_scr_act());                             /* 创建滑块 */
    lv_obj_set_size(slider, scr_act_width() / 2, 20);                               /* 设置大小 */
    lv_obj_center(slider);                                                          /* 设置位置 */
    lv_slider_set_value(slider, 50, LV_ANIM_OFF);                                   /* 设置当前值 */
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);     /* 添加事件 */

    /* 百分比标签 */
    slider_label = lv_label_create(lv_scr_act());                                   /* 创建百分比标签 */
    lv_label_set_text(slider_label, "50%");                                         /* 设置文本内容 */
    lv_obj_set_style_text_font(slider_label, font, LV_STATE_DEFAULT);               /* 设置字体 */
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_RIGHT_MID, 20, 0);           /* 设置位置 */

    /* 音量图标 */
    lv_obj_t *sound_label = lv_label_create(lv_scr_act());                          /* 创建音量标签 */
    lv_label_set_text(sound_label, LV_SYMBOL_VOLUME_MAX);                           /* 设置文本内容：音量图标 */
    lv_obj_set_style_text_font(sound_label, font, LV_STATE_DEFAULT);                /* 设置字体 */
    lv_obj_align_to(sound_label, slider, LV_ALIGN_OUT_LEFT_MID, -20, 0);            /* 设置位置 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_slider();
}




