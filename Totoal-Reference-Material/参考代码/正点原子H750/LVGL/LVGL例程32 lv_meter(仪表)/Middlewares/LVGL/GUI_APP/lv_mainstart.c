/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_meter(仪表)  实验
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

#if (1 != LV_FONT_MONTSERRAT_8 || 1 != LV_FONT_MONTSERRAT_10 || 1 != LV_FONT_MONTSERRAT_14 || 1 != LV_FONT_MONTSERRAT_16)
#error Please Make Sure Enable LV_FONT_MONTSERRAT_8 & LV_FONT_MONTSERRAT_10 & LV_FONT_MONTSERRAT_14 & LV_FONT_MONTSERRAT_16
#endif

/* 获取当前活动屏幕的宽高 */
#define scr_act_width() lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

/**
 * @brief  例1指针动画
 * @param  无
 * @return 无
 */
static void anim_1_indic_cb(lv_obj_t* meter, int32_t value)
{
    lv_meter_indicator_t* indic = (lv_meter_indicator_t*)meter->user_data;

    lv_meter_set_indicator_value(meter, indic, value);
}

/**
 * @brief  例1
 * @param  无
 * @return 无
 */
static void lv_example_meter_1(void)
{
    const lv_font_t* font;
    /* 根据屏幕大小设置字体 */
    if (scr_act_width() <= 320)
        font = &lv_font_montserrat_10;
    else if (scr_act_width() <= 480)
        font = &lv_font_montserrat_14;
    else
        font = &lv_font_montserrat_16;

    lv_obj_t* meter = lv_meter_create(lv_scr_act());                                                                    /* 定义并创建仪表 */
    lv_obj_set_width(meter, scr_act_height() * 0.4);                                                                    /* 设置仪表宽度 */
    lv_obj_set_height(meter, scr_act_height() * 0.4);                                                                   /* 设置仪表高度 */
    lv_obj_align(meter, LV_ALIGN_CENTER, -scr_act_width() / 3, 0);                                                      /* 设置仪表位置 */
    lv_obj_set_style_text_font(meter, font, LV_PART_MAIN);                                                              /* 设置仪表文字字体 */
    /* 设置仪表刻度 */
    lv_meter_scale_t* scale = lv_meter_add_scale(meter);                                                                /* 定义并添加刻度 */
    lv_meter_set_scale_ticks(meter, scale, 41, 1, scr_act_height() / 80, lv_palette_main(LV_PALETTE_GREY));             /* 设置小刻度 */
    lv_meter_set_scale_major_ticks(meter, scale, 8, 1, scr_act_height() / 60, lv_color_black(), scr_act_height() / 30); /* 设置主刻度 */
    /* 设置指针 */
    lv_meter_indicator_t* indic;
    indic = lv_meter_add_arc(meter, scale, 2, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_meter_set_indicator_start_value(meter, indic, 0);
    lv_meter_set_indicator_end_value(meter, indic, 20);
    indic = lv_meter_add_scale_lines(meter, scale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE), false, 0);
    lv_meter_set_indicator_start_value(meter, indic, 0);
    lv_meter_set_indicator_end_value(meter, indic, 20);
    indic = lv_meter_add_arc(meter, scale, 2, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(meter, indic, 80);
    lv_meter_set_indicator_end_value(meter, indic, 100);
    indic = lv_meter_add_scale_lines(meter, scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false, 0);
    lv_meter_set_indicator_start_value(meter, indic, 80);
    lv_meter_set_indicator_end_value(meter, indic, 100);
    indic = lv_meter_add_needle_line(meter, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);
    lv_obj_set_user_data(meter, indic);
    /* 设置指针动画 */
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a,(lv_anim_exec_xcb_t)anim_1_indic_cb);
    lv_anim_set_var(&a, meter);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_time(&a, 2000);
    lv_anim_set_repeat_delay(&a, 100);
    lv_anim_set_playback_time(&a, 500);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}

static lv_obj_t* meter_2;

/**
 * @brief  例2指针动画
 * @param  无
 * @return 无
 */
static void anim_2_indic_cb(lv_meter_indicator_t* indic, int32_t value)
{
    lv_meter_set_indicator_end_value(meter_2, indic, value);
}

/**
 * @brief  例2
 * @param  无
 * @return 无
 */
static void lv_example_meter_2(void)
{
    const lv_font_t* font;
    /* 根据屏幕大小设置字体 */
    if (scr_act_width() <= 320)
        font = &lv_font_montserrat_8;
    else if (scr_act_width() <= 480)
        font = &lv_font_montserrat_10;
    else
        font = &lv_font_montserrat_14;

    meter_2 = lv_meter_create(lv_scr_act());                                                                            /* 定义并创建仪表 */
    lv_obj_set_width(meter_2, scr_act_height() * 0.4);                                                                  /* 设置仪表宽度 */
    lv_obj_set_height(meter_2, scr_act_height() * 0.4);                                                                 /* 设置仪表高度 */
    lv_obj_center(meter_2);                                                                                             /* 设置仪表位置 */
    lv_obj_set_style_text_font(meter_2, font, LV_PART_MAIN);                                                            /* 设置仪表文字字体 */
    lv_obj_remove_style(meter_2, NULL, LV_PART_INDICATOR);                                                              /* 删除仪表指针样式 */
    /* 设置仪表刻度 */
    lv_meter_scale_t* scale = lv_meter_add_scale(meter_2);
    lv_meter_set_scale_ticks(meter_2, scale, 11, 2, scr_act_height() / 15, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter_2, scale, 1, 2, scr_act_height() / 15, lv_color_hex3(0xEEE), scr_act_height() / 40);
    lv_meter_set_scale_range(meter_2, scale, 0, 100, 270, 90);
    /* 设置指针 */
    lv_meter_indicator_t* indic1 = lv_meter_add_arc(meter_2, scale, scr_act_height() / 45, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_indicator_t* indic2 = lv_meter_add_arc(meter_2, scale, scr_act_height() / 45, lv_palette_main(LV_PALETTE_GREEN), -scr_act_height() / 45);
    lv_meter_indicator_t* indic3 = lv_meter_add_arc(meter_2, scale, scr_act_height() / 45, lv_palette_main(LV_PALETTE_BLUE), -scr_act_height() / 22.5);
    /* 设置指针动画 */
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)anim_2_indic_cb);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_repeat_delay(&a, 100);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

    lv_anim_set_time(&a, 2000);
    lv_anim_set_playback_time(&a, 500);
    lv_anim_set_var(&a, indic1);
    lv_anim_start(&a);

    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_time(&a, 1000);
    lv_anim_set_var(&a, indic2);
    lv_anim_start(&a);

    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_time(&a, 2000);
    lv_anim_set_var(&a, indic3);
    lv_anim_start(&a);
}

/**
 * @brief  例3
 * @param  无
 * @return 无
 */
static void lv_example_meter_3(void)
{
    lv_obj_t* meter = lv_meter_create(lv_scr_act());                                                                    /* 定义并创建仪表 */
    lv_obj_remove_style(meter, NULL, LV_PART_MAIN);                                                                     /* 移除背景样式 */
    lv_obj_remove_style(meter, NULL, LV_PART_INDICATOR);                                                                /* 移除指针样式 */
    lv_obj_set_width(meter, scr_act_height() * 0.4);                                                                    /* 设置仪表宽度 */
    lv_obj_set_height(meter, scr_act_height() * 0.4);                                                                   /* 设置仪表高度 */
    lv_obj_align(meter, LV_ALIGN_CENTER, scr_act_width() / 3, 0);                                                       /* 设置仪表位置 */
    lv_obj_update_layout(meter);                                                                                        /* 手动更新参数 */
    /* 设置仪表刻度 */
    lv_meter_scale_t* scale = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale, 0, 0, 0, lv_color_black());
    lv_meter_set_scale_range(meter, scale, 0, 100, 360, 0);
    /* 设置指针 */
    lv_coord_t indic_w = scr_act_height() * 0.05;
    lv_meter_indicator_t* indic1 = lv_meter_add_arc(meter, scale, indic_w, lv_palette_main(LV_PALETTE_ORANGE), 0);
    lv_meter_set_indicator_start_value(meter, indic1, 0);
    lv_meter_set_indicator_end_value(meter, indic1, 40);

    lv_meter_indicator_t* indic2 = lv_meter_add_arc(meter, scale, indic_w, lv_palette_main(LV_PALETTE_YELLOW), 0);
    lv_meter_set_indicator_start_value(meter, indic2, 40);  /*Start from the previous*/
    lv_meter_set_indicator_end_value(meter, indic2, 80);

    lv_meter_indicator_t* indic3 = lv_meter_add_arc(meter, scale, indic_w, lv_palette_main(LV_PALETTE_DEEP_ORANGE), 0);
    lv_meter_set_indicator_start_value(meter, indic3, 80);  /*Start from the previous*/
    lv_meter_set_indicator_end_value(meter, indic3, 100);
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_meter_1();
    lv_example_meter_2();
    lv_example_meter_3();
}
