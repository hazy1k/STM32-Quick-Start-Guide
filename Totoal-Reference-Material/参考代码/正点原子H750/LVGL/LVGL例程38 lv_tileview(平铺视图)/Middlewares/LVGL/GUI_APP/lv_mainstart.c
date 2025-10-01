/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_tileview(平铺视图) 实验
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
 * @brief  平铺视图实例
 * @param  无
 * @return 无
 */
static void lv_example_tileview(void)
{
    /* 根据屏幕宽度设置字体 */
    if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
    }
    else
    {
        font = &lv_font_montserrat_20;
    }

    /* 创建平铺视图页面 */
    lv_obj_t *tileview = lv_tileview_create(lv_scr_act());                                  /* 创建平铺视图 */

    lv_obj_t *tile_1 = lv_tileview_add_tile( tileview, 0, 0, LV_DIR_RIGHT );                /* 添加页面1 */
    lv_obj_t *tile_2 = lv_tileview_add_tile( tileview, 1, 0, LV_DIR_LEFT|LV_DIR_RIGHT );    /* 添加页面2 */
    lv_obj_t *tile_3 = lv_tileview_add_tile( tileview, 2, 0, LV_DIR_LEFT );                 /* 添加页面3 */

    /* 设置页面内容 */
    lv_obj_t *label_1 = lv_label_create(tile_1);                                            /* 创建标签 */
    lv_label_set_text(label_1, "Page_1");                                                   /* 设置文本内容 */
    lv_obj_set_style_text_font(label_1, font, LV_STATE_DEFAULT);                            /* 设置字体 */
    lv_obj_center(label_1);                                                                 /* 设置位置 */

    lv_obj_t *label_2 = lv_label_create(tile_2);                                            /* 创建标签 */
    lv_label_set_text(label_2, "Page_2");                                                   /* 设置文本内容 */
    lv_obj_set_style_text_font(label_2, font, LV_STATE_DEFAULT);                            /* 设置字体 */
    lv_obj_center(label_2);                                                                 /* 设置位置 */

    lv_obj_t *label_3 = lv_label_create(tile_3);                                            /* 创建标签 */
    lv_label_set_text(label_3, "Page_3");                                                   /* 设置文本内容 */
    lv_obj_set_style_text_font(label_3, font, LV_STATE_DEFAULT);                            /* 设置字体 */
    lv_obj_center(label_3);                                                                 /* 设置位置 */
    lv_obj_remove_style(tileview, NULL, LV_PART_SCROLLBAR);                                 /* 移除滚动条 */

    /* 左侧状态栏 */
    lv_obj_t *label_left = lv_label_create(lv_scr_act());                                   /* 创建标签 */
    lv_label_set_text(label_left, "AM 8:30" );                                              /* 设置文本内容 */
    lv_obj_set_style_text_font(label_left, font, LV_STATE_DEFAULT);                         /* 设置字体 */
    lv_obj_align(label_left, LV_ALIGN_TOP_LEFT, 10, 10);                                    /* 设置位置 */

    /* 右侧状态栏 */
    lv_obj_t *label_right = lv_label_create(lv_scr_act());                                  /* 创建标签 */
    lv_label_set_text(label_right, LV_SYMBOL_WIFI "   80% " LV_SYMBOL_BATTERY_3);           /* 设置文本内容 */
    lv_obj_set_style_text_font(label_right, font, LV_STATE_DEFAULT);                        /* 设置字体 */
    lv_obj_align(label_right, LV_ALIGN_TOP_RIGHT, -10, 10);                                 /* 设置位置 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_tileview();
}


