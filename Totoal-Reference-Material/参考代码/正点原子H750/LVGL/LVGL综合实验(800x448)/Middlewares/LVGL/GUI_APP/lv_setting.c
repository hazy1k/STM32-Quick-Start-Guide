#include "lvgl/lvgl.h"
#include <stdio.h>
#include "LVGL/GUI_APP/lv_setting.h"
#include "LVGL/GUI_APP/lv_mainstart.h"


LV_FONT_DECLARE(myFont14)
LV_FONT_DECLARE(myFont18)
LV_FONT_DECLARE(myFont24)

LV_IMG_DECLARE(more)
LV_IMG_DECLARE(neiwore4G)
LV_IMG_DECLARE(network_port)
LV_IMG_DECLARE(question_mark)
LV_IMG_DECLARE(shutdown)
LV_IMG_DECLARE(spin)
LV_IMG_DECLARE(TVCE)
LV_IMG_DECLARE(zoom)

typedef struct
{
    char* setting_app_text_English;
    char* setting_app_text_Chinese;
    const void* setting_app_image;
    int setting_app_flag;
}setting_app_image_info;


static const setting_app_image_info setting_app_image[] =
{
    {" "," ",NULL,0},
    {"network_port","网口设置",&network_port,1},
    {"spin","网速测试",&spin,2},
    {"zoom","图像缩放",&zoom,3},
    {"4G","LTE",&neiwore4G,4},
    {" ","清理",NULL,5},
    {" question_mark","关于本机",&question_mark,6},
    {"more","更多",&more,7},
    {"TV","DTB-T",&TVCE,8},
};

lv_obj_t* lv_setting_obj;

/* 获取路径的个数 */
#define setting_image_mun (int)(sizeof(setting_app_image)/sizeof(setting_app_image[0]))

lv_obj_t* lv_setting_app_t[setting_image_mun];

/* 设置一个app名字数组 */
lv_obj_t* lv_setting_app_name[setting_image_mun];

/* 设置一个app图片数组 */
lv_obj_t* lv_setting_app_img[setting_image_mun];


unsigned int setting_app_readly_list[32];
int lv_setting_trigger_bit = 0;

static lv_point_t line_points1[] = { {5, 5}, {70, 5}, {120, 5}, {180, 5}, {240, 5}, {300, 5} };
lv_obj_t* clear_arc;

static void set_angle(void* obj, int32_t v)
{
    lv_arc_set_value(obj, v);
    printf("%d\r\n",v);
    if (v >= 100)
    {
        lv_arc_set_value(obj, 0);
        lv_obj_update_layout(obj);
    }
}

void lv_clear_arc(void)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, clear_arc);
    lv_anim_set_exec_cb(&a, set_angle);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_repeat_delay(&a, 500);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_start(&a);
}

/**
  * @brief  APP按键回调函数
  * @param  obj  :对象
  * @param  event:事件
  * @retval 无
  */
static void lv_setting_control_event_handler(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* obj = lv_event_get_target(event);
    lv_obj_t* lv_app_parent = lv_obj_get_parent(obj);

    if (code == LV_EVENT_CLICKED)
    {
        for (int i = 0; i < setting_image_mun; i++)
        {
            if (obj == lv_setting_app_t[i])
            {
                setting_app_readly_list[i] = 1; /* app就绪表位置1 */
            }
        }

        lv_setting_trigger_bit = ((unsigned int)lv_clz((setting_app_readly_list)));     /* 计算前导指令 */
        setting_app_readly_list[lv_setting_trigger_bit] = 0;                            /* 该位清零就绪表 */
        
        switch (lv_setting_trigger_bit)                                                 /* 根据该位做相应的函数 */
        {
            case 1:
            case 2:
            case 3:
            case 4:
            case 6:
            case 7:
            case 8:
                /* 以下有实现的功能，请删除这两个注释 */
                //lv_obj_del(lv_app_parent);                                                      /* 界面切换使用删除方法 */
                //lv_app_parent = NULL;                                                           /* 主界面的容器设置为空 */
                switch (lv_setting_trigger_bit)
                {
                    case 1:
                        /* 未实现 */
                        break;
                    case 2:
                        /* 未实现 */
                        break;
                    case 3:
                        /* 未实现 */
                        break;
                    case 4:
                        /* 未实现 */
                        break;
                    case 6:
                        /* 未实现 */
                        break;
                    case 7:
                        /* 未实现 */
                        break;
                    case 8:
                        /* 未实现 */
                        break;
                }
                break;
            case 5:
                lv_clear_arc();
                break;
            default:
                break;
        }
    }
}

void lv_arc(lv_obj_t* lv_parent)
{
    clear_arc = lv_arc_create(lv_parent);
    lv_arc_set_rotation(clear_arc, 270);
    lv_arc_set_bg_angles(clear_arc, 0, 360);
    lv_obj_remove_style(clear_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(clear_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(clear_arc,100,100);
    lv_arc_set_value(clear_arc, 0);
    lv_obj_center(clear_arc);
    lv_obj_update_layout(clear_arc);
}

/**
  * @brief  返回按键回调函数
  * @param  event:事件
  * @retval 无
  */
void lv_write_back_btn_event_handler(lv_event_t* event)
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
        lv_obj_del(lv_setting_obj);
        lv_mainstart();                    /* 打开登录窗体 */
    }
}

/**
  * @brief  实例
  * @param  无
  * @retval 无
  */
void lv_setting_demo(void)
{
    int line_feed_num = 0;
    int lv_setting_index = 0;
    lv_setting_app_t[lv_setting_index] = NULL;
    int i = 0;

    lv_setting_index ++ ;
    lv_general_dev.lv_general_win_create = NULL;
    lv_general_dev.lv_back_event = NULL;
    lv_general_dev.parent = NULL;

    lv_setting_obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(lv_setting_obj,lv_obj_get_width(lv_scr_act()),lv_obj_get_height(lv_scr_act()));
    lv_obj_set_style_bg_color(lv_setting_obj,lv_color_make(1,27,54), LV_STATE_DEFAULT);
    lv_obj_set_style_radius(lv_setting_obj, 0, LV_PART_MAIN);

    lv_general_dev.lv_back_event = lv_write_back_btn_event_handler;
    lv_general_dev.parent = lv_setting_obj;
    lv_general_dev.lv_general_win_create = lv_general_win_create;
    lv_general_dev.lv_general_win_create();

    lv_obj_t* lv_set_label = lv_label_create(lv_setting_obj);
    lv_obj_set_style_text_font(lv_set_label,&myFont24, LV_STATE_DEFAULT);
    lv_obj_align(lv_set_label,LV_ALIGN_TOP_MID,0,10);
    lv_label_set_text(lv_set_label,"设置");
    lv_obj_set_style_text_color(lv_set_label, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);

    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 3);
    lv_style_set_line_color(&style_line, lv_color_make(255, 255, 255));
    lv_style_set_line_rounded(&style_line, true);

    lv_obj_t* line1 = lv_line_create(lv_setting_obj);
    lv_line_set_points(line1, line_points1, 6); 
    lv_obj_add_style(line1, &style_line, 0);
    lv_obj_align_to(line1, lv_set_label, LV_ALIGN_RIGHT_MID, 320, 0);

    lv_obj_t* line2 = lv_line_create(lv_setting_obj);
    lv_line_set_points(line2, line_points1, 6); 
    lv_obj_add_style(line2, &style_line, 0);
    lv_obj_align_to(line2, lv_set_label, LV_ALIGN_OUT_LEFT_MID, -25, 0);

    lv_setting_app_t[lv_setting_index] = lv_obj_create(lv_setting_obj);
    lv_obj_set_size(lv_setting_app_t[lv_setting_index], 144, 160);
    lv_obj_align_to(lv_setting_app_t[lv_setting_index], NULL, LV_ALIGN_TOP_LEFT, 20, 60);
    lv_obj_add_flag(lv_setting_app_t[lv_setting_index],LV_OBJ_FLAG_CHECKABLE);
    
    lv_obj_update_layout(lv_setting_app_t[lv_setting_index]);
    lv_obj_add_event_cb(lv_setting_app_t[lv_setting_index], lv_setting_control_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_set_style_bg_color(lv_setting_app_t[lv_setting_index], lv_color_make(26, 57, 137), LV_STATE_DEFAULT);

    unsigned int lv_width_x = lv_obj_get_width(lv_setting_app_t[lv_setting_index]) + 20;

    lv_setting_app_name[lv_setting_index] = lv_label_create(lv_setting_app_t[lv_setting_index]);
    lv_obj_set_style_text_font(lv_setting_app_name[lv_setting_index], &myFont14, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lv_setting_app_name[lv_setting_index], lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_label_set_text(lv_setting_app_name[lv_setting_index], setting_app_image[lv_setting_index].setting_app_text_Chinese);
    lv_obj_align_to(lv_setting_app_name[lv_setting_index], lv_setting_app_t[lv_setting_index], LV_ALIGN_TOP_LEFT, -10, -10);


    lv_setting_app_img[lv_setting_index] = lv_img_create(lv_setting_app_t[lv_setting_index]);
    lv_img_set_src(lv_setting_app_img[lv_setting_index], setting_app_image[lv_setting_index].setting_app_image);
    lv_obj_align(lv_setting_app_img[lv_setting_index],LV_ALIGN_CENTER,0,0);
    lv_obj_set_style_img_recolor_opa(lv_setting_app_img[lv_setting_index], 255, LV_PART_MAIN);
    lv_obj_set_style_img_recolor(lv_setting_app_img[lv_setting_index], lv_color_make(255, 255, 255), LV_STATE_DEFAULT);

    lv_setting_index ++;

    for (lv_setting_index ; lv_setting_index < setting_image_mun; lv_setting_index++)
    {
        lv_setting_app_t[lv_setting_index] = lv_obj_create(lv_setting_obj);
        lv_obj_set_size(lv_setting_app_t[lv_setting_index],144,160);
        lv_obj_update_layout(lv_setting_app_t[lv_setting_index]);
        lv_obj_add_flag(lv_setting_app_t[lv_setting_index], LV_OBJ_FLAG_CLICKABLE);
        lv_width_x = lv_width_x + lv_obj_get_width(lv_setting_app_t[lv_setting_index]) + 45;

        if (lv_width_x < lv_obj_get_width(lv_scr_act()) )
        {
            lv_obj_align_to(lv_setting_app_t[lv_setting_index], lv_setting_app_t[lv_setting_index - 1], LV_ALIGN_OUT_RIGHT_MID, 45, 0);
        }
        else
        {
            line_feed_num++;
            lv_obj_set_pos(lv_setting_app_t[lv_setting_index], 20, (lv_obj_get_width(lv_setting_app_t[lv_setting_index]) + myFont24.line_height) * line_feed_num + 60 + i * 20);
            i++;
            lv_width_x = lv_obj_get_width(lv_setting_app_t[lv_setting_index]) + 45;
        }

        lv_obj_set_style_bg_color(lv_setting_app_t[lv_setting_index], lv_color_make(26, 57, 137), LV_STATE_DEFAULT);

        lv_setting_app_name[lv_setting_index] = lv_label_create(lv_setting_app_t[lv_setting_index]);
        lv_label_set_text(lv_setting_app_name[lv_setting_index], setting_app_image[lv_setting_index].setting_app_text_Chinese);
        lv_obj_set_style_text_font(lv_setting_app_name[lv_setting_index], &myFont14, LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(lv_setting_app_name[lv_setting_index], lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
        lv_obj_align_to(lv_setting_app_name[lv_setting_index], lv_setting_app_t[lv_setting_index], LV_ALIGN_TOP_LEFT, -10, -10);

        if (setting_app_image[lv_setting_index].setting_app_flag == 5)
        {
            lv_arc(lv_setting_app_t[lv_setting_index]);
        }

        lv_setting_app_img[lv_setting_index] = lv_img_create(lv_setting_app_t[lv_setting_index]);
        lv_img_set_src(lv_setting_app_img[lv_setting_index], setting_app_image[lv_setting_index].setting_app_image);
        lv_obj_align(lv_setting_app_img[lv_setting_index], LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_style_img_recolor_opa(lv_setting_app_img[lv_setting_index], 255, LV_PART_MAIN);
        lv_obj_set_style_img_recolor(lv_setting_app_img[lv_setting_index], lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
        lv_obj_add_event_cb(lv_setting_app_t[lv_setting_index], lv_setting_control_event_handler, LV_EVENT_ALL, NULL);

    }
}
