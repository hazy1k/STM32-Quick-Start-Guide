/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_tabview(选项卡) 实验
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

static const lv_font_t *font;           /* 定义字体 */

/**
 * @brief  选项卡实例
 * @param  无
 * @return 无
 */
static void lv_example_tabview(void)
{
    /* 根据屏幕大小设置字体 */
    if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
    }
    else
    {
        font = &lv_font_montserrat_20;
    }

    /*************************   第一部分：选项卡（基础）   ****************************/

    lv_obj_t *tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, scr_act_height()/6);        /* 创建选项卡 */
    lv_obj_set_style_text_font(tabview, font, LV_STATE_DEFAULT);                                /* 设置字体 */

    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "Message");                                    /* 添加选项卡1 */
    lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "Schedule");                                   /* 添加选项卡2 */
    lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "Meeting");                                    /* 添加选项卡3 */

    lv_obj_t *label1 = lv_label_create(tab1);                                                   /* 创建标签（在选项卡1内） */
    lv_label_set_text(label1, "Tonight's meeting cancelled.");                                  /* 设置文本内容 */
    lv_obj_center(label1);                                                                      /* 设置位置 */

    lv_obj_t *label2 = lv_label_create(tab2);                                                   /* 创建标签（在选项卡2内） */
    lv_label_set_text(label2, "AM   8:30   meet the client\n\n" "PM   13:30   factory tour");   /* 设置文本内容 */
    lv_obj_center(label2);                                                                      /* 设置位置 */

    lv_obj_t *label3 = lv_label_create(tab3);                                                   /* 创建标签（在选项卡3内） */
    lv_label_set_text(label3, "None");                                                          /* 设置文本内容 */
    lv_obj_center(label3);                                                                      /* 设置位置 */

    /*************************   第二部分：选项卡（界面优化）   ************************/

    /* 1、按钮 */
    lv_obj_t *btn = lv_tabview_get_tab_btns(tabview);                                           /* 获取按钮部分 */

    /* 未选中的按钮 */
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xb7472a), LV_PART_ITEMS|LV_STATE_DEFAULT);     /* 设置按钮背景颜色：橙色 */
    lv_obj_set_style_bg_opa(btn, 200, LV_PART_ITEMS|LV_STATE_DEFAULT);                          /* 设置按钮背景透明度 */
    lv_obj_set_style_text_color(btn, lv_color_hex(0xf3f3f3), LV_PART_ITEMS|LV_STATE_DEFAULT);   /* 设置按钮文本颜色：白色 */

    /* 选中的按钮 */
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xe1e1e1), LV_PART_ITEMS|LV_STATE_CHECKED);     /* 设置按钮背景颜色：灰色 */
    lv_obj_set_style_bg_opa(btn, 200, LV_PART_ITEMS|LV_STATE_CHECKED);                          /* 设置按钮背景透明度 */
    lv_obj_set_style_text_color(btn, lv_color_hex(0xb7472a), LV_PART_ITEMS|LV_STATE_CHECKED);   /* 设置按钮文本颜色：橙色 */
    lv_obj_set_style_border_width(btn, 0, LV_PART_ITEMS| LV_STATE_CHECKED);                     /* 设置按钮边框宽度为0 */

    /* 2、主体 */
    lv_obj_t *obj = lv_tabview_get_content(tabview);                                            /* 获取主体部分 */
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_STATE_DEFAULT);                   /* 设置背景颜色：白色 */
    lv_obj_set_style_bg_opa(obj, 255, LV_STATE_DEFAULT);                                        /* 设置背景透明度 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_tabview();
}


