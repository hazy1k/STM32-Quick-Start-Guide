/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL GIF读取实验
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


typedef struct
{
    char *img_path;
    char *label_text;
}img_info_t;

const img_info_t GIF_PATH[] =
{
    {"0:/PICTURE/GIF/alientek.gif", "alientek.gif"},
};

#define image_mun (int)(sizeof(GIF_PATH)/sizeof(GIF_PATH[0]))
int image = 0;
lv_obj_t *img;
extern int has_next;

void lv_my_timer(lv_timer_t *timer)
{ 
    if (has_next == 0)          /* 如果是最后一帧，那么切换GIF */
    {
        image++;
        lv_obj_del(img);        /* 删除前面的GIF */
      
        if (image >= image_mun) /* 判断GIF库包含的个数是否最大 */
        {
            image = 0;          /* 重新开始展示 */
            img = lv_gif_create(lv_scr_act());
            lv_gif_set_src(img, GIF_PATH[image].img_path);
        }
        else                    /* 如果不是最后的GIF */
        {
            img = lv_gif_create(lv_scr_act());
            lv_gif_set_src(img, GIF_PATH[image].img_path);
        }

        timer->user_data = img; /* 设置任务数据等于获取的图片数据 */

        lv_obj_align_to(img,NULL,LV_ALIGN_CENTER,0,0);
    }
}

void lv_mainstart(void)
{
    lv_obj_t *label;
    label = lv_label_create(lv_scr_act());
    lv_label_set_text(label,"GIF_Decoder");
    lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_RED),LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label,&lv_font_montserrat_32,LV_STATE_DEFAULT);
    lv_obj_align_to(label,NULL,LV_ALIGN_TOP_MID,0,0);
    lv_obj_set_style_bg_color(lv_scr_act(),lv_palette_main(LV_PALETTE_BLUE),LV_STATE_DEFAULT);
  
    lv_obj_set_style_bg_color(lv_scr_act(),lv_palette_main(LV_PALETTE_BLUE),LV_STATE_DEFAULT);
    img = lv_gif_create(lv_scr_act());
    lv_gif_set_src(img, GIF_PATH[image].img_path);
    lv_obj_align_to(img,NULL,LV_ALIGN_CENTER,0,0);
    
    lv_timer_create(lv_my_timer,10,img);
}

