/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       LVGL 智能家居实验
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
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "./LVGL/GUI_APP/lv_air_win.h"
#include "./LVGL/GUI_APP/lv_light_win.h"
#include "./LVGL/GUI_APP/lv_user_win.h"
#include "./LVGL/GUI_APP/lv_curtain_win.h"


lv_obj_t* lv_tabview_win;
lv_obj_t* lv_tabview_tab_light_contro_win;
lv_obj_t* lv_tabview_tab_air_conditioning_control_win;
lv_obj_t* lv_tabview_tab_curtain_control_win;
lv_obj_t* lv_tabview_uase_win;


/* 描述主界面结构体图标 */
typedef struct
{
    const void* app_image;
}app_image_info;

/* 主界面所需图标 */
static const app_image_info app_image[] =
{
    {&light_contro_win},
    {&air_conditioning_control_win},
    {&curtain_control_wi},
    {&uase_win},
};

/* 获取图标的个数 */
#define image_mun (int)(sizeof(app_image)/sizeof(app_image[0]))

/**
  * @brief  tabview回调函数
  * @param  e：事件
  * @retval 无
  */
static void lv_tab_btns_event_cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);

    /* 绘画开始事件 */
    if (code == LV_EVENT_DRAW_PART_BEGIN)
    {
        lv_obj_draw_part_dsc_t* dsc = lv_event_get_param(e);
        
        dsc->rect_dsc->radius = 0;
        dsc->rect_dsc->border_color = lv_color_make(0, 0, 0);
        dsc->rect_dsc->border_width = 1;
        dsc->rect_dsc->bg_img_recolor_opa = 255;
        dsc->rect_dsc->bg_img_recolor = lv_color_make(255, 255, 255);
        lv_img_header_t header;
        lv_res_t res = lv_img_decoder_get_info(app_image[0].app_image, &header);
        if (res != LV_RES_OK) return;

        lv_area_t a;
        a.x1 = dsc->draw_area->x1 + (lv_area_get_width(dsc->draw_area) - header.w) / 2;
        a.x2 = a.x1 + header.w - 1;
        a.y1 = dsc->draw_area->y1 + (lv_area_get_height(dsc->draw_area) - header.h) / 2;
        a.y2 = a.y1 + header.h - 1;

        lv_draw_img_dsc_t img_draw_dsc;
        lv_draw_img_dsc_init(&img_draw_dsc);
        img_draw_dsc.recolor = lv_color_white();
        img_draw_dsc.recolor_opa = 255;
        
        if (dsc->id == 0)
        {
            if (lv_btnmatrix_get_selected_btn(obj) == dsc->id)  img_draw_dsc.recolor_opa = LV_OPA_30;
            lv_draw_img(dsc->draw_ctx, &img_draw_dsc, &a, app_image[0].app_image);
        }
        if (dsc->id == 1)
        {
            if (lv_btnmatrix_get_selected_btn(obj) == dsc->id)  img_draw_dsc.recolor_opa = LV_OPA_30;

            lv_draw_img(dsc->draw_ctx, &img_draw_dsc, &a, app_image[1].app_image);

        }
        if (dsc->id == 2)
        {
            if (lv_btnmatrix_get_selected_btn(obj) == dsc->id)  img_draw_dsc.recolor_opa = LV_OPA_30;
            lv_draw_img(dsc->draw_ctx, &img_draw_dsc, &a, app_image[2].app_image);
        }
        if (dsc->id == 3)
        {
            if (lv_btnmatrix_get_selected_btn(obj) == dsc->id)  img_draw_dsc.recolor_opa = LV_OPA_30;
            lv_draw_img(dsc->draw_ctx, &img_draw_dsc, &a, app_image[3].app_image);

        }
    }
}

/**
  * @brief  LVGL 入口
  * @param  无
  * @retval 无
  */
void lv_mainstart(void)
{
    /* 创建tabview控件 */
    lv_tabview_win = lv_tabview_create(lv_scr_act(), LV_DIR_LEFT, 80);

    /* 创建五个选项卡容器 */
    lv_tabview_tab_light_contro_win = lv_tabview_add_tab(lv_tabview_win, " ");
    lv_tabview_tab_air_conditioning_control_win = lv_tabview_add_tab(lv_tabview_win, " ");
    lv_tabview_tab_curtain_control_win = lv_tabview_add_tab(lv_tabview_win, " ");
    lv_tabview_uase_win = lv_tabview_add_tab(lv_tabview_win, " ");

    /* 矩阵按键的对象 */
    lv_obj_t* tab_btns = lv_tabview_get_tab_btns(lv_tabview_win);
    lv_obj_set_style_bg_color(tab_btns, lv_color_make(59, 57, 71), LV_STATE_DEFAULT);
    lv_obj_add_event_cb(tab_btns, lv_tab_btns_event_cb, LV_EVENT_ALL, NULL);

    /* 设置选项卡的容器内容 */
    lv_light_win(lv_tabview_tab_light_contro_win);
    lv_air_conditioning_win(lv_tabview_tab_air_conditioning_control_win);
    lv_curtain_win(lv_tabview_tab_curtain_control_win);
    lv_user_win(lv_tabview_uase_win);

}
