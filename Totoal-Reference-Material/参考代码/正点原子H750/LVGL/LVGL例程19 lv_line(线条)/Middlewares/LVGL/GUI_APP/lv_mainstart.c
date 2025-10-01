/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_line(线条) 实验
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
#include "math.h"


#define scr_act_width() lv_obj_get_width(lv_scr_act())          /* 获取当前活动屏幕的宽度 */
#define scr_act_height() lv_obj_get_height(lv_scr_act())        /* 获取当前活动屏幕的高度 */

#define SIN_POINTS_NUM   (200)                                  /* 正弦波坐标点数量 */

static lv_point_t sin_line_points[SIN_POINTS_NUM];              /* 正弦波坐标数组 */
static lv_point_t straigh_line_points[] = {{0,0},{100,0}};      /* 直线坐标数组 */
static const lv_font_t *font;                                   /* 定义字体 */

/**
 * @brief       产生正弦波坐标点
 * @param       maxval : 峰值
 * @param       samples: 坐标点的个数
 * @retval      无
 */
static void create_sin_buf(uint16_t maxval, uint16_t samples)
{
    uint16_t i;
    float y = 0;

    /*
     * 正弦波最小正周期为2π，约等于2 * 3.1415926
     * 曲线上相邻的两个点在x轴上的间隔 = 2 * 3.1415926 / 采样点数量
     */
    float inc = (2 * 3.1415926) / samples;                      /* 计算相邻两个点的x轴间隔 */

    for (i = 0; i < samples; i++)                               /* 连续产生samples个点 */
    {
        /*
         * 正弦波函数解析式：y = Asin(wx + φ）+ b
         * 计算每个点的y值，将峰值放大maxval倍，并将曲线向上偏移maxval到正数区域
         */
        y = maxval * sin(inc * i) + maxval;

        sin_line_points[i].x = 2 * i;                           /* 存入x轴坐标 */
        sin_line_points[i].y = y;                               /* 存入y轴坐标 */
    }
}

/**
 * @brief  正弦波实例
 * @param  无
 * @return 无
 */
static void lv_example_line(void)
{
    /* 根据活动屏幕宽度选择字体 */
    if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
    }
    else
    {
        font = &lv_font_montserrat_30;
    }

    /* 标题 */
    lv_obj_t *label = lv_label_create(lv_scr_act());                                        /* 创建标签 */
    lv_label_set_text(label, "Line");                                                       /* 设置文本内容 */
    lv_obj_set_style_text_font(label, font, LV_STATE_DEFAULT);                              /* 设置字体 */
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, scr_act_width() / 20, scr_act_height()/16);      /* 设置位置 */

    /* 直线（用作下划线）*/
    lv_obj_t *straigh_line = lv_line_create(lv_scr_act());                                  /* 创建线条 */
    lv_line_set_points(straigh_line, straigh_line_points, 2);                               /* 设置线条坐标点 */
    lv_obj_align_to(straigh_line, label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);                   /* 设置位置 */

    /* 正弦波 */
    lv_obj_t *sin_line = lv_line_create(lv_scr_act());                                      /* 创建线条 */
    create_sin_buf(scr_act_height()/4, SIN_POINTS_NUM);                                     /* 产生正弦波坐标点 */
    lv_line_set_points(sin_line, sin_line_points, SIN_POINTS_NUM);                          /* 设置线条坐标点 */
    lv_obj_center(sin_line);                                                                /* 设置位置 */
    lv_obj_set_style_line_width(sin_line, 8, LV_PART_MAIN);                                 /* 设置线的宽度 */
    lv_obj_set_style_line_color(sin_line, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);  /* 设置线的颜色 */
    lv_obj_set_style_line_rounded(sin_line, true, LV_PART_MAIN);                            /* 设置线条圆角 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_line();         /* 正弦波实例 */
}



