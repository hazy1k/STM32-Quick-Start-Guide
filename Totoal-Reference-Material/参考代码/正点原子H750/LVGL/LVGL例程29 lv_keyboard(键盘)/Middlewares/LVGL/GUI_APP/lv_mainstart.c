/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_keyboard(键盘) 实验
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
#define scr_act_width()  lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

/**
 * @brief  键盘事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void keyboard_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);                            /* 获取事件类型 */
    lv_obj_t *target = lv_event_get_target(e);                              /* 获取触发源 */

    if(code == LV_EVENT_VALUE_CHANGED)
    {
        uint16_t id = lv_btnmatrix_get_selected_btn(target);                /* 获取键盘按钮索引 */

        const char *txt = lv_btnmatrix_get_btn_text(target, id);            /* 获取按钮文本 */

        if(strcmp(txt, LV_SYMBOL_KEYBOARD) == 0)                            /* 判断是不是键盘图标被按下 */
        {
            if(lv_keyboard_get_mode(target) == LV_KEYBOARD_MODE_NUMBER)     /* 获取当前键盘模式，判断是否为数字模式 */
            {
                lv_keyboard_set_mode(target, LV_KEYBOARD_MODE_TEXT_LOWER);  /* 如果是数字模式，则切换为小写字母模式 */
            }
            else
            {
                lv_keyboard_set_mode(target, LV_KEYBOARD_MODE_NUMBER);      /* 不是数字模式，则切换为数字模式 */
            }
        }
    }
}

/**
 * @brief  键盘实例
 * @param  无
 * @return 无
 */
static void lv_example_keyboard(void)
{
    /* 文本框 */
    lv_obj_t *textarea = lv_textarea_create(lv_scr_act());                              /* 创建文本框 */
    lv_obj_set_size(textarea, scr_act_width() - 10, scr_act_height() / 2 - 10);         /* 设置大小 */
    lv_obj_align(textarea, LV_ALIGN_TOP_MID, 0, 0);                                     /* 设置位置 */
    
    /* 键盘 */
    lv_obj_t *keyboard = lv_keyboard_create(lv_scr_act());                              /* 创建键盘 */
    lv_keyboard_set_textarea(keyboard, textarea);                                       /* 关联键盘和文本框 */
    lv_obj_add_event_cb(keyboard, keyboard_event_cb, LV_EVENT_VALUE_CHANGED, NULL);     /* 设置键盘事件回调 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_keyboard();
}
