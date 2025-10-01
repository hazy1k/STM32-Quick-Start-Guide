/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL综合 实验
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
#include "./FATFS/exfuns/exfuns.h"
#include "./MALLOC/malloc.h"
#include "./BSP/LCD/lcd.h"
#include "./SYSTEM/usart/usart.h"
#include "LVGL/GUI_APP/lv_file.h"
#include "LVGL/GUI_APP/lv_setting.h"
#include "LVGL/GUI_APP/lv_calculator.h"
#include "LVGL/GUI_APP/lv_meter.h"
#include "LVGL/GUI_APP/lv_app.h"
#include "LVGL/GUI_APP/lv_imc.h"
#include "LVGL/GUI_APP/lv_draw.h"
#include "LVGL/GUI_APP/lv_shelf.h"
#include "LVGL/GUI_APP/lv_scale.h"
#include "LVGL/GUI_APP/lv_qr.h"
#include "lvgl.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"


LV_IMG_DECLARE(Setting)
LV_IMG_DECLARE(File)
LV_IMG_DECLARE(lv_radar)
LV_IMG_DECLARE(Phone)
LV_IMG_DECLARE(Test)
LV_IMG_DECLARE(Timer)
LV_IMG_DECLARE(Calculator)
LV_IMG_DECLARE(lv_draw)
LV_IMG_DECLARE(Use_)
LV_IMG_DECLARE(Photograph)
LV_IMG_DECLARE(binary)
LV_IMG_DECLARE(lv_qrcode)


lv_m_general lv_general_dev;

typedef struct
{
    char* app_text_English;
    char* app_text_Chinese;
    const void* app_image;
    uint16_t app_witch;
    uint16_t app_hietch;
}app_image_info;


static const app_image_info app_image[] =
{
    {" "," ",NULL},
    {"Setting","设置",&Setting,146,140},
    {"Browser","进制转换",&binary,146,140},
    {"File","文件管理器",&File,146,140},
    {"Data_usage","测试情况",&Test,146,140},
    {"China_Mobile","中国移动",&Phone,146,140},
    {"draw","绘画",&lv_draw,146,140},
    {"Timer","时钟",&Timer,304,140},
    {"Calculator","计算器",&Calculator,304,140},
    {"qrcode","二维码",&lv_qrcode,146,140},
//    {"Photograph","拍照",&Photograph,304,140},
};

/* 获取路径的个数 */
#define image_mun (int)(sizeof(app_image)/sizeof(app_image[0]))
/* 设置一个app数组 */
lv_obj_t *lv_app_t[image_mun];
/* 设置一个app名字数组 */
lv_obj_t *lv_app_name[image_mun];
/* 设置一个app图片数组 */
lv_obj_t* lv_app_img[image_mun];
/* app就绪表 */
unsigned int  app_readly_list[32];
/* app触发位 */
int lv_trigger_bit = 0;

/**
  * @brief  返回按键
  * @param  无
  * @retval 无
  */
void lv_general_win_create(void)
{
    #define TOP_OFFSET  -10
    lv_obj_t* back_btn = lv_label_create(lv_general_dev.parent);
    lv_label_set_text(back_btn, BACK_BTN_TITLE);
    lv_obj_add_flag(back_btn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align_to(back_btn, NULL, LV_ALIGN_TOP_LEFT, 15, TOP_OFFSET);
    lv_obj_set_style_text_color(back_btn, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_add_event_cb(back_btn, lv_general_dev.lv_back_event, LV_EVENT_ALL, NULL);
}

/**
  * @brief  计算前导置零
  * @param  无
  * @retval 无
  */
int lv_clz(unsigned int  app_readly_list[])
{
    int bit = 0;

    for (int i = 0; i < 32; i++)
    {
        if (app_readly_list[i] == 1)
        {
            break;
        }

        bit ++ ;
    }

    return bit;
}

/**
  * @brief  APP按键回调函数
  * @param  obj  :对象
  * @param  event:事件
  * @retval 无
  */
static void lv_imgbtn_control_event_handler(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t * obj = lv_event_get_target(event);
    lv_obj_t *lv_app_parent = lv_obj_get_parent(obj);

    if (code == LV_EVENT_CLICKED)
    {
        for (int i = 0;i < image_mun;i ++)
        {
            if (obj == lv_app_t[i])
            {
                app_readly_list[i] = 1 ;                                       /* app就绪表位置1 */
            }
        }

        lv_trigger_bit = ((unsigned int)lv_clz((app_readly_list)));            /* 计算前导指令 */
        app_readly_list[lv_trigger_bit] = 0;                                   /* 该位清零就绪表 */
        lv_obj_del(lv_app_parent);                                             /* 界面切换使用删除方法 */
        lv_app_parent = NULL;                                                  /* 主界面的容器设置为空 */
        
        switch(lv_trigger_bit)                                                 /* 根据该位做相应的函数 */
        {
            case 1:
              lv_setting_demo();                                               /* 设置管理系统(完成) */
              break;
            case 2:
              lv_scale_demo();                                                 /* 进制转换系统(完成) */
              break;
            case 3:
              lv_file_demo();                                                  /* 文件管理系统(完成) */
              break;
            case 4:
              lv_shelf_demo();                                                 /* 板载测试系统(完成) */
              break;
            case 5:
              lv_imc_demo();                                                   /* 电话(未完成) */
              break;
            case 6:
              lv_draw_demo();                                                  /* 绘画(完成) */
              break;
            case 7:
              lv_meter_demo();                                                 /* 时钟(完成) */
              break;
            case 8:
              lv_calculator_demo();                                            /* 计算器(完成) */
              break;
            case 9:
              lv_qr_windowm();                                                 /* 二维码制作(完成) */
              break;
            default:
              break;
        }
    }
}

/**
  * @brief  APP显示
  * @param  parent:父类对象
  * @retval 无
  */
void lv_mid_cont_add_app(lv_obj_t *parent)
{
    int line_feed_num = 0;
    int lv_index = 0;
    lv_app_t[lv_index] = NULL;
    lv_index ++;
    int i = 0;
    int n = 1;

    lv_app_t[lv_index] = lv_obj_create(parent);
    lv_obj_set_pos(lv_app_t[lv_index],-7,20);
    lv_obj_set_size(lv_app_t[lv_index], app_image[lv_index].app_witch, app_image[lv_index].app_hietch);
    lv_obj_set_style_bg_color(lv_app_t[lv_index], lv_color_make(26, 57, 137), LV_STATE_DEFAULT);
    lv_obj_clear_flag(lv_app_t[lv_index], LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(lv_app_t[lv_index],0,LV_PART_MAIN);
    lv_obj_add_event_cb(lv_app_t[lv_index], lv_imgbtn_control_event_handler, LV_EVENT_ALL, NULL);

    lv_app_name[lv_index] = lv_label_create(lv_app_t[lv_index]);
    lv_obj_set_style_text_color(lv_app_name[lv_index],lv_color_make(255,255,255), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lv_app_name[lv_index],&myFont14, LV_STATE_DEFAULT);
    lv_label_set_text(lv_app_name[lv_index], app_image[lv_index].app_text_Chinese);
    lv_obj_align(lv_app_name[lv_index],LV_ALIGN_BOTTOM_LEFT,-10, 10);

    lv_app_img[lv_index] = lv_img_create(lv_app_t[lv_index]);
    lv_img_set_src(lv_app_img[lv_index], app_image[lv_index].app_image);
    lv_obj_center(lv_app_img[lv_index]);
    lv_obj_set_style_img_recolor_opa(lv_app_img[lv_index], 255, LV_PART_MAIN);
    lv_obj_set_style_img_recolor(lv_app_img[lv_index], lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    unsigned int lv_width_x = lv_obj_get_width(lv_app_t[1]) + 20;
    lv_index ++;
    

    for (lv_index = 2 ; lv_index < image_mun ; lv_index ++)
    {
        lv_app_t[lv_index] = lv_obj_create(parent);
        lv_obj_set_size(lv_app_t[lv_index], app_image[lv_index].app_witch, app_image[lv_index].app_hietch);
        lv_obj_set_style_bg_color(lv_app_t[lv_index], lv_color_make(26, 57, 137), LV_STATE_DEFAULT);
        lv_obj_clear_flag(lv_app_t[lv_index], LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(lv_app_t[lv_index], 0, LV_PART_MAIN);
        lv_obj_update_layout(lv_app_t[lv_index]);
        lv_width_x = lv_width_x + lv_obj_get_width(lv_app_t[lv_index]) + 10;

        if (lv_width_x < lv_obj_get_width(lv_scr_act()))
        {
            lv_obj_align_to(lv_app_t[lv_index], lv_app_t[lv_index - 1], LV_ALIGN_OUT_RIGHT_MID, 10, 0);
        }
        else
        {
            line_feed_num++;

            if (line_feed_num >= 2)
            {
                i = 10 * n;
                n ++;
            }
            else
            {
                i = 0;
            }
            
            lv_obj_set_pos(lv_app_t[lv_index], -7, (lv_obj_get_height(lv_app_t[lv_index]) + 20) * line_feed_num + 10 - i );
            lv_width_x = lv_obj_get_width(lv_app_t[lv_index]) + 10;
        }
        
        lv_app_name[lv_index] = lv_label_create(lv_app_t[lv_index]);
        lv_obj_set_style_text_color(lv_app_name[lv_index], lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(lv_app_name[lv_index], &myFont14, LV_STATE_DEFAULT);
        lv_label_set_text(lv_app_name[lv_index], app_image[lv_index].app_text_Chinese);
        lv_obj_align(lv_app_name[lv_index], LV_ALIGN_BOTTOM_LEFT, -10, 10);

        lv_app_img[lv_index] = lv_img_create(lv_app_t[lv_index]);
        lv_img_set_src(lv_app_img[lv_index], app_image[lv_index].app_image);
        lv_obj_center(lv_app_img[lv_index]);
        lv_obj_set_style_img_recolor_opa(lv_app_img[lv_index], 255, LV_PART_MAIN);
        lv_obj_set_style_img_recolor(lv_app_img[lv_index], lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
        lv_obj_add_event_cb(lv_app_t[lv_index], lv_imgbtn_control_event_handler, LV_EVENT_ALL, NULL);
    }
}

void lv_app_icon(lv_obj_t *praten)
{
    /* 左上角图标 */
    lv_obj_t* lv_letf_acon = lv_label_create(praten);
    lv_label_set_text(lv_letf_acon, LV_SYMBOL_WIFI " " LV_SYMBOL_AUDIO);
    lv_obj_align(lv_letf_acon,LV_ALIGN_TOP_LEFT,-5,-10);
    lv_obj_set_style_text_color(lv_letf_acon, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    /* 中间的时间 */
    lv_obj_t* lv_timer = lv_label_create(praten);
    lv_label_set_text(lv_timer, "2022/1/20");
    lv_obj_align(lv_timer, LV_ALIGN_TOP_MID, 0, -10);
    lv_obj_set_style_text_color(lv_timer, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    /* 右上角图标 */
    lv_obj_t* lv_right_acon = lv_label_create(praten);
    lv_label_set_text(lv_right_acon, LV_SYMBOL_BATTERY_3 " " LV_SYMBOL_USB);
    lv_obj_align(lv_right_acon, LV_ALIGN_TOP_RIGHT, 5, -10);
    lv_obj_set_style_text_color(lv_right_acon, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);

}

/**
  * @brief  主界面
  * @param  无
  * @retval 无
  */
void lv_main_window(void)
{
    lv_obj_t *lv_main_cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(lv_main_cont, lv_obj_get_width(lv_scr_act()), lv_obj_get_height(lv_scr_act()));
    lv_obj_set_style_bg_color(lv_main_cont, lv_color_make(1, 27, 54), LV_STATE_DEFAULT);
    lv_obj_set_style_radius(lv_main_cont, 0, LV_PART_MAIN);
    lv_mid_cont_add_app(lv_main_cont);
    lv_app_icon(lv_main_cont);
}

/**
  * @brief  LVGL 入口
  * @param  无
  * @retval 无
  */
void lv_mainstart(void)
{
    lv_main_window();       /* 进入页面 */
}
