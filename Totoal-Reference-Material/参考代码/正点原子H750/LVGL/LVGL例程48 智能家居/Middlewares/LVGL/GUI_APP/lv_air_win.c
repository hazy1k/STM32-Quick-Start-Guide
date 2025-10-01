/**
 ****************************************************************************************************
 * @file        lv_air_win.c
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
#include "./LVGL/GUI_APP/lv_air_win.h"
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "./LVGL/GUI_APP/lv_light_win.h"


char* lv_air_btn_name[6] = { "干燥","静音","制冷","健康","节能","制冷"};
lv_obj_t* lv_air_btn[6];
lv_obj_t* lv_air_btn_label[6];

#define lv_object_number                    6
#define lv_air_btn_w                        150
#define lv_air_btn_h                        70
#define lv_air_btn_number_of_rows           3
#define lv_air_btn_object_clearance         15
#define lv_air_btn_Top_and_bottom_clearance 15

/* air_app就绪表 */
unsigned int  air_app_readly_list[32];
/* air_app触发位 */
int lv_air_trigger_bit = 0;
/* air_app状态表 */
int air_app[lv_object_number] = { 0,0,0,0,0,0 };

/**
  * @brief  空调回调函数
  * @param  obj  :对象
  * @param  event:事件
  * @retval 无
  */
static void lv_air_control_event_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* obj = lv_event_get_target(event);

    if (code == LV_EVENT_CLICKED)
    {
        for (int i = 0; i < lv_object_number; i++)
        {
            if (obj == lv_air_btn[i])
            {
                air_app_readly_list[i] = 1;                                             /* app就绪表位置1 */
            }
        }

        lv_air_trigger_bit = ((unsigned int)lv_clz((air_app_readly_list)));             /* 计算前导指令 */
        air_app[lv_air_trigger_bit] ++;
        air_app_readly_list[lv_air_trigger_bit] = 0;                                    /* 该位清零就绪表 */

        switch (lv_air_trigger_bit)                                                     /* 根据该位做相应的函数 */
        {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                for (int i = 0; i < lv_object_number; i++)
                {
                    if (obj == lv_air_btn[i])
                    {
                        lv_obj_set_style_bg_color(lv_air_btn[i], lv_palette_main(LV_PALETTE_BLUE), LV_STATE_DEFAULT);
                    }
                    else
                    {
                        lv_obj_set_style_bg_color(lv_air_btn[i], lv_palette_main(LV_PALETTE_GREY), LV_STATE_DEFAULT);
                    }
                }
                break;
            default:
                break;
        }
    }
}

/**
  * @brief  空调UI
  * @param  parent  :父类
  * @retval 无
  */
void lv_air_conditioning_win(lv_obj_t* parent)
{
    int lv_index = 0;
    /* 设置该容器不可滑动 */
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* lv_air_image_bg = lv_img_create(parent);
    lv_img_set_src(lv_air_image_bg, &lv_light_bg);
    lv_obj_center(lv_air_image_bg);

    lv_obj_t* lv_air_t = lv_img_create(parent);
    lv_img_set_src(lv_air_t, &air_conditioning_control_win);
    lv_obj_align(lv_air_t, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_img_recolor_opa(lv_air_t, 255, LV_PART_MAIN);
    lv_obj_set_style_img_recolor(lv_air_t, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);

    lv_obj_t* lv_air_name = lv_label_create(parent);
    lv_obj_set_style_text_color(lv_air_name, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lv_air_name, &myFont18, LV_STATE_DEFAULT);
    lv_label_set_text(lv_air_name, "空调控制");
    lv_obj_align_to(lv_air_name, lv_air_t, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, 0);

    lv_obj_t* air_temperature = lv_label_create(parent);
    lv_obj_set_style_text_color(air_temperature, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(air_temperature, &myFont18, LV_STATE_DEFAULT);
    lv_label_set_text(air_temperature, "当前空调温度");
    lv_obj_align_to(air_temperature, lv_air_name, LV_ALIGN_OUT_BOTTOM_MID, 0, 50);

    lv_obj_t* air_temperature_number = lv_label_create(parent);
    lv_obj_set_style_text_color(air_temperature_number, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(air_temperature_number, &myFont72, LV_STATE_DEFAULT);
    lv_label_set_text(air_temperature_number, "23");
    lv_obj_align_to(air_temperature_number, air_temperature, LV_ALIGN_OUT_BOTTOM_MID, -20, 30);

    lv_obj_t* air_temperature_suffix = lv_label_create(parent);
    lv_obj_set_style_text_color(air_temperature_suffix, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(air_temperature_suffix, &myFont48, LV_STATE_DEFAULT);
    lv_label_set_text(air_temperature_suffix, "℃");
    lv_obj_align_to(air_temperature_suffix, air_temperature_number, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

    lv_air_btn[lv_index] = lv_btn_create(parent);
    lv_obj_set_size(lv_air_btn[lv_index], lv_air_btn_w, lv_air_btn_h);
    lv_obj_set_style_bg_color(lv_air_btn[lv_index],lv_palette_main(LV_PALETTE_GREY),LV_STATE_DEFAULT);
    lv_obj_align_to(lv_air_btn[lv_index], air_temperature_number, LV_ALIGN_BOTTOM_MID, 0, 80);
    lv_obj_add_event_cb(lv_air_btn[lv_index], lv_air_control_event_handler, LV_EVENT_ALL, NULL);

    lv_air_btn_label[lv_index] = lv_label_create(lv_air_btn[lv_index]);
    lv_obj_set_style_text_color(lv_air_btn_label[lv_index], lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lv_air_btn_label[lv_index], &myFont18, LV_STATE_DEFAULT);
    lv_label_set_text(lv_air_btn_label[lv_index], lv_air_btn_name[lv_index]);
    lv_obj_center(lv_air_btn_label[lv_index]);

    lv_index++;

    /* 使用程序把多个btn对象按照一定的顺序排列 */
    for (lv_index = 1; lv_index < 6; lv_index++)
    {
        lv_air_btn[lv_index] = lv_btn_create(parent);
        lv_obj_set_size(lv_air_btn[lv_index], lv_air_btn_w, lv_air_btn_h);
        lv_obj_set_style_bg_color(lv_air_btn[lv_index], lv_palette_main(LV_PALETTE_GREY), LV_STATE_DEFAULT);
        lv_obj_add_event_cb(lv_air_btn[lv_index], lv_air_control_event_handler, LV_EVENT_ALL, NULL);

        if (lv_index < lv_air_btn_number_of_rows)
        {
            lv_obj_align_to(lv_air_btn[lv_index], lv_air_btn[lv_index - 1], LV_ALIGN_OUT_RIGHT_MID, lv_air_btn_object_clearance, 0);
        }
        else if (lv_index == lv_air_btn_number_of_rows)
        {
            lv_obj_align_to(lv_air_btn[lv_index], lv_air_btn[0], LV_ALIGN_OUT_BOTTOM_MID, 0, lv_air_btn_Top_and_bottom_clearance);
        }
        else
        {
            lv_obj_align_to(lv_air_btn[lv_index], lv_air_btn[lv_index - 1], LV_ALIGN_OUT_RIGHT_MID, lv_air_btn_object_clearance, 0);
        }

        lv_air_btn_label[lv_index] = lv_label_create(lv_air_btn[lv_index]);
        lv_obj_set_style_text_color(lv_air_btn_label[lv_index], lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(lv_air_btn_label[lv_index], &myFont18, LV_STATE_DEFAULT);
        lv_label_set_text(lv_air_btn_label[lv_index], lv_air_btn_name[lv_index]);
        lv_obj_center(lv_air_btn_label[lv_index]);

    }
}
