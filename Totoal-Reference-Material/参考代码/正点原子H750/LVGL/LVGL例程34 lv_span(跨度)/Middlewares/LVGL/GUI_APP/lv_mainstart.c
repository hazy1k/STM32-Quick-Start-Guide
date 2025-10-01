/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_span() 实验
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

#if (1 != LV_FONT_MONTSERRAT_10 || 1 != LV_FONT_MONTSERRAT_14 || 1 != LV_FONT_MONTSERRAT_16)
#error Please Make Sure Enable LV_FONT_MONTSERRAT_10 & LV_FONT_MONTSERRAT_14 & LV_FONT_MONTSERRAT_16
#endif

/* 获取当前活动屏幕的宽高 */
#define scr_act_width() lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

/**
 * @brief  例
 * @param  无
 * @return 无
 */
static void lv_example_span(void)
{
    lv_obj_t* spangroup = lv_spangroup_create(lv_scr_act());                                        /* 定义并创建spangroup */
    lv_obj_set_width(spangroup, 200);                                                               /* 设置spangroup宽度 */
    lv_obj_set_height(spangroup, 200);                                                              /* 设置spangroup高度 */
    lv_obj_center(spangroup);                                                                       /* 设置spangroup位置 */
    lv_obj_set_style_border_width(spangroup, 1, LV_PART_MAIN);                                      /* 设置spangroup边框宽度 */
    lv_obj_set_style_border_color(spangroup, lv_palette_main(LV_PALETTE_ORANGE), LV_PART_MAIN);     /* 设置spangroup边框颜色 */
    lv_spangroup_set_align(spangroup, LV_TEXT_ALIGN_LEFT);                                          /* 设置spangroup文本对齐方式 */
    lv_spangroup_set_overflow(spangroup, LV_SPAN_OVERFLOW_CLIP);                                    /* 设置spangroup溢出 */
    lv_spangroup_set_indent(spangroup, 20);                                                         /* 设置spangroup缩进 */
    lv_spangroup_set_mode(spangroup, LV_SPAN_MODE_BREAK);                                           /* 设置spangroup模式 */

    lv_span_t* span = lv_spangroup_new_span(spangroup);                                             /* 创建并新建span */
    lv_span_set_text(span, "This is span 1.");                                                      /* 设置span文本 */
    lv_style_set_text_color(&span->style, lv_palette_main(LV_PALETTE_RED));                         /* 设置span文本颜色 */
    lv_style_set_text_decor(&span->style, LV_TEXT_DECOR_STRIKETHROUGH | LV_TEXT_DECOR_UNDERLINE);   /* 设置span文本格式 */
    lv_style_set_text_opa(&span->style, LV_OPA_30);                                                 /* 设置span文本透明度 */

    span = lv_spangroup_new_span(spangroup);                                                        /* 新建span */
    lv_span_set_text_static(span, "This is span 2.");                                               /* 设置span文本 */
    lv_style_set_text_font(&span->style, &lv_font_montserrat_16);                                   /* 设置span文本字体 */
    lv_style_set_text_color(&span->style, lv_palette_main(LV_PALETTE_GREEN));                       /* 设置span文本颜色 */
                                                                                                       
    span = lv_spangroup_new_span(spangroup);                                                        /* 新建span */
    lv_span_set_text_static(span, "This is span 3.");                                               /* 设置span文本 */
    lv_style_set_text_color(&span->style, lv_palette_main(LV_PALETTE_BLUE));                        /* 设置span文本颜色 */
                                                                                                       
    span = lv_spangroup_new_span(spangroup);                                                        /* 新建span */
    lv_span_set_text_static(span, "This is span 4.");                                               /* 设置span文本 */
    lv_style_set_text_color(&span->style, lv_palette_main(LV_PALETTE_GREEN));                       /* 设置span文本颜色 */
    lv_style_set_text_font(&span->style, &lv_font_montserrat_10);                                   /* 设置span文本字体 */
    lv_style_set_text_decor(&span->style, LV_TEXT_DECOR_UNDERLINE);                                 /* 设置span文本格式 */
                                                                                                       
    span = lv_spangroup_new_span(spangroup);                                                        /* 新建span */
    lv_span_set_text(span, "This is span 5.");                                                      /* 设置span文本 */
                                                                                                       
    lv_spangroup_refr_mode(spangroup);                                                              /* 更新spangroup模式 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_span();
}
