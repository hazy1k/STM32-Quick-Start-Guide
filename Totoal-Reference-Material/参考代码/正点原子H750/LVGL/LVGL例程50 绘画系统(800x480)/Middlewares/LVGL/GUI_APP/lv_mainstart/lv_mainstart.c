/**
 ****************************************************************************************************
 * @file        lv_mainstart.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-23
 * @brief       LVGL 驱动
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
 
#include "./lv_mainstart/lv_mainstart.h"
#include "lvgl.h"

#if (1 != LV_FONT_MONTSERRAT_14 || 1 != LV_FONT_MONTSERRAT_18 || 1 != LV_FONT_MONTSERRAT_22)
#error Please Make Sure Enable LV_FONT_MONTSERRAT_14 & LV_FONT_MONTSERRAT_18 & LV_FONT_MONTSERRAT_22
#endif

/* 获取当前活动屏幕的宽高 */
#define scr_act_width() lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

#ifdef __cplusplus
extern "C" {
#endif
    LV_IMG_DECLARE(img_zdyz);
#ifdef __cplusplus
}
#endif

/**
 * @brief  后退按钮事件回调
 * @param  无
 * @return 无
 */
static void event_btn_back_cb(lv_event_t* e)
{
    lv_obj_t* textarea = (lv_obj_t*)e->user_data;

    lv_textarea_set_text(textarea, LV_SYMBOL_LEFT);
}

/**
 * @brief  前进按钮事件回调
 * @param  无
 * @return 无
 */
static void event_btn_forward_cb(lv_event_t* e)
{
    lv_obj_t* textarea = (lv_obj_t*)e->user_data;

    lv_textarea_set_text(textarea, LV_SYMBOL_RIGHT);
}

/**
 * @brief  刷新按钮事件回调
 * @param  无
 * @return 无
 */
static void event_btn_refresh_cb(lv_event_t* e)
{
    lv_obj_t* textarea = (lv_obj_t*)e->user_data;

    lv_textarea_set_text(textarea, LV_SYMBOL_REFRESH);
}

/**
 * @brief  例
 * @param  无
 * @return 无
 */
static void lv_example_win(void)
{
    const lv_font_t* font;
    /* 根据屏幕大小设置字体 */
    if (scr_act_width() <= 320)
        font = &lv_font_montserrat_14;
    else if (scr_act_width() <= 480)
        font = &lv_font_montserrat_18;
    else
        font = &lv_font_montserrat_22;

    lv_coord_t dev_num = scr_act_width() <= 480 ? 6 : 7;

    lv_obj_t* win = lv_win_create(lv_scr_act(), scr_act_height() / dev_num);                    /* 定义并创建win */
    lv_obj_t* btn_back = lv_win_add_btn(win, LV_SYMBOL_LEFT, scr_act_height() / dev_num);       /* win添加按钮 */
    lv_obj_t* btn_forward = lv_win_add_btn(win, LV_SYMBOL_RIGHT, scr_act_height() / dev_num);   /* win添加按钮 */
    lv_obj_t* btn_refresh = lv_win_add_btn(win, LV_SYMBOL_REFRESH, scr_act_height() / dev_num); /* win添加按钮 */
    lv_obj_t* title = lv_win_add_title(win, "http://www.openedv.com/");                         /* win添加标题 */

    lv_obj_t* content = lv_win_get_content(win);                                                /* 获取win的内容物体 */
    lv_obj_t* textarea = lv_textarea_create(content);
    lv_obj_center(textarea);
    lv_obj_set_width(textarea, scr_act_width() * 0.7);
    lv_obj_align(lv_textarea_get_label(textarea), LV_ALIGN_CENTER, 0, 0);   
    lv_textarea_set_one_line(textarea, true);
    lv_obj_set_style_text_font(textarea, font, LV_PART_MAIN);
    lv_obj_update_layout(textarea);

    lv_obj_t* img = lv_img_create(content);
    lv_img_set_src(img, &img_zdyz);
    if (scr_act_width() <= 320)
        lv_img_set_zoom(img, 160);
    else if (scr_act_width() <= 480)
        lv_img_set_zoom(img, 256);
    else
        lv_img_set_zoom(img, 300);
    lv_obj_align_to(img, lv_scr_act(), LV_ALIGN_CENTER, 0, -lv_obj_get_height(textarea) * 1.5);

    lv_obj_add_event_cb(btn_back, event_btn_back_cb, LV_EVENT_CLICKED, textarea);
    lv_obj_add_event_cb(btn_forward, event_btn_forward_cb, LV_EVENT_CLICKED, textarea);
    lv_obj_add_event_cb(btn_refresh, event_btn_refresh_cb, LV_EVENT_CLICKED, textarea);
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_win();
}
