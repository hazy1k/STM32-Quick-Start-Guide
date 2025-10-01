/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_chart(图表) 实验
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



LV_FONT_DECLARE(myFont14) \
LV_FONT_DECLARE(myFont24)

#if (1 != LV_FONT_MONTSERRAT_18)
#error Please Make Sure Enable LV_FONT_MONTSERRAT_18
#endif

/* 获取当前活动屏幕的宽高 */
#define scr_act_width() lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

/* 示例数据 */
static const lv_coord_t example_data[] = {
    25, 26, 27, 29, 30, 31, 32, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 
    45, 46, 47, 47, 48, 48, 49, 49, 49, 50, 50, 50, 50, 50, 50, 50, 50, 49, 49, 
    49, 48, 48, 47, 47, 46, 45, 44, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 33, 
    32, 31, 30, 29, 27, 26, 25, 24, 22, 21, 20, 19, 17, 16, 15, 14, 13, 12, 11, 
    10, 9, 8, 7, 6, 5, 5, 4, 3, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 
    1, 2, 2, 3, 4, 4, 5, 6, 6, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17, 18, 19, 20, 
    22, 23, 24, 25, 27, 28, 29, 30, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 
    43, 44, 45, 46, 46, 47, 47, 48, 48, 49, 49, 49, 50, 50, 50, 50, 50, 50, 50, 
    50, 49, 49, 49, 48, 48, 47, 46, 46, 45, 44, 43, 42, 42, 41, 40, 39, 38, 36, 
    35, 34, 33, 32, 31, 29, 28, 27, 26, 24, 23, 22, 21, 19, 18, 17, 16, 15, 14, 
    12, 11, 10, 9, 8, 8, 7, 6, 5, 4, 4, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 
    0, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 
    18, 20, 21, 22, 23, 25, 26, 27, 28, 30, 31, 32, 33, 34, 36, 37, 38, 39, 40, 
    41, 42, 43, 44, 44, 45, 46, 46, 47, 48, 48, 49, 49, 49, 50, 50, 50, 50, 50, 
    50, 50, 50, 49, 49, 49, 48, 48, 47, 47, 46, 45, 45, 44, 43, 42, 41, 40, 39, 
    38, 37, 36, 35, 34, 33, 31, 30, 29, 28, 26, 25, 24, 23, 21, 20, 19, 18, 17, 
    15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 6, 5, 4, 3, 3, 2, 2, 1, 1, 1, 0, 0, 0, 
    0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 13, 
    14, 15, 16, 18, 19, 20, 21, 23, 24, 25, 26, 28, 29, 30, 31, 32, 34, 35, 36, 
    37, 38, 39, 40, 41, 42, 43, 44, 45, 45, 46, 47, 47, 48, 48, 49, 49, 49, 50, 
    50, 50, 50, 50, 50, 50, 50, 49, 49, 49, 48, 48, 47, 47, 46, 45, 44, 44, 43, 
    42, 41, 40, 39, 38, 37, 36, 35, 33, 32, 31, 30, 29, 27, 26, 25, 24, 22, 21, 
    20, 19, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 5, 4, 3, 3, 2, 2, 1, 
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 
    11, 12, 13, 15, 16, 17, 18, 19, 20, 22, 23, 24, 25, 27, 28, 29, 30, 32, 33, 
    34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 46, 47, 47, 48, 48, 49, 
    49, 49, 50, 50, 50, 50, 50, 50, 50, 50, 49, 49, 49, 48, 48, 47, 46, 46, 45, 
    44, 43, 42, 42, 41, 40, 39, 38, 36, 35, 34, 33, 32, 31, 29, 28, 27, 26, 24, 
    23, 22, 21, 19, 18, 17, 16, 15, 14, 12, 11, 10, 9, 8, 7, 7, 6, 5, 4, 4, 3, 
    2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 
    8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 20, 21, 22, 23, 25, 26, 27, 28, 
    30, 31, 32, 33, 34, 36, 37, 38, 39, 40, 41, 42, 43, 44, 44, 45, 46, 46, 47, 
    48, 48, 49, 49, 49, 50, 50, 50, 50, 50, 50, 50, 50, 49, 49, 49, 48, 48, 47, 
    47, 46, 45, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 31, 30, 29, 
    28, 26, 25, 24, 23, 21, 20, 19, 18, 17, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 
    6, 5, 4, 3, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 
    4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 21, 23, 24, 25, 
    26, 28, 29, 30, 31, 32, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 45, 
    46, 47, 47, 48, 48, 49, 49, 49, 50, 50, 50, 50, 50, 50, 50, 50, 49, 49, 49, 
    48, 48, 47, 47, 46, 45, 44, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 33, 32, 
    31, 30, 29, 27, 26, 25, 24, 22, 21, 20, 19, 17, 16, 15, 14, 13, 12, 11, 10, 
    9, 8, 7, 6, 5, 5, 4, 3, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 
    2, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17, 18, 19, 20, 
    22, 23, 24, 25, 27, 28, 29, 30, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 
    43, 44, 45, 46, 46, 47, 47, 48, 48, 49, 49, 49, 50, 50, 50, 50, 50, 50, 50, 
    50, 49, 49, 49, 48, 48, 47, 46, 46, 45, 44, 43, 42, 42, 41, 40, 39, 38, 36, 
    35, 34, 33, 32, 31, 29, 28, 27, 26, 24, 23, 22, 21, 19, 18, 17, 16, 15, 14, 
    12, 11, 10, 9, 8, 7, 7, 6, 5, 4, 4, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 
    0, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 
};

static lv_obj_t* label;

/**
 * @brief  图表回调
 * @param  无
 * @return 无
 */
static void event_chart_cb(lv_event_t* e)
{
    lv_obj_t* chart = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_chart_cursor_t* cursor = (lv_chart_cursor_t*)e->user_data;

    if (LV_EVENT_VALUE_CHANGED == code) {
        uint16_t point_id = lv_chart_get_pressed_point(chart);
        if (LV_CHART_POINT_NONE != point_id) {
            lv_chart_set_cursor_point(chart, cursor, NULL, point_id);
            char buf[20];
            lv_snprintf(buf, sizeof(buf), "(%d, %d)", point_id, example_data[point_id]);
            lv_label_set_text(label, buf);
        }
    }
}

/**
 * @brief  X轴滑杆回调
 * @param  无
 * @return 无
 */
static void event_slider_x_cb(lv_event_t* e)
{
    lv_obj_t* slider_x = lv_event_get_target(e);
    lv_obj_t* chart = (lv_obj_t*)e->user_data;
    lv_chart_set_zoom_x(chart, lv_slider_get_value(slider_x));
}

/**
 * @brief  Y轴滑杆回调
 * @param  无
 * @return 无
 */
static void event_slider_y_cb(lv_event_t* e)
{
    lv_obj_t* slider_y = lv_event_get_target(e);
    lv_obj_t* chart = (lv_obj_t*)e->user_data;
    lv_chart_set_zoom_y(chart, lv_slider_get_value(slider_y));
}

/**
 * @brief  例
 * @param  无
 * @return 无
 */
static void lv_example_chart(void)
{
    lv_obj_t* chart = lv_chart_create(lv_scr_act());                                                                        /* 定义并初始化图表 */
    lv_obj_set_size(chart, scr_act_width(), scr_act_height());                                                              /* 设置图表大小 */
    lv_obj_align(chart, LV_ALIGN_TOP_LEFT, 0, 0);                                                                           /* 设置图表位置 */
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, -5, 55);                                                             /* 设置左轴Y方向的最大最小值 */
    lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);                                                                     /* 不显示数据上的点 */
    lv_chart_series_t* ser = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);          /* 在图表中分配并添加数据系列 */
    uint32_t pcnt = sizeof(example_data) / sizeof(example_data[0]);                                                         /* 计算数据线上的新点数 */
    lv_chart_set_point_count(chart, pcnt);                                                                                  /* 在图表上设置数据线上的点数 */
    lv_chart_set_ext_y_array(chart, ser, (lv_coord_t*)example_data);                                                        /* 为Y轴数据点设置一个外部阵列 */
    lv_chart_cursor_t* cursor = lv_chart_add_cursor(chart, lv_palette_main(LV_PALETTE_BLUE), LV_DIR_LEFT | LV_DIR_BOTTOM);  /* 添加光标 */
    lv_obj_update_layout(chart);                                                                                            /* 手动更新图表参数 */

    label = lv_label_create(lv_scr_act());                                                                                  /* 定义并创建显示值的标签 */
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 5, 5);                                                                           /* 设置标签的位置 */
    lv_obj_set_style_text_font(label, &lv_font_montserrat_18, LV_PART_MAIN);                                                /* 设置标签的字体 */
    lv_label_set_text(label, "");                                                                                           /* 设置标签的文本 */

    lv_obj_t* slider_x = lv_slider_create(lv_scr_act());                                                                    /* 定义并创建X轴缩放滑杆 */
    lv_slider_set_range(slider_x, LV_IMG_ZOOM_NONE, LV_IMG_ZOOM_NONE * 10);                                                 /* 设置滑杆范围 */
    lv_obj_add_event_cb(slider_x, event_slider_x_cb, LV_EVENT_VALUE_CHANGED, chart);                                        /* 设置滑杆回调 */
    lv_obj_set_width(slider_x, lv_obj_get_width(chart) - 3.5 * lv_obj_get_height(chart) / 10);                              /* 设置滑杆宽度 */
    lv_obj_set_height(slider_x, lv_obj_get_height(chart) / 10);                                                             /* 设置滑杆高度 */
    lv_obj_align_to(slider_x, chart, LV_ALIGN_BOTTOM_LEFT, lv_obj_get_height(chart) / 10, -lv_obj_get_height(chart) / 20);  /* 设置滑杆位置 */
    
    lv_obj_t* slider_y = lv_slider_create(lv_scr_act());                                                                    /* 定义并创建Y轴缩放滑杆 */
    lv_slider_set_range(slider_y, LV_IMG_ZOOM_NONE, LV_IMG_ZOOM_NONE * 10);                                                 /* 设置滑杆范围 */
    lv_obj_add_event_cb(slider_y, event_slider_y_cb, LV_EVENT_VALUE_CHANGED, chart);                                        /* 设置滑杆回调 */
    lv_obj_set_width(slider_y, lv_obj_get_height(chart) / 10);                                                              /* 设置滑杆宽度 */
    lv_obj_set_height(slider_y, lv_obj_get_height(chart) - 3.5 * lv_obj_get_height(chart) / 10);                            /* 设置滑杆高度 */
    lv_obj_align_to(slider_y, chart, LV_ALIGN_TOP_RIGHT, -lv_obj_get_height(chart) / 20, lv_obj_get_height(chart) / 10);    /* 设置滑杆位置 */

    lv_obj_add_event_cb(chart, event_chart_cb, LV_EVENT_VALUE_CHANGED, cursor);                                             /* 设置图表回调 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_chart();
}
