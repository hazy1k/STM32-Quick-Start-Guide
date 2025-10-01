/**
 ****************************************************************************************************
 * @file        lv_app.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
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
 
#include "LVGL/GUI_APP/lv_app.h"
#include "./FATFS/exfuns/exfuns.h"
#include "./MALLOC/malloc.h"
#include "./BSP/LCD/lcd.h"
#include "./SYSTEM/usart/usart.h"
#include "lvgl.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"


LV_IMG_DECLARE(Calculator1)
LV_IMG_DECLARE(Clock)
LV_IMG_DECLARE(File1)
LV_IMG_DECLARE(Qr_t)
LV_IMG_DECLARE(Radar)
LV_IMG_DECLARE(scale)
LV_IMG_DECLARE(write)
LV_IMG_DECLARE(text)
LV_IMG_DECLARE(Input_method)
LV_IMG_DECLARE(Line)
LV_IMG_DECLARE(blue)
LV_IMG_DECLARE(message)
LV_IMG_DECLARE(phone)
LV_IMG_DECLARE(video)
LV_IMG_DECLARE(WIFI)
LV_IMG_DECLARE(camera)
LV_IMG_DECLARE(record)
LV_IMG_DECLARE(SETTING)

typedef struct
{
    char* app_text_English;
    char* app_text_Chinese;
    const void* app_image;
}app_image_info;


static const app_image_info app_image[] =
{
    {" "," ",NULL},
    {"Calculator1","计算器",&Calculator1},
    {"Clock","时钟",&Clock},
    {"File","文件",&File1},
    {"Qr_t","二维码",&Qr_t},
    {"Radar","旋转",&Radar},
    {"scale","进制转换",&scale},
    {"write","手写",&write},
    {"test","板载测试",&text},
    {"IME","输入法",&Input_method},
    {"record","录音",&record},
};

/* 获取路径的个数 */
#define image_mun (int)(sizeof(app_image)/sizeof(app_image[0]))
/* 设置一个app图像数组 */
lv_obj_t *lv_app_user_t[image_mun];
/* 设置一个app名字数组 */
lv_obj_t *lv_app_user_name[image_mun];
/* app就绪表 */
unsigned int  app_readly_user_list;
/* app触发位 */
int lv_trigger_user_bit = 0;



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
            if (obj == lv_app_user_t[i])
            {
                app_readly_user_list |= 1 << i; /* app就绪表位置1 */
            }
        }
        
        lv_trigger_user_bit = (31UL -  ( unsigned char ) __clz( ( app_readly_user_list ) ) );/* 计算前导指令 */
        app_readly_user_list &= ~(1 << lv_trigger_user_bit);                                 /* 该位清零就绪表 */
        lv_obj_del(lv_app_parent);                                                 /* 界面切换使用删除方法 */
        lv_app_parent = NULL;                                                      /* 主界面的容器设置为空 */

        switch(lv_trigger_user_bit)                   /* 根据该位做相应的函数 */
        {
            case 1:
//              lv_Calculator1_demo();              /* 计算器 */
              break;
            case 2:
//              lv_meter_demo();                   /* 时钟 */
              break;
            case 3:
//              lv_file_demo();                    /* 文件系统 */
              break;
            case 4:
//              lv_qr_demo();                      /* 二维码 */
              break;
            case 5:
//              lv_radar_demo();                   /* 旋转器 */
              break;
            case 6:
//              lv_scale_demo();                   /* 进制转换器 */
              break;
            case 7:
//              lv_write_demo();                   /* 手写 */
              break;
            case 8:
//              lv_shelf_demo();                   /* 板载测试 */
              break;
            case 9:
//              lv_imc_demo();                     /* 中文输入法 */
              break;
            case 10:
//              lv_line_demo();                    /* 线 */
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
void lv_mid_user_add_app(lv_obj_t *parent)
{
    unsigned int lv_width_x = Calculator1.header.w +10;
    int app_index = 1;
    int line_feed_num = 0;
    int i = 0;
    static lv_style_t style_pr;
    lv_style_init(&style_pr);
    lv_style_set_img_recolor_opa(&style_pr, LV_OPA_30);
    lv_style_set_img_recolor(&style_pr, lv_color_black());
    lv_app_user_t[0] = NULL;
  
    lv_app_user_t[app_index] = lv_imgbtn_create(parent);
    lv_imgbtn_set_src(lv_app_user_t[app_index], LV_IMGBTN_STATE_RELEASED, NULL,app_image[app_index].app_image,NULL);
    lv_imgbtn_set_src(lv_app_user_t[app_index], LV_IMGBTN_STATE_PRESSED, NULL,app_image[app_index].app_image,NULL);
    lv_imgbtn_set_src(lv_app_user_t[app_index], LV_IMGBTN_STATE_CHECKED_RELEASED, NULL,app_image[app_index].app_image,NULL);
    lv_imgbtn_set_src(lv_app_user_t[app_index], LV_IMGBTN_STATE_CHECKED_PRESSED, NULL,app_image[app_index].app_image,NULL);
    lv_obj_set_size(lv_app_user_t[app_index], Calculator1.header.w, Calculator1.header.h);
    lv_obj_align_to(lv_app_user_t[app_index], NULL, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_add_event_cb(lv_app_user_t[app_index], lv_imgbtn_control_event_handler,LV_EVENT_ALL,NULL);
    lv_obj_add_style(lv_app_user_t[app_index], &style_pr, LV_STATE_PRESSED);
    
    lv_app_user_name[app_index] = lv_label_create(parent);
    lv_obj_set_style_text_font(lv_app_user_name[app_index],&myFont24,LV_STATE_DEFAULT);
    lv_label_set_text(lv_app_user_name[app_index],app_image[app_index].app_text_Chinese);
    lv_obj_align_to(lv_app_user_name[app_index], lv_app_user_t[app_index], LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
  
    for (app_index = 2;app_index < image_mun;app_index ++)
    {   
        lv_app_user_t[app_index] = lv_imgbtn_create(parent);
        lv_imgbtn_set_src(lv_app_user_t[app_index], LV_IMGBTN_STATE_RELEASED, NULL,app_image[app_index].app_image,NULL);
        lv_imgbtn_set_src(lv_app_user_t[app_index], LV_IMGBTN_STATE_PRESSED, NULL,app_image[app_index].app_image,NULL);
        lv_imgbtn_set_src(lv_app_user_t[app_index], LV_IMGBTN_STATE_CHECKED_RELEASED, NULL,app_image[app_index].app_image,NULL);
        lv_imgbtn_set_src(lv_app_user_t[app_index], LV_IMGBTN_STATE_CHECKED_PRESSED, NULL,app_image[app_index].app_image,NULL);
        lv_obj_set_size(lv_app_user_t[app_index], Calculator1.header.w, Calculator1.header.h);
        lv_width_x = lv_width_x + Calculator1.header.w + 10;
        
        if (lv_width_x <= lv_obj_get_width(parent) + 10)
        {
            lv_obj_align_to(lv_app_user_t[app_index], lv_app_user_t[app_index - 1], LV_ALIGN_OUT_RIGHT_MID, 10, 0);
        }
        else
        {
            line_feed_num ++;
            lv_obj_set_pos(lv_app_user_t[app_index], 10, (Calculator1.header.w + myFont24.line_height) * line_feed_num + 30 + i * 20);
            i ++;
            lv_width_x = Calculator1.header.w;
        }

        lv_app_user_name[app_index] = lv_label_create(parent);
        lv_label_set_text(lv_app_user_name[app_index],app_image[app_index].app_text_Chinese);
        lv_obj_set_style_text_font(lv_app_user_name[app_index],&myFont24,LV_STATE_DEFAULT);
        lv_obj_align_to(lv_app_user_name[app_index], lv_app_user_t[app_index], LV_ALIGN_OUT_BOTTOM_MID, 10, 0);
        lv_obj_add_style(lv_app_user_t[app_index], &style_pr, LV_STATE_PRESSED);
        lv_obj_add_event_cb(lv_app_user_t[app_index], lv_imgbtn_control_event_handler,LV_EVENT_ALL,NULL);
    }
}

/**
  * @brief  主界面
  * @param  无
  * @retval 无
  */
void lv_main_user_window(void)
{
    lv_obj_t *lv_main_cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(lv_main_cont, 800, 480);
    lv_obj_set_style_bg_color(lv_main_cont,lv_color_hex(0xB0C4DE),LV_STATE_DEFAULT);
    lv_mid_user_add_app(lv_main_cont);
}

/**
  * @brief  LVGL 入口
  * @param  无
  * @retval 无
  */
void lv_app_demo(void)
{
    lv_main_user_window();       /* 进入页面 */
}
