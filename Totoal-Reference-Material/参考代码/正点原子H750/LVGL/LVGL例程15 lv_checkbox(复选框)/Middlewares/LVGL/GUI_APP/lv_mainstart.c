/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL lv_checkbox(复选框) 实验
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
#define scr_act_width() lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

static int16_t aggregate = 0;                   /* 定义总价格 */

static const lv_font_t *font;                   /* 定义字体 */

static lv_obj_t *label_menu;                    /* 菜单标题标签 */
static lv_obj_t *label_aggregate;               /* 总价格标签 */

/* 定义复选框 */
static lv_obj_t *checkbox1;
static lv_obj_t *checkbox2;
static lv_obj_t *checkbox3;
static lv_obj_t *checkbox4;

/**
 * @brief  回调事件
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void checkbox_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);                              /* 获取触发源 */

    if(target == checkbox1)                                                 /* 复选框1触发 */
    {
        lv_obj_has_state(checkbox1, LV_STATE_CHECKED) ? (aggregate += 19) : (aggregate -= 19);
    }
    else if(target == checkbox2)                                            /* 复选框2触发 */
    {
        lv_obj_has_state(checkbox2, LV_STATE_CHECKED) ? (aggregate += 29) : (aggregate -= 29);
    }

    lv_label_set_text_fmt(label_aggregate, "Aggregate  :  $%d", aggregate); /* 更新总价格标签 */
}

/**
 * @brief  标题、总价格标签
 * @param  无
 * @return 无
 */
static void lv_example_label(void)
{
    /* 根据活动屏幕宽度选择字体 */
    if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
    }
    else
    {
        font = &lv_font_montserrat_20;
    }
    
    /* 菜单标题标签 */
    label_menu = lv_label_create(lv_scr_act());
    lv_label_set_text(label_menu, "MENU");
    lv_obj_set_style_text_font(label_menu, font, LV_STATE_DEFAULT);
    lv_obj_align(label_menu, LV_ALIGN_CENTER, 0, -scr_act_height() * 2 / 5 );

    /* 总价格标签 */
    label_aggregate = lv_label_create(lv_scr_act());
    lv_label_set_text(label_aggregate, "Aggregate  :  $0");
    lv_obj_set_style_text_font(label_aggregate, font, LV_STATE_DEFAULT);
    lv_obj_align(label_aggregate, LV_ALIGN_CENTER, 0, scr_act_height() * 2 / 5 );
}

/**
 * @brief  菜品复选框
 * @param  无
 * @return 无
 */
static void lv_example_checkbox(void)
{
    /* 创建基础对象作为背景 */
    lv_obj_t *obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj, scr_act_width() * 4 /5 , scr_act_height() * 3 /5);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0 , 0);

    /* 菜品1复选框 */
    checkbox1 = lv_checkbox_create(obj);
    lv_checkbox_set_text(checkbox1, "Roast chicken     $19");
    lv_obj_set_style_text_font(checkbox1, font, LV_STATE_DEFAULT);
    lv_obj_align(checkbox1, LV_ALIGN_LEFT_MID, 0, -scr_act_height() / 5 );
    lv_obj_add_event_cb(checkbox1, checkbox_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    /* 菜品2复选框 */
    checkbox2 = lv_checkbox_create(obj);
    lv_checkbox_set_text(checkbox2, "Roast duck     $29");
    lv_obj_set_style_text_font(checkbox2, font, LV_STATE_DEFAULT);
    lv_obj_align_to(checkbox2, checkbox1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 16);
    lv_obj_add_event_cb(checkbox2, checkbox_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    /* 菜品3复选框 */
    checkbox3 = lv_checkbox_create(obj);
    lv_checkbox_set_text(checkbox3, "Roast fish     $39");
    lv_obj_set_style_text_font(checkbox3, font, LV_STATE_DEFAULT);
    lv_obj_align_to(checkbox3, checkbox2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 16);
    lv_obj_add_state(checkbox3, LV_STATE_DISABLED);

    /* 菜品4复选框 */
    checkbox4 = lv_checkbox_create(obj);
    lv_checkbox_set_text(checkbox4, "Roast lamb     $69");
    lv_obj_set_style_text_font(checkbox4, font, LV_STATE_DEFAULT);
    lv_obj_align_to(checkbox4, checkbox3, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 16);
    lv_obj_add_state(checkbox4, LV_STATE_DISABLED);
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_label();     /* 菜单标题、总价标签 */
    lv_example_checkbox();  /* 菜品复选框 */
}



