/**
 ****************************************************************************************************
 * @file        lv_user_win.c
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
#include "./LVGL/GUI_APP/lv_user_win.h"
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "./LVGL/GUI_APP/lv_light_win.h"


typedef struct
{
    char* app_text_Chinese; /* 中文 */
    const void* app_image;  /* 英文 */
    uint16_t app_witch;     /* 宽度 */
    uint16_t app_hietch;    /* 高度 */
}user_win_image_info;       /* 图标信息 */

/* 图标信息初始化 */
static const user_win_image_info lv_user_image[] =
{
    {" ",NULL,NULL,NULL},
    {"离家模式",&lv_leave_home,150,150},
    {"温馨模式",&lv_Warm,150,150},
    {"阅读模式",&lv_read,150,150},
    {"在家模式",&lv_home,150,150},
    {"娱乐模式",&lv_recreation,150,150},
    {"影院模式",&lv_cinema,150,150},
};

/* 获取路径的个数 */
#define lv_user_info_mun (int)(sizeof(lv_user_image)/sizeof(lv_user_image[0]))
/* 设置一个usert_app数组 */
lv_obj_t* lv_user_app_t[lv_user_info_mun];
/* 设置一个user_app名字数组 */
lv_obj_t* lv_user_app_name[lv_user_info_mun];
/* 设置一个user_app图片数组 */
lv_obj_t* lv_user_app_img[lv_user_info_mun];
/* user_app就绪表 */
unsigned int  user_app_readly_list[32];
/* user_app触发位 */
int lv_user_trigger_bit = 0;
/* user_app状态表 */
int user_app[lv_user_info_mun] = { 0,0,0,0,0,0 };

/**
  * @brief  APP按键回调函数
  * @param  event:事件
  * @retval 无
  */
static void lv_usert_control_event_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* obj = lv_event_get_target(event);

    if (code == LV_EVENT_CLICKED)
    {
        for (int i = 0; i < lv_user_info_mun; i++)
        {
            if (obj == lv_user_app_t[i])
            {
                user_app_readly_list[i] = 1;                                                /* app就绪表位置1 */
            }
        }

        lv_user_trigger_bit = ((unsigned int)lv_clz((user_app_readly_list)));               /* 计算前导指令 */
        user_app[lv_user_trigger_bit] ++;
        user_app_readly_list[lv_user_trigger_bit] = 0;                                      /* 该位清零就绪表 */

        switch (lv_user_trigger_bit)                                                        /* 根据该位做相应的函数 */
        {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                for (int i = 1; i < lv_user_info_mun; i++) /* 根据情景模式选择填充颜色 */
                {
                    if (obj == lv_user_app_t[i])
                    {
                        lv_obj_set_style_bg_color(lv_user_app_t[i], lv_palette_main(LV_PALETTE_BLUE), LV_STATE_DEFAULT);
                    }
                    else
                    {
                        lv_obj_set_style_bg_color(lv_user_app_t[i], lv_color_make(127, 111, 121), LV_STATE_DEFAULT);
                    }
                }
                break;
            default:
                break;
        }
    }
}

/**
  * @brief  情景窗口
  * @param  parent:父类
  * @retval 无
  */
void lv_user_win(lv_obj_t* parent)
{
    int lv_index = 0;
    lv_user_app_t[lv_index] = NULL;
    lv_index++;
    /* 取消滚动 */
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);
    /* 设置背景 */
    lv_obj_t* lv_user_image_bg = lv_img_create(parent);
    lv_img_set_src(lv_user_image_bg, &lv_light_bg);
    lv_obj_center(lv_user_image_bg);
    /* 设置选项卡图片 */
    lv_obj_t* lv_user_t = lv_img_create(parent);
    lv_img_set_src(lv_user_t, &uase_win);
    lv_obj_align(lv_user_t, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_img_recolor_opa(lv_user_t, 255, LV_PART_MAIN);
    lv_obj_set_style_img_recolor(lv_user_t, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    /* 设置选项卡名称 */
    lv_obj_t* lv_user_name = lv_label_create(parent);
    lv_obj_set_style_text_color(lv_user_name, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lv_user_name, &myFont18, LV_STATE_DEFAULT);
    lv_label_set_text(lv_user_name, "情景模式");
    lv_obj_align_to(lv_user_name, lv_user_t, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, 0);
    /* 创建多个对象以及设置样式 */
    lv_user_app_t[lv_index] = lv_obj_create(parent);
    lv_obj_set_size(lv_user_app_t[lv_index], lv_user_image[lv_index].app_witch, lv_user_image[lv_index].app_hietch);
    lv_obj_align(lv_user_app_t[lv_index], LV_ALIGN_LEFT_MID, lv_user_image[lv_index].app_witch /2 + 20,0);
    lv_obj_set_style_bg_color(lv_user_app_t[lv_index], lv_color_make(127, 111, 121), LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(lv_user_app_t[lv_index], 0, LV_STATE_DEFAULT);
    lv_obj_clear_flag(lv_user_app_t[lv_index], LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(lv_user_app_t[lv_index], 0, LV_PART_MAIN);
    lv_obj_add_event_cb(lv_user_app_t[lv_index], lv_usert_control_event_handler, LV_EVENT_ALL, NULL);
    /* 创建多个对象的图片以及设置样式 */
    lv_user_app_img[lv_index] = lv_img_create(lv_user_app_t[lv_index]);
    lv_img_set_src(lv_user_app_img[lv_index], lv_user_image[lv_index].app_image);
    lv_obj_center(lv_user_app_img[lv_index]);
    lv_obj_set_style_img_recolor_opa(lv_user_app_img[lv_index], 255, LV_PART_MAIN);
    lv_obj_set_style_img_recolor(lv_user_app_img[lv_index], lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    /* 创建多个对象的名称以及设置样式 */
    lv_user_app_name[lv_index] = lv_label_create(lv_user_app_t[lv_index]);
    lv_obj_set_style_text_color(lv_user_app_name[lv_index], lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lv_user_app_name[lv_index], &myFont18, LV_STATE_DEFAULT);
    lv_label_set_text(lv_user_app_name[lv_index], lv_user_image[lv_index].app_text_Chinese);
    lv_obj_align_to(lv_user_app_name[lv_index], lv_user_app_img[lv_index], LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_index++;
    /* 对多个对象进行有序的排列 */
    for (lv_index = 2; lv_index < lv_user_info_mun; lv_index++)
    {
        lv_user_app_t[lv_index] = lv_obj_create(parent);
        lv_obj_set_size(lv_user_app_t[lv_index], lv_user_image[lv_index].app_witch, lv_user_image[lv_index].app_hietch);
        lv_obj_set_style_bg_color(lv_user_app_t[lv_index], lv_color_make(127, 111, 121), LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(lv_user_app_t[lv_index], 0, LV_STATE_DEFAULT);
        lv_obj_clear_flag(lv_user_app_t[lv_index], LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(lv_user_app_t[lv_index], 0, LV_PART_MAIN);
        lv_obj_update_layout(lv_user_app_t[lv_index]);
        lv_obj_add_event_cb(lv_user_app_t[lv_index], lv_usert_control_event_handler, LV_EVENT_ALL, NULL);

        if (lv_index < 4)       /* 如果一行放三个对象 */
        {
            lv_obj_align_to(lv_user_app_t[lv_index], lv_user_app_t[lv_index - 1], LV_ALIGN_OUT_RIGHT_MID, 10, 0);
        }
        else if (lv_index == 4)/* 第四个对象要换行 */
        {
            lv_obj_align_to(lv_user_app_t[lv_index], lv_user_app_t[1], LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
        }
        else                   /* 第二行对象排列 */
        {
            lv_obj_align_to(lv_user_app_t[lv_index], lv_user_app_t[lv_index - 1], LV_ALIGN_OUT_RIGHT_MID, 10, 0);
        }

        lv_user_app_img[lv_index] = lv_img_create(lv_user_app_t[lv_index]);
        lv_img_set_src(lv_user_app_img[lv_index], lv_user_image[lv_index].app_image);
        lv_obj_center(lv_user_app_img[lv_index]);
        lv_obj_set_style_img_recolor_opa(lv_user_app_img[lv_index], 255, LV_PART_MAIN);
        lv_obj_set_style_img_recolor(lv_user_app_img[lv_index], lv_color_make(255, 255, 255), LV_STATE_DEFAULT);

        lv_user_app_name[lv_index] = lv_label_create(lv_user_app_t[lv_index]);
        lv_obj_set_style_text_color(lv_user_app_name[lv_index], lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(lv_user_app_name[lv_index], &myFont18, LV_STATE_DEFAULT);
        lv_label_set_text(lv_user_app_name[lv_index], lv_user_image[lv_index].app_text_Chinese);
        lv_obj_align_to(lv_user_app_name[lv_index], lv_user_app_img[lv_index], LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    }
}
