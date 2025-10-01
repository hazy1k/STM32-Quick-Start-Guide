/**
 ****************************************************************************************************
 * @file        lv_curtain_win.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       LVGL 智能家具实验
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

#include "lvgl/lvgl.h"
#include <stdio.h>
#include "./LVGL/GUI_APP/lv_curtain_win.h"
#include "LVGL/GUI_APP/lv_mainstart.h"


LV_IMG_DECLARE(tianqi1)


/**
  * @brief  绘画回调函数
  * @param  e：事件
  * @retval 无
  */
static void draw_event_cb(lv_event_t* e)
{
    lv_obj_draw_part_dsc_t* dsc = lv_event_get_draw_part_dsc(e);
    if (!lv_obj_draw_part_check_type(dsc, &lv_chart_class, LV_CHART_DRAW_PART_TICK_LABEL)) return;

    if (dsc->id == LV_CHART_AXIS_PRIMARY_X && dsc->text) {
        const char* month[] = { "06时", "12时", "现在", "00时" };
        lv_snprintf(dsc->text, dsc->text_length, "%s", month[dsc->value]);
    }
}

/**
  * @brief  天气预报
  * @param  parent：父类
  * @retval 无
  */
void lv_curtain_win(lv_obj_t* parent)
{
    /* 创建一个容器 */
    lv_obj_t* lv_curtain_cont = lv_obj_create(parent);
    lv_obj_set_style_radius(lv_curtain_cont, 0, LV_STATE_DEFAULT);
    lv_obj_set_size(lv_curtain_cont,lv_obj_get_width(parent),lv_obj_get_height(parent));
    lv_obj_center(lv_curtain_cont);

    lv_obj_set_style_bg_color(lv_curtain_cont, lv_color_make(24, 25, 29), LV_STATE_DEFAULT);
    /* 设置该容器不能滑动 */
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);

    /* 创建一个小容器用来显示时间和温度 */
    lv_obj_t * lv_obj_l = lv_obj_create(lv_curtain_cont);
    lv_obj_set_style_radius(lv_obj_l, 20, LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(lv_obj_l, 10, LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(lv_obj_l, lv_color_make(255, 255, 255),LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(lv_obj_l, 1, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(lv_obj_l, lv_color_make(45, 46, 50), LV_STATE_DEFAULT);
    lv_obj_set_size(lv_obj_l,lv_obj_get_width(parent)/2 - 60,120);
    lv_obj_align(lv_obj_l,LV_ALIGN_TOP_LEFT,0,30);

    /* 设置今天的天气图标 */
    lv_obj_t* lv_tianqi1 = lv_img_create(lv_obj_l);
    lv_img_set_src(lv_tianqi1,&tianqi1);
    lv_img_set_zoom(lv_tianqi1,380);
    lv_obj_align(lv_tianqi1,LV_ALIGN_TOP_RIGHT,-10,10);

    lv_obj_t* lv_tianqi1_label1 = lv_label_create(lv_obj_l);
    lv_obj_set_style_text_color(lv_tianqi1_label1, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lv_tianqi1_label1,&lv_font_montserrat_36,LV_STATE_DEFAULT);
    lv_label_set_text(lv_tianqi1_label1,"35");

    lv_obj_t* lv_tianqi1_label2 = lv_label_create(lv_obj_l);
    lv_obj_set_style_text_color(lv_tianqi1_label2, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lv_tianqi1_label2, &myFont14, LV_STATE_DEFAULT);
    lv_label_set_text(lv_tianqi1_label2, "广州 晴");
    lv_obj_align_to(lv_tianqi1_label2, lv_tianqi1_label1, LV_ALIGN_OUT_RIGHT_MID, 10, 5);

    lv_obj_t* lv_tianqi1_label3 = lv_label_create(lv_obj_l);
    lv_obj_set_style_text_color(lv_tianqi1_label3, lv_palette_main(LV_PALETTE_YELLOW), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lv_tianqi1_label3, &myFont14, LV_STATE_DEFAULT);
    lv_label_set_text(lv_tianqi1_label3, "2020/4/28 星期四");
    lv_obj_align_to(lv_tianqi1_label3, lv_tianqi1_label1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

    /* 创建列表查看某段时间的天气 */
    lv_obj_t* list1 = lv_list_create(lv_curtain_cont);
    lv_obj_set_style_radius(list1, 20, LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(list1, 10, LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(list1, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(list1, 1, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(list1, lv_color_make(45, 46, 50), LV_STATE_DEFAULT);
    lv_obj_set_size(list1, lv_obj_get_width(parent) / 2 - 60, lv_obj_get_height(parent)/2 );
    lv_obj_align_to(list1,lv_obj_l, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_obj_set_style_text_font(list1, &myFont14, LV_STATE_DEFAULT);
    
    lv_obj_t* btn[5];

    btn[0] = lv_list_add_btn(list1, NULL, "4/28 星期四    晴天");
    btn[1] = lv_list_add_btn(list1, NULL, "4/29 星期五    多云");
    btn[2] = lv_list_add_btn(list1, NULL, "4/30 星期六    小雨");
    btn[3] = lv_list_add_btn(list1, NULL, "5/01 星期一    大雨");
    btn[4] = lv_list_add_btn(list1, NULL, "5/02 星期二    雷雨");
    
    for (int i = 0 ; i < 5 ; i ++)
    {
        lv_obj_set_style_bg_color(btn[i], lv_color_make(45, 46, 50), LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(btn[i],lv_color_white(),LV_STATE_DEFAULT);
    }

    /* 创建一个容器用来查看天气线图 */
    lv_obj_t * lv_obj_r = lv_obj_create(lv_curtain_cont);
    lv_obj_set_style_radius(lv_obj_r, 20, LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(lv_obj_r, 10, LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(lv_obj_r, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(lv_obj_r, 1, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(lv_obj_r, lv_color_make(45, 46, 50), LV_STATE_DEFAULT);
    lv_obj_set_size(lv_obj_r, lv_obj_get_width(parent) / 2 , lv_obj_get_height(lv_obj_l) + lv_obj_get_height(list1) + 20);
    lv_obj_align_to(lv_obj_r, lv_obj_l, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
    
    /* 创建一个线图 */
    lv_obj_t* chart;
    chart = lv_chart_create(lv_obj_r);
    lv_obj_set_size(chart, lv_obj_get_width(lv_obj_r) - 60 , lv_obj_get_height(lv_obj_r) - 100);
    lv_obj_set_style_radius(chart,0,LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(chart, 1, LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(chart, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(chart, lv_color_make(45, 46, 50), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(chart, &myFont14, LV_STATE_DEFAULT);
    lv_obj_align_to(chart,NULL,LV_ALIGN_CENTER,20,10);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 50);
    lv_chart_set_point_count(chart, 12);
    lv_chart_set_div_line_count(chart, 5, 5);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 6, 2, true, 50);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 1, 1, 4, 1, true, 40);
    lv_obj_add_event_cb(chart, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
    lv_chart_set_point_count(chart, 5);
    lv_chart_series_t* ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_next_value(chart, ser1, 31);
    lv_chart_set_next_value(chart, ser1, 34);
    lv_chart_set_next_value(chart, ser1, 32);
    lv_chart_set_next_value(chart, ser1, 40);
    lv_chart_set_next_value(chart, ser1, 41);

    lv_obj_t* lv_obj_r_label1 = lv_label_create(lv_obj_r);
    lv_obj_set_style_text_color(lv_obj_r_label1, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lv_obj_r_label1, &myFont14, LV_STATE_DEFAULT);
    lv_label_set_text(lv_obj_r_label1,"24H预报");
    lv_obj_align(lv_obj_r_label1,LV_ALIGN_TOP_LEFT,0,0);

    lv_obj_t* lv_obj_r_label2 = lv_label_create(lv_obj_r);
    lv_obj_set_style_text_color(lv_obj_r_label2, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lv_obj_r_label2, &myFont14, LV_STATE_DEFAULT);
    lv_label_set_text(lv_obj_r_label2, "Jun 18,2022");
    lv_obj_align(lv_obj_r_label2, LV_ALIGN_TOP_RIGHT, 0, 0);
}
