/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_img(图片) 实验
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

/* 定义图片部件、滑块部件 */
static lv_obj_t *img;
static lv_obj_t *slider_zoom, *slider_angle, *slider_r, *slider_g, *slider_b, *slider_opa;

LV_IMG_DECLARE(img_gear);                     /* 声明图片 */


/**
 * @brief  滑块事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void slider_event_cb(lv_event_t *e)
{
    lv_img_set_zoom(img, lv_slider_get_value(slider_zoom));                                         /* 设置图片缩放 */
    lv_img_set_angle(img, lv_slider_get_value(slider_angle));                                       /* 设置图片旋转角度 */

    /* 设置图片重新着色 */
    lv_obj_set_style_img_recolor(img,
                                 lv_color_make(lv_slider_get_value(slider_r), lv_slider_get_value(slider_g), lv_slider_get_value(slider_b)),
                                 LV_PART_MAIN);

    lv_obj_set_style_img_recolor_opa(img, lv_slider_get_value(slider_opa), LV_PART_MAIN);           /* 设置重新着色透明度 */
}

/**
 * @brief  创建滑块
 * @param  color:颜色值
 * @return *slider：创建成功的滑块部件
 */
static lv_obj_t *my_slider_create(lv_color_t color)
{
    lv_obj_t *slider = lv_slider_create(lv_scr_act());                                              /* 创建滑块 */
    lv_obj_set_height(slider, scr_act_height() / 20);                                               /* 设置高度 */
    lv_obj_set_width(slider, scr_act_width() / 3);                                                  /* 设置宽度 */
    lv_obj_remove_style(slider, NULL, LV_PART_KNOB);                                                /* 移除旋钮 */
    lv_obj_set_style_bg_color(slider,color,LV_PART_INDICATOR);                                      /* 设置滑块指示器颜色 */
    lv_obj_set_style_bg_color(slider,lv_color_darken(color, 100),LV_PART_MAIN);                     /* 设置滑块主体颜色、透明度 */
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);                     /* 设置滑块回调 */
    return slider;
}

/**
 * @brief  图片部件实例
 * @param  无
 * @return 无
 */
static void lv_example_img(void)
{
    img = lv_img_create(lv_scr_act());                                                              /* 创建图片部件 */
    lv_img_set_src(img, &img_gear);                                                                 /* 设置图片源 */
    lv_obj_align(img, LV_ALIGN_CENTER, -scr_act_width() / 5, 0);                                    /* 设置图片位置 */
    lv_obj_update_layout(img);                                                                      /* 更新图片参数 */

    /* 图片缩放控制滑块 */
    slider_zoom = my_slider_create(lv_color_hex(0x989c98));                                         /* 创建滑块 */
    lv_slider_set_range(slider_zoom, 128, 512);                                                     /* 设置滑块的范围 */
    lv_slider_set_value(slider_zoom, 256, LV_ANIM_OFF);                                             /* 设置滑块的值 */
    lv_obj_align(slider_zoom, LV_ALIGN_CENTER, scr_act_width() / 4, -scr_act_height() / 4);         /* 设置滑块位置 */

    /* 旋转角度控制滑块 */
    slider_angle = my_slider_create(lv_color_hex(0x989c98));                                        /* 创建滑块 */
    lv_slider_set_range(slider_angle, 0, 3600);                                                     /* 设置滑块的范围 */
    lv_obj_align_to(slider_angle, slider_zoom, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 20); /* 设置滑块位置 */

    /* 红色通道控制滑块 */
    slider_r = my_slider_create(lv_color_hex(0xff0000));                                            /* 创建滑块 */
    lv_slider_set_range(slider_r, 0, 255);                                                          /* 设置滑块的范围 */
    lv_obj_align_to(slider_r, slider_angle, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 20);    /* 设置滑块位置 */

    /* 绿色通道控制滑块 */
    slider_g = my_slider_create(lv_color_hex(0x00ff00));                                            /* 创建滑块 */
    lv_slider_set_range(slider_g, 0, 255);                                                          /* 设置滑块的范围 */
    lv_obj_align_to(slider_g, slider_r, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 20);        /* 设置滑块位置 */

    /* 蓝色通道控制滑块 */
    slider_b = my_slider_create(lv_color_hex(0x0000ff));                                            /* 创建滑块 */
    lv_slider_set_range(slider_b, 0, 255);                                                          /* 设置滑块的范围 */
    lv_obj_align_to(slider_b, slider_g, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 20);        /* 设置滑块位置 */

    /* 着色透明度控制滑块 */
    slider_opa = my_slider_create(lv_color_hex(0x000000));                                          /* 创建滑块 */
    lv_slider_set_range(slider_opa, 0, 255);                                                        /* 设置滑块的范围 */
    lv_slider_set_value(slider_opa, 150, LV_ANIM_OFF);                                              /* 设置滑块的值 */
    lv_obj_align_to(slider_opa, slider_b, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 20);      /* 设置滑块位置 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_img();
}



