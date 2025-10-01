/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_switch(开关) 实验
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

static lv_obj_t *switch_cool;            /* 制冷模式开关 */
static lv_obj_t *switch_heat;            /* 制暖模式开关 */
static lv_obj_t *switch_dry;             /* 干燥模式开关 */
static const lv_font_t *font;            /* 定义字体 */

/**
 * @brief  回调事件
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void switch_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);                      /* 获取触发源 */

    if(target == switch_cool)                                       /* 制冷开关触发 */
    {
        if(lv_obj_has_state(switch_cool, LV_STATE_CHECKED))         /* 判断开关状态 */
        {
            lv_obj_clear_state(switch_heat, LV_STATE_CHECKED);      /* 制冷模式已打开，关闭制暖模式 */
        }
    }
    else if(target == switch_heat)                                  /* 制暖开关触发 */
    {
        if(lv_obj_has_state(switch_heat, LV_STATE_CHECKED))         /* 判断开关状态 */
        {
            lv_obj_clear_state(switch_cool, LV_STATE_CHECKED);      /* 制暖模式已打开，关闭制冷模式 */
        }
    }

}

/**
 * @brief  功能文本标签
 * @param  无
 * @return 无
 */
static void lv_example_label(void)
{
    /* 根据活动屏幕宽度选择字体 */
    if (scr_act_width() <= 320)
    {
        font = &lv_font_montserrat_10;
    }
    else if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
    }
    else
    {
        font = &lv_font_montserrat_20;
    }
    lv_obj_t *label = lv_label_create(lv_scr_act());                            /* 创建标签 */
    lv_label_set_text(label, "Control  Center");                                /* 设置文本内容 */
    lv_obj_set_style_text_font(label, font, LV_STATE_DEFAULT);                  /* 设置字体 */
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -scr_act_height() / 3 );            /* 设置位置 */
}

/**
 * @brief  制冷功能开关
 * @param  无
 * @return 无
 */
static void lv_example_switch1(void)
{
    /* 制冷模式基础对象（矩形背景） */
    lv_obj_t *obj_cool = lv_obj_create(lv_scr_act());                               /* 创建基础对象 */
    lv_obj_set_size(obj_cool,scr_act_height() / 3, scr_act_height() / 3 );          /* 设置大小 */
    lv_obj_align(obj_cool, LV_ALIGN_CENTER, -scr_act_width() / 4, 0 );              /* 设置位置 */

    /* 制冷模式开关标签 */
    lv_obj_t *label_cool = lv_label_create(obj_cool);                               /* 创建标签 */
    lv_label_set_text(label_cool, "Cool");                                          /* 设置文本内容 */
    lv_obj_set_style_text_font(label_cool, font, LV_STATE_DEFAULT);                 /* 设置字体 */
    lv_obj_align(label_cool, LV_ALIGN_CENTER, 0, -scr_act_height() / 16 );          /* 设置位置 */

    /* 制冷模式开关 */
    switch_cool = lv_switch_create(obj_cool);                                       /* 创建开关 */
    lv_obj_set_size(switch_cool,scr_act_height() / 6, scr_act_height() / 12 );      /* 设置大小 */
    lv_obj_align(switch_cool, LV_ALIGN_CENTER, 0, scr_act_height() / 16 );          /* 设置位置 */
    lv_obj_add_event_cb(switch_cool, switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);/* 添加事件 */
}

/**
 * @brief  制暖功能开关
 * @param  无
 * @return 无
 */
static void lv_example_switch2(void)
{
    /* 制暖模式基础对象（矩形背景） */
    lv_obj_t *obj_heat = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj_heat,scr_act_height() / 3, scr_act_height() / 3 );
    lv_obj_align(obj_heat, LV_ALIGN_CENTER, 0, 0 );

    /* 制暖模式开关标签 */
    lv_obj_t *label_heat = lv_label_create(obj_heat);
    lv_label_set_text(label_heat, "Heat");
    lv_obj_set_style_text_font(label_heat, font, LV_STATE_DEFAULT);
    lv_obj_align(label_heat, LV_ALIGN_CENTER, 0, -scr_act_height() / 16 );

    /* 制暖模式开关 */
    switch_heat = lv_switch_create(obj_heat);
    lv_obj_set_size(switch_heat,scr_act_height() / 6, scr_act_height() / 12 );
    lv_obj_align(switch_heat, LV_ALIGN_CENTER, 0, scr_act_height() / 16 );
    lv_obj_add_event_cb(switch_heat, switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
}

/**
 * @brief  干燥功能开关
 * @param  无
 * @return 无
 */
static void lv_example_switch3(void)
{
    /* 干燥模式基础对象（矩形背景） */
    lv_obj_t *obj_dry = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj_dry,scr_act_height() / 3, scr_act_height() / 3 );
    lv_obj_align(obj_dry, LV_ALIGN_CENTER, scr_act_width() / 4, 0 );

    /* 干燥模式开关标签 */
    lv_obj_t *label_dry = lv_label_create(obj_dry);
    lv_label_set_text(label_dry, "Dry");
    lv_obj_set_style_text_font(label_dry, font, LV_STATE_DEFAULT);
    lv_obj_align(label_dry, LV_ALIGN_CENTER, 0, -scr_act_height() / 16 );

    /* 干燥模式开关 */
    switch_dry = lv_switch_create(obj_dry);
    lv_obj_set_size(switch_dry,scr_act_height() / 6, scr_act_height() / 12 );
    lv_obj_align(switch_dry, LV_ALIGN_CENTER, 0, scr_act_height() / 16 );
    lv_obj_add_state(switch_dry, LV_STATE_CHECKED|LV_STATE_DISABLED);
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_label();         /* 功能标题 */
    lv_example_switch1();       /* 制冷模式开关 */
    lv_example_switch2();       /* 制暖模式开关 */
    lv_example_switch3();       /* 干燥模式开关 */
}




