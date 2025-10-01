/**
 ****************************************************************************************************
 * @file        lv_mainstart.h
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
 
#include "LVGL/GUI_APP/lv_imc.h"
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "./BSP/LCD/lcd.h"


LV_IMG_DECLARE(lv_24g)
LV_IMG_DECLARE(lv_phone)
LV_IMG_DECLARE(lv_prev)
LV_IMG_DECLARE(lv_ren)
LV_IMG_DECLARE(lv_set)

lv_obj_t* lv_phone_cont;


typedef struct
{
    int phone_index;
    char* phone_text;
}phone_info;


static const phone_info lv_phone_info[] =
{
    {1,"1"},
    {2,"2"},
    {3,"3"},
    {4,"4"},
    {5,"5"},
    {6,"6"},
    {7,"7"},
    {8,"8"},
    {9,"9"},
    {10,"*"},
    {11,"0"},
    {12,"#"},
};


lv_obj_t* btn[8];
lv_obj_t* phone_label;
lv_obj_t* phone_1;
lv_obj_t* lv_prev1;

static const char* phone_map_num[] = {
                                    "1", "2", "3","\n",
                                    "4", "4", "5","\n",
                                    "7", "8", "9","\n",
                                    "*", "0", "#", "" };


/**
  * @brief  返回按键回调函数
  * @param  event:事件
  * @retval 无
  */
void lv_phone_back_btn_event_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* obj = lv_event_get_target(event);

    if (code == LV_EVENT_PRESSED)
    {
        lv_label_set_text(obj, "#444444 "BACK_BTN_TITLE"#");
        lv_label_set_recolor(obj, true);
    }
    else if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST)
    {
        lv_obj_del(lv_phone_cont);
        lv_disp_set_rotation(lv_disp_get_default(), LV_DISP_ROT_NONE);
        lv_main_window();/* 打开登录窗体 */
    }
}

/**
  * @brief  回调函数
  * @param  obj  :对象
  * @param  event:事件
  * @retval 无
  */
static void lv_prev_event_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* obj = lv_event_get_target(event);

    if (code == LV_EVENT_CLICKED)
    {
        if (obj == lv_prev1)
        {
            lv_textarea_del_char(phone_1);
        }
    }
}

/**
  * @brief  回调函数
  * @param  obj  :对象
  * @param  event:事件
  * @retval 无
  */
static void lv_phone_event_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* obj = lv_event_get_target(event);

    if (code == LV_EVENT_CLICKED)
    {
        for (int i = 0 ; i < 12 ; i ++)
        {
            if (obj == btn[i])
            {
                lv_textarea_add_text(phone_1, lv_phone_info[i].phone_text);
            }
        }
    }
}

void lv_phone_window()
{
    lv_obj_t* lv_phone_class = lv_obj_create(lv_scr_act());
}
/**
  * @brief  回调函数
  * @param  obj  :对象
  * @param  event:事件
  * @retval 无
  */
static void lv_phone1_event_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* obj = lv_event_get_target(event);

    if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST)
    {
        lv_obj_del(lv_phone_cont);
        lv_phone_window();/* 打开登录窗体 */
    }
}

void lv_btn_set(lv_obj_t *parent)
{
    uint32_t i;

    for (i = 0; i < 12; i++)
    {
        uint8_t col = i % 3;
        uint8_t row = i / 3;

        btn[i] = lv_btn_create(parent);
        lv_obj_set_style_radius(btn[i], 70, LV_PART_MAIN);
        lv_obj_set_style_bg_color(btn[i], lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(btn[i], lv_color_make(0, 0, 0), LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(btn[i], lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(btn[i], 0, LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(btn[i], &lv_font_montserrat_32, LV_STATE_DEFAULT);
        lv_obj_set_grid_cell(btn[i], LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);
        phone_label = lv_label_create(btn[i]);
        lv_label_set_text_fmt(phone_label, "%s", lv_phone_info[i].phone_text);
        lv_obj_center(phone_label);

        lv_obj_add_event_cb(btn[i], lv_phone_event_handler, LV_EVENT_ALL, NULL);
    }
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_imc_demo(void)
{
    lv_disp_set_rotation(lv_disp_get_default(), LV_DISP_ROT_90);
    
    static lv_coord_t col_dsc[] = { 100, 100, 100,100, LV_GRID_TEMPLATE_LAST };
    static lv_coord_t row_dsc[] = { 100, 100, 100,100, LV_GRID_TEMPLATE_LAST };

    lv_general_dev.lv_general_win_create = NULL;
    lv_general_dev.lv_back_event = NULL;
    lv_general_dev.parent = NULL;

    lv_phone_cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(lv_phone_cont,lv_obj_get_width(lv_scr_act()),lv_obj_get_height(lv_scr_act()));
    lv_obj_set_style_bg_color(lv_phone_cont, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_set_style_radius(lv_phone_cont, 0, LV_PART_MAIN);
    lv_obj_clear_flag(lv_phone_cont, LV_OBJ_FLAG_SCROLLABLE);

    lv_general_dev.lv_back_event = lv_phone_back_btn_event_handler;
    lv_general_dev.parent = lv_phone_cont;
    lv_general_dev.lv_general_win_create = lv_general_win_create;
    lv_general_dev.lv_general_win_create();

    lv_obj_t* lv_phone_btn = lv_obj_create(lv_phone_cont);
    lv_obj_set_size(lv_phone_btn,lv_obj_get_width(lv_phone_cont)/2 + 120,lv_obj_get_height(lv_phone_cont)/2 + 150);
    lv_obj_set_pos(lv_phone_btn,40, lv_obj_get_height(lv_phone_cont) / 2 - 150);
    lv_obj_set_style_border_color(lv_phone_btn, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(lv_phone_btn, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_set_style_grid_column_dsc_array(lv_phone_btn, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(lv_phone_btn, row_dsc, 0);
    lv_obj_set_layout(lv_phone_btn, LV_LAYOUT_GRID);
    
    /* 创建输入框 */
    phone_1 = lv_textarea_create(lv_phone_cont);
    lv_textarea_set_text(phone_1, "");
    lv_textarea_set_one_line(phone_1, true);           /* 将文本区域配置为一行或恢复正常 */
    lv_textarea_set_cursor_click_pos(phone_1, true);   /* 隐藏光标 */
    lv_obj_set_size(phone_1, lv_obj_get_width(lv_scr_act()) - 20, lv_font_montserrat_24.line_height + 20);
    lv_obj_clear_flag(phone_1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align_to(phone_1, lv_phone_btn, LV_ALIGN_OUT_TOP_MID, 0, -70);
    lv_obj_set_style_text_font(phone_1, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_align(phone_1,LV_TEXT_ALIGN_CENTER, LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(phone_1, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(phone_1, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_clear_flag(phone_1, LV_OBJ_FLAG_SCROLLABLE);

    lv_btn_set(lv_phone_btn);

    lv_obj_t* lv_24gt = lv_imgbtn_create(lv_phone_cont);
    lv_imgbtn_set_src(lv_24gt, LV_IMGBTN_STATE_RELEASED, NULL, &lv_24g, NULL);
    lv_obj_set_size(lv_24gt, lv_24g.header.w, lv_24g.header.h);
    lv_obj_align(lv_24gt,LV_ALIGN_BOTTOM_LEFT, 80, 0);

    lv_obj_t* lv_phone1 = lv_imgbtn_create(lv_phone_cont);
    lv_imgbtn_set_src(lv_phone1, LV_IMGBTN_STATE_RELEASED, NULL, &lv_phone, NULL);
    lv_obj_set_size(lv_phone1, lv_phone.header.w, lv_phone.header.h);
    lv_obj_set_style_img_recolor_opa(lv_phone1, 255, LV_PART_MAIN);
    lv_obj_set_style_img_recolor(lv_phone1, lv_palette_main(LV_PALETTE_GREEN), LV_STATE_DEFAULT);
    lv_obj_align_to(lv_phone1, lv_24gt,LV_ALIGN_OUT_RIGHT_MID, 50, 0);
    lv_obj_add_event_cb(lv_phone1, lv_phone1_event_handler, LV_EVENT_ALL, NULL);

    lv_obj_t* lv_setting = lv_imgbtn_create(lv_phone_cont);
    lv_imgbtn_set_src(lv_setting, LV_IMGBTN_STATE_RELEASED, NULL, &lv_set, NULL);
    lv_obj_set_size(lv_setting, lv_set.header.w, lv_set.header.h);
    lv_obj_align_to(lv_setting, lv_phone1, LV_ALIGN_OUT_RIGHT_MID, 50, 0);
    
    lv_obj_t* phone_flage = lv_img_create(lv_phone_cont);
    lv_img_set_src(phone_flage,&lv_ren);
    lv_img_set_zoom(phone_flage,400);
    lv_obj_align(phone_flage,LV_ALIGN_TOP_MID,0,40);

    lv_prev1 = lv_imgbtn_create(lv_phone_cont);
    lv_imgbtn_set_src(lv_prev1, LV_IMGBTN_STATE_RELEASED,NULL,&lv_prev,NULL);
    lv_obj_set_size(lv_prev1, lv_prev.header.w, lv_prev.header.h);
    lv_obj_align_to(lv_prev1, lv_phone_btn, LV_ALIGN_OUT_TOP_MID, 100, 0);
    lv_obj_add_event_cb(lv_prev1, lv_prev_event_handler, LV_EVENT_ALL, NULL);
}
