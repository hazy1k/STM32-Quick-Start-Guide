/**
 ****************************************************************************************************
 * @file        lv_light_win.c
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
#include "./LVGL/GUI_APP/lv_light_win.h"
#include "LVGL/GUI_APP/lv_mainstart.h"


typedef struct
{
    char* app_text_Chinese;
    const void* app_image;
    uint16_t app_witch;
    uint16_t app_hietch;
}light_win_image_info;

static const light_win_image_info lv_light_image[] =
{
    {"",NULL,NULL,NULL},
    {"客厅灯",&lv_light,150,150},
    {"氛围灯",&lv_light,150,150},
    {"筒灯",&lv_light,150,150},
    {"大厅灯",&lv_light,150,150},
    {"台灯",&lv_light,150,150},
    {"次卧灯",&lv_light,150,150},
    {"客卧灯",&lv_light,150,150},
    {"主卧灯",&lv_light,150,150},
};

char* lv_light_state[2] = {"关","开"};

enum {
    LV_CLOSE_STATE = 0, 
    LV_OPEN_STATE, 
};

#define lv_light_info_mun (int)(sizeof(lv_light_image)/sizeof(lv_light_image[0]))
    
unsigned int  light_app_readly_list[32];

int lv_light_trigger_bit = 0;

int light_app[lv_light_info_mun] = {0,0,0,0,0,0,0,0};


lv_obj_t* lv_light_app_t[lv_light_info_mun];

lv_obj_t* lv_light_app_name[lv_light_info_mun];

lv_obj_t* lv_light_app_img[lv_light_info_mun];

lv_obj_t* lv_light_app_describe[lv_light_info_mun];

lv_obj_t* lv_light_app_led[lv_light_info_mun];


/**
  * @brief  设置obj样式
  * @param  lv_set_style  :要设置样式的对象
  * @retval 返回位数
  */
void lv_ovj_set_style(lv_obj_t * lv_set_style)
{

    static lv_style_t style_pr;
    lv_style_init(&style_pr);


    lv_style_set_outline_width(&style_pr, 30);
    lv_style_set_outline_opa(&style_pr, LV_OPA_TRANSP);

    lv_style_set_shadow_ofs_y(&style_pr, 3);
    lv_style_set_bg_color(&style_pr, lv_palette_darken(LV_PALETTE_BLUE, 2));
    lv_style_set_bg_grad_color(&style_pr, lv_palette_darken(LV_PALETTE_BLUE, 4));

    lv_obj_add_style(lv_set_style, &style_pr, LV_STATE_PRESSED);
}

/**
  * @brief  前导指令函数
  * @param  app_readly_list  :32位的数组
  * @retval 返回位数
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

        bit++;
    }

    return bit;
}


/**
  * @brief  灯光回调函数
  * @param  event  :事件
  * @retval 无
  */
static void lv_light_control_event_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* obj = lv_event_get_target(event);

    if (code == LV_EVENT_CLICKED)
    {
        for (int i = 0; i < lv_light_info_mun; i++)
        {
            if (obj == lv_light_app_t[i])
            {
                light_app_readly_list[i] = 1;
            }
        }
        /* 使用前导置零法判断对象按下的状态 */
        lv_light_trigger_bit = ((unsigned int)lv_clz((light_app_readly_list)));
        light_app[lv_light_trigger_bit] ++;
        light_app_readly_list[lv_light_trigger_bit] = 0;
        switch (lv_light_trigger_bit)
        {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
                if (light_app[lv_light_trigger_bit] == 2)
                {
                    light_app[lv_light_trigger_bit] = 0;
                    lv_obj_set_style_bg_color(lv_light_app_led[lv_light_trigger_bit], lv_palette_main(LV_PALETTE_GREY), LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_make(127, 111, 121), LV_STATE_DEFAULT);
                    lv_label_set_text(lv_light_app_describe[lv_light_trigger_bit], lv_light_state[LV_CLOSE_STATE]);
                    lv_obj_invalidate(lv_light_app_describe[lv_light_trigger_bit]);
                    lv_obj_invalidate(obj);
                }
                else
                {
                    lv_obj_set_style_bg_color(lv_light_app_led[lv_light_trigger_bit], lv_palette_main(LV_PALETTE_RED), LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_palette_main(LV_PALETTE_BLUE), LV_STATE_DEFAULT);
                    lv_label_set_text(lv_light_app_describe[lv_light_trigger_bit], lv_light_state[LV_OPEN_STATE]);
                    lv_obj_invalidate(lv_light_app_describe[lv_light_trigger_bit]);
                    lv_obj_invalidate(obj);
                }
                break;
            default:
                break;
        }
    }
}

/**
  * @brief  灯光UI
  * @param  parent  :父类
  * @retval 无
  */
void lv_light_win(lv_obj_t * parent)
{
    int lv_index = 0;
    lv_light_app_t[lv_index] = NULL;
    lv_index ++;
    /* 设置该容器不可滑动 */
    lv_obj_clear_flag(parent,LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * lv_light_image_bg = lv_img_create(parent);
    lv_img_set_src(lv_light_image_bg, &lv_light_bg);
    lv_obj_center(lv_light_image_bg);

    lv_obj_t * lv_light_t = lv_img_create(parent);
    lv_img_set_src(lv_light_t, &light_contro_win);
    lv_obj_align(lv_light_t,LV_ALIGN_TOP_LEFT,0,0);
    lv_obj_set_style_img_recolor_opa(lv_light_t, 255, LV_PART_MAIN);
    lv_obj_set_style_img_recolor(lv_light_t, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);

    lv_obj_t* lv_light_name = lv_label_create(parent);
    lv_obj_set_style_text_color(lv_light_name, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lv_light_name, &myFont18, LV_STATE_DEFAULT);
    lv_label_set_text(lv_light_name, "灯光控制");
    lv_obj_align_to(lv_light_name, lv_light_t, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, 0);

    lv_light_app_t[lv_index] = lv_obj_create(parent);
    lv_obj_align(lv_light_app_t[lv_index],LV_ALIGN_LEFT_MID, 20,0);
    lv_obj_set_size(lv_light_app_t[lv_index], lv_light_image[lv_index].app_witch, lv_light_image[lv_index].app_hietch);
    lv_obj_set_style_bg_color(lv_light_app_t[lv_index], lv_color_make(127, 111, 121), LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(lv_light_app_t[lv_index], 0, LV_STATE_DEFAULT);
    lv_obj_clear_flag(lv_light_app_t[lv_index], LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(lv_light_app_t[lv_index], 0, LV_PART_MAIN);
    lv_ovj_set_style(lv_light_app_t[lv_index]);
    lv_obj_add_event_cb(lv_light_app_t[lv_index], lv_light_control_event_handler, LV_EVENT_ALL, NULL);

    lv_light_app_img[lv_index] = lv_img_create(lv_light_app_t[lv_index]);
    lv_img_set_src(lv_light_app_img[lv_index], lv_light_image[lv_index].app_image);
    lv_obj_center(lv_light_app_img[lv_index]);
    lv_obj_set_style_img_recolor_opa(lv_light_app_img[lv_index], 255, LV_PART_MAIN);
    lv_obj_set_style_img_recolor(lv_light_app_img[lv_index], lv_color_make(255, 255, 255), LV_STATE_DEFAULT);

    lv_light_app_name[lv_index] = lv_label_create(lv_light_app_t[lv_index]);
    lv_obj_set_style_text_color(lv_light_app_name[lv_index], lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lv_light_app_name[lv_index], &myFont18, LV_STATE_DEFAULT);
    lv_label_set_text(lv_light_app_name[lv_index], lv_light_image[lv_index].app_text_Chinese);
    lv_obj_align_to(lv_light_app_name[lv_index], lv_light_app_img[lv_index], LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_light_app_led[lv_index] = lv_led_create(lv_light_app_t[lv_index]);
    lv_obj_set_style_shadow_width(lv_light_app_led[lv_index], 0, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(lv_light_app_led[lv_index], lv_palette_main(LV_PALETTE_GREY), LV_STATE_DEFAULT);
    lv_obj_align(lv_light_app_led[lv_index], LV_ALIGN_TOP_LEFT, -10, -10);

    lv_light_app_describe[lv_index] = lv_label_create(lv_light_app_t[lv_index]);
    lv_obj_set_style_text_color(lv_light_app_describe[lv_index], lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lv_light_app_describe[lv_index], &myFont14, LV_STATE_DEFAULT);
    lv_label_set_text(lv_light_app_describe[lv_index], lv_light_state[0]);
    lv_obj_align_to(lv_light_app_describe[lv_index], lv_light_app_led[lv_index], LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    lv_index++;

     /* 使用程序把多个obj对象按照一定的顺序排列 */
    for (lv_index = 2; lv_index < lv_light_info_mun; lv_index ++)
    {
        lv_light_app_t[lv_index] = lv_obj_create(parent);
        lv_obj_set_size(lv_light_app_t[lv_index], lv_light_image[lv_index].app_witch, lv_light_image[lv_index].app_hietch);
        lv_obj_set_style_bg_color(lv_light_app_t[lv_index], lv_color_make(127, 111, 121), LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(lv_light_app_t[lv_index], 0, LV_STATE_DEFAULT);
        lv_obj_clear_flag(lv_light_app_t[lv_index], LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(lv_light_app_t[lv_index], 0, LV_PART_MAIN);
        lv_obj_update_layout(lv_light_app_t[lv_index]);
        lv_ovj_set_style(lv_light_app_t[lv_index]);
        lv_obj_add_event_cb(lv_light_app_t[lv_index], lv_light_control_event_handler, LV_EVENT_ALL, NULL);

        if (lv_index < 5)
        {
            lv_obj_align_to(lv_light_app_t[lv_index], lv_light_app_t[lv_index - 1], LV_ALIGN_OUT_RIGHT_MID, 10, 0);
        }
        else if (lv_index == 5)
        {
            lv_obj_align_to(lv_light_app_t[lv_index], lv_light_app_t[1], LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
        }
        else
        {
            lv_obj_align_to(lv_light_app_t[lv_index], lv_light_app_t[lv_index - 1], LV_ALIGN_OUT_RIGHT_MID, 10, 0);
        }

        lv_light_app_img[lv_index] = lv_img_create(lv_light_app_t[lv_index]);
        lv_img_set_src(lv_light_app_img[lv_index], lv_light_image[lv_index].app_image);
        lv_obj_center(lv_light_app_img[lv_index]);
        lv_obj_set_style_img_recolor_opa(lv_light_app_img[lv_index], 255, LV_PART_MAIN);
        lv_obj_set_style_img_recolor(lv_light_app_img[lv_index], lv_color_make(255, 255, 255), LV_STATE_DEFAULT);

        lv_light_app_name[lv_index] = lv_label_create(lv_light_app_t[lv_index]);
        lv_obj_set_style_text_color(lv_light_app_name[lv_index], lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(lv_light_app_name[lv_index], &myFont18, LV_STATE_DEFAULT);
        lv_label_set_text(lv_light_app_name[lv_index], lv_light_image[lv_index].app_text_Chinese);
        lv_obj_align_to(lv_light_app_name[lv_index], lv_light_app_img[lv_index], LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

        lv_light_app_led[lv_index] = lv_led_create(lv_light_app_t[lv_index]);
        lv_obj_set_style_shadow_width(lv_light_app_led[lv_index], 0, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(lv_light_app_led[lv_index], lv_palette_main(LV_PALETTE_GREY), LV_STATE_DEFAULT);
        lv_obj_align(lv_light_app_led[lv_index], LV_ALIGN_TOP_LEFT, -10, -10);

        lv_light_app_describe[lv_index] = lv_label_create(lv_light_app_t[lv_index]);
        lv_obj_set_style_text_color(lv_light_app_describe[lv_index], lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(lv_light_app_describe[lv_index], &myFont14, LV_STATE_DEFAULT);
        lv_label_set_text(lv_light_app_describe[lv_index], lv_light_state[LV_CLOSE_STATE]);
        lv_obj_align_to(lv_light_app_describe[lv_index], lv_light_app_led[lv_index], LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    }

}
