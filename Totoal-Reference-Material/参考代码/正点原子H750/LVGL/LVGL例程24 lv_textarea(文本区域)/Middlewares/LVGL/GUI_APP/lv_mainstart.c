/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_textarea(文本区域) 实验
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

static const lv_font_t *font;       /* 定义字体 */

static lv_obj_t *keyboard;          /* 键盘 */
static lv_obj_t *label_name;        /* 用户名正误提示标签 */
static lv_obj_t *label_pass;        /* 密码正误提示标签 */

/**
 * @brief  用户名文本框事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void name_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);            /* 获取事件类型 */
    lv_obj_t *target = lv_event_get_target(e);              /* 获取触发源 */

    if(code == LV_EVENT_FOCUSED)                            /* 事件类型：被聚焦 */
    {
        lv_keyboard_set_textarea(keyboard, target);         /* 关联用户名文本框和键盘 */
    }
    else if(code == LV_EVENT_VALUE_CHANGED)                 /* 事件类型：文本框的内容发生变化 */
    {
        const char *txt = lv_textarea_get_text(target);     /* 获取文本框的文本 */

        if(strcmp(txt,"admin") == 0)                        /* 判断用户名是否正确 */
        {
            lv_label_set_text(label_name, LV_SYMBOL_OK);    /* 用户名正确，显示√ */
        }
        else
        {
            lv_label_set_text(label_name, "");              /* 用户名错误，不提示 */
        }
    }
}

/**
 * @brief  密码文本框事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void pass_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);            /* 获取事件类型 */
    lv_obj_t *target = lv_event_get_target(e);              /* 获取触发源 */

    if(code == LV_EVENT_FOCUSED)                            /* 事件类型：被聚焦 */
    {
        lv_keyboard_set_textarea(keyboard, target);         /* 关联用户名文本框和键盘 */
    }
    else if(code == LV_EVENT_VALUE_CHANGED)                 /* 事件类型：文本框的内容发生变化 */
    {
        const char *txt = lv_textarea_get_text(target);     /* 获取文本框的文本 */

        if(strcmp(txt,"123456") == 0)                       /* 判断密码是否正确 */
        {
            lv_label_set_text(label_pass, LV_SYMBOL_OK);    /* 密码正确，显示√ */
        }
        else
        {
            lv_label_set_text(label_pass, "");              /* 密码错误，不提示 */
        }
    }
}

/**
 * @brief  用户登录实例
 * @param  无
 * @return 无
 */
static void lv_example_textarea(void)
{

    /* 根据屏幕大小设置字体 */
    if (scr_act_width() <= 320)
    {
        font = &lv_font_montserrat_12;
    }
    else if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
    }
    else
    {
        font = &lv_font_montserrat_22;
    }

    /* 用户名文本框 */
    lv_obj_t *textarea_name = lv_textarea_create(lv_scr_act());                                     /* 创建文本框 */
    lv_obj_set_width(textarea_name, scr_act_width()/2);                                             /* 设置宽度 */
    lv_obj_set_style_text_font(textarea_name, font, LV_PART_MAIN);                                  /* 设置字体 */
    lv_obj_align(textarea_name, LV_ALIGN_CENTER, 0, -scr_act_height()/3 );                          /* 设置位置 */
    lv_textarea_set_one_line(textarea_name, true);                                                  /* 设置单行模式 */
    lv_textarea_set_max_length(textarea_name, 6);                                                   /* 设置输入字符的最大长度 */
    lv_textarea_set_placeholder_text(textarea_name, "user name");                                   /* 设置占位符 */
    lv_obj_add_event_cb(textarea_name, name_event_cb, LV_EVENT_ALL, NULL);                          /* 添加文本框事件回调 */

    /* 用户名正误提示标签 */
    label_name = lv_label_create(lv_scr_act());                                                     /* 创建标签 */
    lv_label_set_text(label_name, "");                                                              /* 默认不提示 */
    lv_obj_set_style_text_font(label_name, font, LV_PART_MAIN);                                     /* 设置字体 */
    lv_obj_align_to(label_name, textarea_name, LV_ALIGN_OUT_RIGHT_MID, 5, 0);                       /* 设置位置 */

    /* 密码文本框 */
    lv_obj_t *textarea_pass = lv_textarea_create(lv_scr_act());                                     /* 创建文本框 */
    lv_obj_set_width(textarea_pass, scr_act_width()/2);                                             /* 设置宽度 */
    lv_obj_set_style_text_font(textarea_pass, font, LV_PART_MAIN);                                  /* 设置字体 */
    lv_obj_align_to(textarea_pass, textarea_name, LV_ALIGN_OUT_BOTTOM_MID, 0, scr_act_height()/20); /* 设置位置 */
    lv_textarea_set_one_line(textarea_pass, true);                                                  /* 设置单行模式 */
    lv_textarea_set_password_mode(textarea_pass, true);                                             /* 设置密码模式 */
    lv_textarea_set_password_show_time(textarea_pass, 1000);                                        /* 设置密码显示时间 */
    lv_textarea_set_max_length(textarea_pass, 8);                                                   /* 设置输入字符的最大长度 */
    lv_textarea_set_placeholder_text(textarea_pass, "password");                                    /* 设置占位符 */
    lv_obj_add_event_cb(textarea_pass, pass_event_cb, LV_EVENT_ALL, NULL);                          /* 添加文本框事件回调 */

    /* 密码正误提示标签 */
    label_pass = lv_label_create(lv_scr_act());                                                     /* 创建标签 */
    lv_label_set_text(label_pass, "");                                                              /* 默认不提示 */
    lv_obj_set_style_text_font(label_pass, font, LV_PART_MAIN);                                     /* 设置字体 */
    lv_obj_align_to(label_pass, textarea_pass, LV_ALIGN_OUT_RIGHT_MID, 5, 0);                       /* 设置位置 */

    /* 键盘 */
    keyboard = lv_keyboard_create(lv_scr_act());                                                    /* 创建键盘 */
    lv_obj_set_size(keyboard, scr_act_width(), scr_act_height()/2);                                 /* 设置大小 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_textarea();
}



