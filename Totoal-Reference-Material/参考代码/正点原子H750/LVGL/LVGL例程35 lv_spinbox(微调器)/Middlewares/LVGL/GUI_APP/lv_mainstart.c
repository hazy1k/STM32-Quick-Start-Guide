/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_spinbox（微调器） 实验
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


static lv_obj_t *spinbox;                          /* 微调器 */
static lv_obj_t *btn_up;                           /* 递增按钮 */
static lv_obj_t *btn_down;                         /* 递减按钮 */

/**
 * @brief  按钮事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void btn_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);     /* 获取触发源 */

    if (target == btn_up)                          /* 递增按钮按下 */
    {
        lv_spinbox_increment(spinbox);             /* 数值递增 */
    }
    else if (target == btn_down)                   /* 递减按钮按下 */
    {
        lv_spinbox_decrement(spinbox);             /* 数值递减 */
    }
}

/**
 * @brief  微调器事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void spinbox_event_cb(lv_event_t *e)
{
    float spinbox_value;                           /* 微调器当前值 */

    lv_event_code_t code = lv_event_get_code(e);   /* 获取事件类型 */
    
    if(code == LV_EVENT_VALUE_CHANGED)
    {
        spinbox_value= (float)lv_spinbox_get_value(spinbox);

        printf("%.1f\r\n", spinbox_value/10);
    }
}

/**
 * @brief  微调器实例
 * @param  无
 * @return 无
 */
static void lv_example_spinbox(void)
{
    /* 微调器 */
    spinbox = lv_spinbox_create(lv_scr_act());                                          /* 创建微调器 */
    lv_spinbox_set_range(spinbox, -10000, 10000);                                       /* 设置范围值 */
    lv_spinbox_set_digit_format(spinbox, 5, 4);                                         /* 设置数字格式 */
    lv_obj_set_size(spinbox, 150, 47);                                                  /* 设置大小 */
    lv_obj_center(spinbox);                                                             /* 设置位置 */
    lv_obj_update_layout(spinbox);                                                      /* 更新布局 */
    lv_obj_set_style_text_font(spinbox, &lv_font_montserrat_18, LV_PART_MAIN);          /* 设置字体 */
    lv_obj_set_style_text_align(spinbox, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);           /* 设置文本对齐方式 */
    lv_obj_add_event_cb(spinbox, spinbox_event_cb, LV_EVENT_VALUE_CHANGED, NULL);       /* 添加事件回调 */

    /* 递增按钮 */
    btn_up = lv_btn_create(lv_scr_act());                                               /* 创建按钮 */
    lv_obj_set_size(btn_up, 38, 38);                                                    /* 设置大小 */
    lv_obj_align_to(btn_up, spinbox, LV_ALIGN_OUT_LEFT_MID, -10, 0);                    /* 设置位置 */
    lv_obj_set_style_bg_img_src(btn_up, LV_SYMBOL_PLUS, LV_PART_MAIN);                  /* 设置背景图标 */
    lv_obj_add_event_cb(btn_up, btn_event_cb, LV_EVENT_CLICKED, NULL);                  /* 添加事件回调 */

    /* 递减按钮 */
    btn_down = lv_btn_create(lv_scr_act());                                             /* 创建按钮 */
    lv_obj_set_size(btn_down, 38, 38);                                                  /* 设置大小 */
    lv_obj_align_to(btn_down, spinbox, LV_ALIGN_OUT_RIGHT_MID, 10, 0);                  /* 设置位置 */
    lv_obj_set_style_bg_img_src(btn_down, LV_SYMBOL_MINUS, LV_PART_MAIN);               /* 设置背景图标 */
    lv_obj_add_event_cb(btn_down, btn_event_cb, LV_EVENT_CLICKED, NULL);                /* 添加事件回调 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_spinbox();
}


