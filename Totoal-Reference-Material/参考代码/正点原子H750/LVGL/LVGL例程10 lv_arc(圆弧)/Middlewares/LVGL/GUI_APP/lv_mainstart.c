/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_arc(圆弧) 实验
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

static lv_obj_t *label_left;            /* 左侧百分比标签 */
static lv_obj_t *label_right;           /* 右侧百分比标签 */
static lv_obj_t *arc_left;              /* 左侧圆弧 */
static lv_obj_t *arc_right;             /* 右侧圆弧 */

static const lv_font_t *font;           /* 定义字体 */
static uint8_t arc_width;               /* 圆弧宽度 */

/**
 * @brief  圆弧事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void arc_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);                                  /* 获取触发源 */
    lv_event_code_t code = lv_event_get_code(e);                                /* 获取事件类型 */

    if(code == LV_EVENT_VALUE_CHANGED)
    {
        lv_label_set_text_fmt(label_left, "%d%%", lv_arc_get_value(target));    /* 获取当前值，更新左侧百分比标签 */
        lv_label_set_text_fmt(label_right, "%d%%", lv_arc_get_value(target));   /* 获取当前值，更新右侧百分比标签 */
        lv_arc_set_value(arc_right, lv_arc_get_value(target));                  /* 获取左侧圆弧当前值，更新右侧圆弧当前值 */
    }
}

/**
 * @brief  左侧圆弧
 * @param  无
 * @return 无
 */
void lv_example_arc1(void)
{
    /* 根据活动屏幕宽度选择字体 */
    if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
        arc_width = 10;
    }
    else
    {
        font = &lv_font_montserrat_30;
        arc_width = 20;
    }

    /* 左侧圆弧 */
    arc_left = lv_arc_create(lv_scr_act());                                     /* 创建圆弧 */
    lv_obj_set_size(arc_left, scr_act_height() * 3/8, scr_act_height() * 3/8);  /* 设置大小 */
    lv_obj_align(arc_left, LV_ALIGN_CENTER, -scr_act_width()/5, 0);             /* 设置位置 */
    lv_arc_set_value(arc_left, 0);                                              /* 设置当前值 */
    lv_obj_set_style_arc_width(arc_left, arc_width, LV_PART_MAIN);              /* 设置背景弧宽度 */
    lv_obj_set_style_arc_width(arc_left, arc_width, LV_PART_INDICATOR);         /* 设置前景弧宽度 */
    lv_obj_add_event_cb(arc_left, arc_event_cb, LV_EVENT_VALUE_CHANGED, NULL);  /* 添加事件 */

    /* 左侧百分比标签 */
    label_left = lv_label_create(lv_scr_act());                                 /* 创建百分比标签 */
    lv_obj_align(label_left, LV_ALIGN_CENTER, -scr_act_width()/5, 0);           /* 设置位置 */
    lv_label_set_text(label_left, "0%");                                        /* 设置文本 */
    lv_obj_set_style_text_font(label_left, font, LV_STATE_DEFAULT);             /* 设置字体 */
}

/**
 * @brief  右侧圆弧
 * @param  无
 * @return 无
 */
void lv_example_arc2(void)
{
    /* 右侧圆弧 */
    arc_right = lv_arc_create(lv_scr_act());                                    /* 创建圆弧 */
    lv_obj_set_size(arc_right, scr_act_height() * 3/8, scr_act_height() * 3/8); /* 设置大小 */
    lv_obj_align(arc_right, LV_ALIGN_CENTER, scr_act_width()/5, 0);             /* 设置位置 */
    lv_arc_set_value(arc_right, 0);                                             /* 设置当前值 */
    lv_arc_set_bg_angles(arc_right, 0, 360);                                    /* 设置背景弧角度 */
    lv_arc_set_rotation(arc_right, 270);                                        /* 设置旋转角度 */
    lv_obj_remove_style(arc_right, NULL, LV_PART_KNOB);                         /* 去除旋钮 */
    lv_obj_clear_flag(arc_right, LV_OBJ_FLAG_CLICKABLE);                        /* 去除可点击属性 */
    lv_obj_set_style_arc_width(arc_right, arc_width, LV_PART_MAIN);             /* 设置背景弧宽度 */
    lv_obj_set_style_arc_width(arc_right, arc_width, LV_PART_INDICATOR);        /* 设置前景弧宽度 */

    /* 右侧百分比标签 */
    label_right = lv_label_create(lv_scr_act());                                /* 创建百分比标签 */
    lv_obj_align(label_right, LV_ALIGN_CENTER, scr_act_width()/5, 0);           /* 设置位置 */
    lv_label_set_text(label_right, "0%");                                       /* 设置文本 */
    lv_obj_set_style_text_font(label_right, font, LV_STATE_DEFAULT);            /* 设置字体 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_arc1();       /* 左侧圆弧 */
    lv_example_arc2();       /* 右侧圆弧 */
}
