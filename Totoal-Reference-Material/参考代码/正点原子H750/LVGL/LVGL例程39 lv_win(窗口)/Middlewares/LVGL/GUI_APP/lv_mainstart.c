/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_win(窗口) 实验
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

static const lv_font_t *font;                                       /* 定义字体 */
static lv_obj_t *win;                                               /* 定义窗口 */

/**
 * @brief  按钮事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void btn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);                    /* 获取事件类型 */

    if(code == LV_EVENT_CLICKED)                                    /* 按钮按下 */
    {
        lv_obj_add_flag(win, LV_OBJ_FLAG_HIDDEN);                   /* 隐藏窗口 */
    }
}

/**
 * @brief  窗口实例
 * @param  无
 * @return 无
 */
static void lv_example_win(void)
{
    /* 根据屏幕宽度选择字体 */
    if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_12;
    }
    else
    {
        font = &lv_font_montserrat_20;
    }

    /* 窗口整体 */
    win = lv_win_create(lv_scr_act(), scr_act_height()/12);                                     /* 创建窗口 */
    lv_obj_set_size(win, scr_act_width()* 5/8, scr_act_height()* 4/7);                          /* 设置大小 */
    lv_obj_center(win);                                                                         /* 设置位置 */
    lv_obj_set_style_border_width(win, 1, LV_STATE_DEFAULT);                                    /* 设置边框宽度 */
    lv_obj_set_style_border_color(win, lv_color_hex(0x8a8a8a), LV_STATE_DEFAULT);               /* 设置边框颜色 */
    lv_obj_set_style_border_opa(win, 100, LV_STATE_DEFAULT);                                    /* 设置边框透明度 */
    lv_obj_set_style_radius(win, 10, LV_STATE_DEFAULT);                                         /* 设置圆角 */

    /*************************   第一部分：头部   ****************************/

    /* 左侧按钮 */
    lv_obj_t *btn_setting = lv_win_add_btn(win, LV_SYMBOL_SETTINGS,30);                         /* 添加按钮 */
    lv_obj_set_style_bg_opa(btn_setting, 0, LV_STATE_DEFAULT);                                  /* 设置背景透明度 */
    lv_obj_set_style_shadow_width(btn_setting, 0, LV_STATE_DEFAULT);                            /* 设置阴影宽度 */
    lv_obj_set_style_text_color(btn_setting, lv_color_hex(0x000000), LV_STATE_DEFAULT);         /* 设置文本颜色 */

    /* 标题 */
    lv_obj_t *title = lv_win_add_title(win, "Setting");                                         /* 添加标题 */
    lv_obj_set_style_text_font(title, font, LV_STATE_DEFAULT);                                  /* 设置字体 */

    /* 右侧按钮 */
    lv_obj_t *btn_close = lv_win_add_btn(win, LV_SYMBOL_CLOSE,30);                              /* 添加按钮 */
    lv_obj_set_style_bg_opa(btn_close, 0, LV_STATE_DEFAULT);                                    /* 设置背景透明度 */
    lv_obj_set_style_shadow_width(btn_close, 0, LV_STATE_DEFAULT);                              /* 设置阴影宽度 */
    lv_obj_set_style_text_color(btn_close, lv_color_hex(0x000000), LV_STATE_DEFAULT);           /* 设置文本颜色（未按下） */
    lv_obj_set_style_text_color(btn_close, lv_color_hex(0xff0000), LV_STATE_PRESSED);           /* 设置文本颜色（已按下） */
    lv_obj_add_event_cb(btn_close, btn_event_cb, LV_EVENT_CLICKED, NULL);                       /* 添加事件 */

    /*************************   第二部分：主体   ****************************/

    /* 主体背景 */
    lv_obj_t *content = lv_win_get_content(win);                                                /* 获取主体 */
    lv_obj_set_style_bg_color(content, lv_color_hex(0xffffff),LV_STATE_DEFAULT);                /* 设置背景颜色 */

    /* 音乐音量滑块 */
    lv_obj_t *slider_audio = lv_slider_create(content);                                         /* 创建滑块 */
    lv_obj_set_size(slider_audio, scr_act_width()/3, scr_act_height()/30);                      /* 设置大小 */
    lv_obj_align(slider_audio, LV_ALIGN_CENTER, 15, -scr_act_height()/14);                      /* 设置位置 */
    lv_slider_set_value(slider_audio, 50, LV_ANIM_OFF);                                         /* 设置当前值 */
    lv_obj_set_style_bg_color(slider_audio, lv_color_hex(0x787c78), LV_PART_MAIN);              /* 设置主体颜色 */
    lv_obj_set_style_bg_color(slider_audio, lv_color_hex(0xc3c3c3), LV_PART_INDICATOR);         /* 设置指示器颜色 */
    lv_obj_remove_style(slider_audio, NULL, LV_PART_KNOB);                                      /* 移除旋钮 */

    /* 音乐音量图标 */
    lv_obj_t *label_audio = lv_label_create(content);                                           /* 创建音量标签 */
    lv_label_set_text(label_audio, LV_SYMBOL_AUDIO);                                            /* 设置文本内容：音乐图标 */
    lv_obj_set_style_text_font(label_audio, font, LV_STATE_DEFAULT);                            /* 设置字体 */
    lv_obj_align_to(label_audio, slider_audio, LV_ALIGN_OUT_LEFT_MID, -scr_act_width()/40, 0);  /* 设置位置 */

    /* 闹钟音量滑块 */
    lv_obj_t *slider_bell = lv_slider_create(content);                                          /* 创建滑块 */
    lv_obj_set_size(slider_bell, scr_act_width()/3, scr_act_height()/30);                       /* 设置大小 */
    lv_obj_align(slider_bell, LV_ALIGN_CENTER, 15, scr_act_height()/14);                        /* 设置位置 */
    lv_slider_set_value(slider_bell, 50, LV_ANIM_OFF);                                          /* 设置当前值 */
    lv_obj_set_style_bg_color(slider_bell, lv_color_hex(0x787c78), LV_PART_MAIN);               /* 设置主体颜色 */
    lv_obj_set_style_bg_color(slider_bell, lv_color_hex(0xc3c3c3), LV_PART_INDICATOR);          /* 设置指示器颜色 */
    lv_obj_remove_style(slider_bell, NULL, LV_PART_KNOB);                                       /* 移除旋钮 */

    /* 闹钟音量图标 */
    lv_obj_t *label_bell = lv_label_create(content);                                            /* 创建音量标签 */
    lv_label_set_text(label_bell, LV_SYMBOL_BELL);                                              /* 设置文本内容：闹钟图标 */
    lv_obj_set_style_text_font(label_bell, font, LV_STATE_DEFAULT);                             /* 设置字体 */
    lv_obj_align_to(label_bell, slider_bell, LV_ALIGN_OUT_LEFT_MID, -scr_act_width()/40, 0);    /* 设置位置 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_win();
}


