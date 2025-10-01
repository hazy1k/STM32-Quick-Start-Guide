/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_dropdown(下拉列表) 实验
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

static lv_obj_t *label;                                             /* 当前选项标签 */
static const lv_font_t *dropdown_font;                              /* 定义字体 */
static int16_t dropdown_width;                                      /* 列表宽度 */

/* 下拉列表的选项 */
static const char *options =    "option 1\n"
                                "option 2\n"
                                "option 3\n"
                                "option 4\n"
                                "option 5\n"
                                "option 6\n"
                                "option 7\n"
                                "option 8\n";

/**
 * @brief  下拉列表事件回调
 * @param  无
 * @return 无
 */
static void dropdown_event_cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);                    /* 获取事件类型 */
    lv_obj_t *dropdown = lv_event_get_target(e);                    /* 获取触发源 */

    if (LV_EVENT_VALUE_CHANGED == code)                             /* 判断事件类型 */
    {
        char buf[10];
        lv_dropdown_get_selected_str(dropdown, buf, sizeof(buf));   /* 获取当前选项文本 */
        lv_label_set_text(label, buf);                              /* 显示当前选项文本 */
    }
}

/**
 * @brief  例1
 * @param  无
 * @return 无
 */
static void lv_example_dropdown_1(void)
{
    /* 根据屏幕宽度选择字体和列表宽度 */
    if (scr_act_width() <= 320) 
    {
        dropdown_font = &lv_font_montserrat_14;
        dropdown_width = 90;
    }
    else if (scr_act_width() <= 480) 
    {
        dropdown_font = &lv_font_montserrat_18;
        dropdown_width = 120;
    }
    else 
    {
        dropdown_font = &lv_font_montserrat_22;
        dropdown_width = 150;
    }

    lv_obj_t* dropdown = lv_dropdown_create(lv_scr_act());                                      /* 定义并创建下拉列表 */
    lv_dropdown_set_options_static(dropdown, options);                                          /* 添加下拉列表选项 */
    lv_obj_set_style_text_font(dropdown, dropdown_font, LV_PART_MAIN);                          /* 设置下拉列表字体 */
    lv_obj_set_width(dropdown, dropdown_width);                                                 /* 设置下拉列表宽度 */
    lv_obj_align(dropdown, LV_ALIGN_CENTER, -scr_act_width() / 3, 0);                           /* 设置下拉列表位置 */

    label = lv_label_create(lv_scr_act());                                                      /* 定义并创建标签 */
    lv_obj_set_style_text_font(label, dropdown_font, LV_PART_MAIN);                             /* 设置标签表字体 */
    lv_obj_set_width(label, dropdown_width);                                                    /* 设置标签宽度 */
    lv_obj_align_to(label, dropdown, LV_ALIGN_OUT_TOP_MID, 15, -scr_act_height() / 8);          /* 设置标签位置 */
    lv_label_set_text(label, "option 1");                                                       /* 设置标签文本 */

    lv_obj_add_event_cb(dropdown, dropdown_event_cb, LV_EVENT_VALUE_CHANGED, NULL);             /* 添加下拉列表回调 */
}

/**
 * @brief  例2
 * @param  无
 * @return 无
 */
static void lv_example_dropdown_2(void)
{
    lv_obj_t* dropdown;                                                                         /* 定义下拉列表 */
    
    dropdown = lv_dropdown_create(lv_scr_act());                                                /* 创建下拉列表 */
    lv_dropdown_set_options_static(dropdown, options);                                          /* 添加下拉列表选项 */
    lv_obj_set_style_text_font(dropdown, dropdown_font, LV_PART_MAIN);                          /* 设置下拉列表字体 */
    lv_obj_set_width(dropdown, dropdown_width);                                                 /* 设置下拉列表宽度 */
    lv_dropdown_set_dir(dropdown, LV_DIR_BOTTOM);                                               /* 设置下拉列表方向 */
    lv_dropdown_set_symbol(dropdown, LV_SYMBOL_DOWN);                                           /* 设置下拉列表符号 */
    lv_obj_align(dropdown, LV_ALIGN_CENTER, scr_act_width() / 8, -3 * scr_act_height() / 8);    /* 设置下拉列表位置 */

    dropdown = lv_dropdown_create(lv_scr_act());                                                /* 创建下拉列表 */
    lv_dropdown_set_options_static(dropdown, options);                                          /* 添加下拉列表选项 */
    lv_obj_set_style_text_font(dropdown, dropdown_font, LV_PART_MAIN);                          /* 设置下拉列表字体 */
    lv_obj_set_width(dropdown, dropdown_width);                                                 /* 设置下拉列表宽度 */
    lv_dropdown_set_dir(dropdown, LV_DIR_LEFT);                                                 /* 设置下拉列表方向 */
    lv_dropdown_set_symbol(dropdown, LV_SYMBOL_LEFT);                                           /* 设置下拉列表符号 */
    lv_obj_align(dropdown, LV_ALIGN_CENTER, scr_act_width() / 8, -1 * scr_act_height() / 8);    /* 设置下拉列表位置 */

    dropdown = lv_dropdown_create(lv_scr_act());                                                /* 创建下拉列表 */
    lv_dropdown_set_options_static(dropdown, options);                                          /* 添加下拉列表选项 */
    lv_obj_set_style_text_font(dropdown, dropdown_font, LV_PART_MAIN);                          /* 设置下拉列表字体 */
    lv_obj_set_width(dropdown, dropdown_width);                                                 /* 设置下拉列表宽度 */
    lv_dropdown_set_dir(dropdown, LV_DIR_RIGHT);                                                /* 设置下拉列表方向 */
    lv_dropdown_set_symbol(dropdown, LV_SYMBOL_RIGHT);                                          /* 设置下拉列表符号 */
    lv_obj_align(dropdown, LV_ALIGN_CENTER, scr_act_width() / 8, 1 * scr_act_height() / 8);     /* 设置下拉列表位置 */

    dropdown = lv_dropdown_create(lv_scr_act());                                                /* 创建下拉列表 */
    lv_dropdown_set_options_static(dropdown, options);                                          /* 添加下拉列表选项 */
    lv_obj_set_style_text_font(dropdown, dropdown_font, LV_PART_MAIN);                          /* 设置下拉列表字体 */
    lv_obj_set_width(dropdown, dropdown_width);                                                 /* 设置下拉列表宽度 */
    lv_dropdown_set_dir(dropdown, LV_DIR_TOP);                                                  /* 设置下拉列表方向 */
    lv_dropdown_set_symbol(dropdown, LV_SYMBOL_UP);                                             /* 设置下拉列表符号 */
    lv_obj_align(dropdown, LV_ALIGN_CENTER, scr_act_width() / 8, 3 * scr_act_height() / 8);     /* 设置下拉列表位置 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_dropdown_1();
    lv_example_dropdown_2();
}


