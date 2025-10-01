/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_imgbtn(图片按钮) 实验
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

static lv_point_t line_points[] = {{0,0},{0,120}};          /* 直线坐标数组 */

static const lv_font_t *font;                               /* 定义字体 */
static uint8_t cool_mode_state = 0;                         /* 制冷模式状态：0，关闭；1，开启 */
static uint8_t warm_mode_state = 0;                         /* 制暖模式状态：0，关闭；1，开启 */
static uint8_t dry_mode_state = 0;                          /* 干燥模式状态：0，关闭；1，开启 */

LV_IMG_DECLARE(img_cool);                                   /* 声明制冷按钮图片 */
LV_IMG_DECLARE(img_warm);                                   /* 声明制暖按钮图片 */
LV_IMG_DECLARE(img_dry);                                    /* 声明干燥按钮图片 */

/**
 * @brief  制冷按钮事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void cool_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);                              /* 获取触发源 */

    if(cool_mode_state == 0)                                                /* 判断按钮状态，如果为0，即关闭状态 */
    {
        cool_mode_state = 1;                                                /* 切换按钮状态 */
        lv_obj_set_style_img_recolor_opa(target, 255, 0);                   /* 设置图片重新着色透明度 */
        lv_obj_set_style_img_recolor(target, lv_color_hex(0x00a9ff), 0);    /* 设置图片重新着色：蓝色 */
    }
    else                                                                    /* 按钮为开启状态 */
    {
        cool_mode_state = 0;                                                /* 切换按钮状态 */
        lv_obj_set_style_img_recolor_opa(target, 255, 0);                   /* 设置图片重新着色透明度 */
        lv_obj_set_style_img_recolor(target, lv_color_hex(0x8a8a8a), 0);    /* 设置图片重新着色：灰色 */
    }
}

/**
 * @brief  制暖按钮事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void warm_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);                              /* 获取触发源 */

    if(warm_mode_state == 0)                                                /* 判断按钮状态，如果为0，即关闭状态 */
    {
        warm_mode_state = 1;                                                /* 切换按钮状态 */
        lv_obj_set_style_img_recolor_opa(target, 255, 0);                   /* 设置图片重新着色透明度 */
        lv_obj_set_style_img_recolor(target, lv_color_hex(0xff0000), 0);    /* 设置图片重新着色：红色 */
    }
    else
    {
        warm_mode_state = 0;                                                /* 切换按钮状态 */
        lv_obj_set_style_img_recolor_opa(target, 255, 0);                   /* 设置图片重新着色透明度 */
        lv_obj_set_style_img_recolor(target, lv_color_hex(0x8a8a8a), 0);    /* 设置图片重新着色：灰色 */
    }
}

/**
 * @brief  干燥按钮事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void dry_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);                              /* 获取触发源 */

    if(dry_mode_state == 0)                                                 /* 判断按钮状态，如果为0，即关闭状态 */
    {
        dry_mode_state = 1;                                                 /* 切换按钮状态 */
        lv_obj_set_style_img_recolor_opa(target, 255, 0);                   /* 设置图片重新着色透明度 */
        lv_obj_set_style_img_recolor(target, lv_color_hex(0x00a9ff), 0);    /* 设置图片重新着色：蓝色 */
    }
    else                                                                    /* 按钮为开启状态 */
    {
        dry_mode_state = 0;                                                 /* 切换按钮状态 */
        lv_obj_set_style_img_recolor_opa(target, 255, 0);                   /* 设置图片重新着色透明度 */
        lv_obj_set_style_img_recolor(target, lv_color_hex(0x8a8a8a), 0);    /* 设置图片重新着色：灰色 */
    }
}

/**
 * @brief  图片按钮实例
 * @param  无
 * @return 无
 */
static void lv_example_imgbtn(void)
{
    /* 根据活动屏幕宽度选择字体 */
    if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
    }
    else
    {
        font = &lv_font_montserrat_22;
    }

    /* 基础对象（用作背景） */
    lv_obj_t *obj = lv_obj_create(lv_scr_act());                                        /* 创建基础对象 */
    lv_obj_set_size(obj, scr_act_width()-10, 200);                                      /* 设置大小 */
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);                                           /* 设置位置 */

    /* 制冷模式（图片按钮） */
    lv_obj_t *imgbtn_cool = lv_imgbtn_create(obj);                                      /* 创建图片按钮 */
    lv_imgbtn_set_src(imgbtn_cool, LV_IMGBTN_STATE_RELEASED, NULL, &img_cool, NULL);    /* 设置图片源 */
    lv_obj_set_size(imgbtn_cool, 64, 64);                                               /* 设置大小 */
    lv_obj_align(imgbtn_cool, LV_ALIGN_CENTER, -scr_act_width()/3, -15);                /* 设置位置 */
    lv_obj_add_event_cb(imgbtn_cool, cool_event_cb, LV_EVENT_PRESSED, NULL);            /* 添加事件 */

    /* 制冷模式（标签） */
    lv_obj_t *label_cool = lv_label_create(obj);                                        /* 创建标签 */
    lv_label_set_text(label_cool, "Cool");                                              /* 设置文本 */
    lv_obj_set_style_text_font(label_cool, font, 0);                                    /* 设置字体 */
    lv_obj_align_to(label_cool, imgbtn_cool, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);           /* 设置位置 */

    /* 制暖模式（图片按钮） */
    lv_obj_t *imgbtn_warm = lv_imgbtn_create(obj);                                      /* 创建图片按钮 */
    lv_imgbtn_set_src(imgbtn_warm, LV_IMGBTN_STATE_RELEASED, NULL, &img_warm, NULL);    /* 设置图片源 */
    lv_obj_set_size(imgbtn_warm, 64, 64);                                               /* 设置大小 */
    lv_obj_align(imgbtn_warm, LV_ALIGN_CENTER, 0, -15);                                 /* 设置位置 */
    lv_obj_add_event_cb(imgbtn_warm, warm_event_cb, LV_EVENT_PRESSED, NULL);            /* 添加事件 */

    /* 制暖模式（标签） */
    lv_obj_t *label_warm = lv_label_create(obj);                                        /* 创建标签 */
    lv_label_set_text(label_warm, "Warm");                                              /* 设置文本 */
    lv_obj_set_style_text_font(label_warm, font, 0);                                    /* 设置字体 */
    lv_obj_align_to(label_warm, imgbtn_warm, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);           /* 设置位置 */

    /* 干燥模式（图片按钮） */
    lv_obj_t *imgbtn_dry = lv_imgbtn_create(obj);                                       /* 创建图片按钮 */
    lv_imgbtn_set_src(imgbtn_dry, LV_IMGBTN_STATE_RELEASED, NULL, &img_dry, NULL);      /* 设置图片源 */
    lv_obj_set_size(imgbtn_dry, 64, 64);                                                /* 设置大小 */
    lv_obj_align(imgbtn_dry, LV_ALIGN_CENTER, scr_act_width()/3, -15);                  /* 设置位置 */
    lv_obj_add_event_cb(imgbtn_dry, dry_event_cb, LV_EVENT_PRESSED, NULL);              /* 添加事件 */

    /* 干燥模式（标签） */
    lv_obj_t *label_dry = lv_label_create(obj);                                         /* 创建标签 */
    lv_label_set_text(label_dry, "Dry");                                                /* 设置文本 */
    lv_obj_set_style_text_font(label_dry, font, 0);                                     /* 设置字体 */
    lv_obj_align_to(label_dry, imgbtn_dry, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);             /* 设置位置 */
    
    /* 线条（分割线） */
    lv_obj_t *line_left = lv_line_create(obj);                                          /* 创建线条 */
    lv_line_set_points(line_left, line_points, 2);                                      /* 设置线条坐标点 */
    lv_obj_set_style_line_color(line_left, lv_color_hex(0xc6c6c6),LV_STATE_DEFAULT);    /* 设置线条颜色 */
    lv_obj_align(line_left, LV_ALIGN_CENTER, -scr_act_width()/6, 0);                    /* 设置位置 */

    lv_obj_t *line_right = lv_line_create(obj);                                         /* 创建线条 */
    lv_line_set_points(line_right, line_points, 2);                                     /* 设置线条坐标点 */
    lv_obj_set_style_line_color(line_right, lv_color_hex(0xc6c6c6),LV_STATE_DEFAULT);   /* 设置线条颜色 */
    lv_obj_align(line_right, LV_ALIGN_CENTER, scr_act_width()/6, 0);                    /* 设置位置 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_imgbtn();
}



