/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_msgbox(消息框) 实验
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

static lv_obj_t *msgbox;                                                            /* 消息框 */
static lv_obj_t *slider_label;                                                      /* 音量百分比标签 */


/*************************  第一部分 音量调节滑块  ******************************/

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
        lv_label_set_text_fmt(slider_label, "%d%%", lv_slider_get_value(target));   /* 获取当前值，更新音量百分比 */

        if(lv_slider_get_value(target) > 80)                                        /* 音量大于80% */
        {
            lv_obj_clear_flag(msgbox, LV_OBJ_FLAG_HIDDEN);                          /* 清除消息框隐藏属性，出现弹窗提示 */
        }
    }
}

/**
 * @brief  音量调节滑块
 * @param  无
 * @return 无
 */
static void lv_example_slider(void)
{
    /* 滑块 */
    lv_obj_t * slider = lv_slider_create(lv_scr_act());                                 /* 创建滑块 */
    lv_obj_set_size(slider, scr_act_width() / 2, 20);                                   /* 设置大小 */
    lv_obj_center(slider);                                                              /* 设置位置 */
    lv_slider_set_value(slider, 50, LV_ANIM_OFF);                                       /* 设置当前值 */
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);         /* 添加事件 */

    /* 百分比标签 */
    slider_label = lv_label_create(lv_scr_act());                                       /* 创建百分比标签 */
    lv_label_set_text(slider_label, "50%");                                             /* 设置文本内容 */
    lv_obj_set_style_text_font(slider_label, &lv_font_montserrat_20, LV_STATE_DEFAULT); /* 设置字体 */
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_RIGHT_MID, 20, 0);               /* 设置位置 */

    /* 音量图标 */
    lv_obj_t *sound_label = lv_label_create(lv_scr_act());                              /* 创建音量标签 */
    lv_label_set_text(sound_label, LV_SYMBOL_VOLUME_MAX);                               /* 设置文本内容：音量图标 */
    lv_obj_set_style_text_font(sound_label, &lv_font_montserrat_20, LV_STATE_DEFAULT);  /* 设置字体 */
    lv_obj_align_to(sound_label, slider, LV_ALIGN_OUT_LEFT_MID, -20, 0);                /* 设置位置 */
}


/*************************  第二部分 消息框  ******************************/

/**
 * @brief  消息框事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void msgbox_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_current_target(e);                 /* 获取当前触发源 */

    if(lv_msgbox_get_active_btn(target) == 2)                          /* 获取按钮索引 */
    {
        lv_obj_add_flag(msgbox, LV_OBJ_FLAG_HIDDEN);                   /* 隐藏消息框 */
    }
}

/**
 * @brief  消息框实例
 * @param  无
 * @return 无
 */
static void lv_example_msgbox(void)
{
    static const char *btns[] = { " ", " ", "OK", "" };

    /* 消息框整体 */
    msgbox = lv_msgbox_create(lv_scr_act(),LV_SYMBOL_WARNING " Notice","Excessive volume may damage hearing.", btns,false);
    lv_obj_set_size(msgbox, 300, 170);                                                       /* 设置大小 */
    lv_obj_center(msgbox);                                                                   /* 设置位置 */
    lv_obj_set_style_border_width(msgbox, 0, LV_STATE_DEFAULT);                              /* 去除边框 */
    lv_obj_set_style_shadow_width(msgbox, 20, LV_STATE_DEFAULT);                             /* 设置阴影宽度 */
    lv_obj_set_style_shadow_color(msgbox, lv_color_hex(0xa9a9a9), LV_STATE_DEFAULT);         /* 设置阴影颜色 */
    lv_obj_set_style_pad_top(msgbox,18,LV_STATE_DEFAULT);                                    /* 设置顶部填充 */
    lv_obj_set_style_pad_left(msgbox,20,LV_STATE_DEFAULT);                                   /* 设置左侧填充 */
    lv_obj_add_event_cb(msgbox, msgbox_event_cb, LV_EVENT_VALUE_CHANGED, NULL);              /* 添加事件 */

    /* 消息框标题 */
    lv_obj_t *title = lv_msgbox_get_title(msgbox);                                           /* 获取标题部分 */
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, LV_STATE_DEFAULT);             /* 设置字体 */
    lv_obj_set_style_text_color(title, lv_color_hex(0xff0000),LV_STATE_DEFAULT);             /* 设置文本颜色：红色 */

    /* 消息框主体 */
    lv_obj_t *content = lv_msgbox_get_content(msgbox);                                       /* 获取主体部分 */
    lv_obj_set_style_text_font(content, &lv_font_montserrat_20, LV_STATE_DEFAULT);           /* 设置字体 */
    lv_obj_set_style_text_color(content, lv_color_hex(0x6c6c6c), LV_STATE_DEFAULT);          /* 设置文本颜色：灰色 */
    lv_obj_set_style_pad_top(content,15,LV_STATE_DEFAULT);                                   /* 设置顶部填充 */

    /* 消息框按钮 */
    lv_obj_t *btn = lv_msgbox_get_btns(msgbox);                                              /* 获取按钮矩阵部分 */
    lv_obj_set_style_bg_opa(btn, 0, LV_PART_ITEMS);                                          /* 设置按钮背景透明度 */
    lv_obj_set_style_shadow_width(btn, 0, LV_PART_ITEMS);                                    /* 去除按钮阴影 */
    lv_obj_set_style_text_font(btn, &lv_font_montserrat_20, LV_PART_ITEMS);                  /* 设置字体 */
    lv_obj_set_style_text_color(btn, lv_color_hex(0x2271df),LV_PART_ITEMS);                  /* 设置文本颜色（未按下）：蓝色 */
    lv_obj_set_style_text_color(btn, lv_color_hex(0xff0000),LV_PART_ITEMS|LV_STATE_PRESSED); /* 设置文本颜色（已按下）：红色 */

    lv_obj_add_flag(msgbox, LV_OBJ_FLAG_HIDDEN);                                             /* 隐藏消息框 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_slider();
    lv_example_msgbox();
}

