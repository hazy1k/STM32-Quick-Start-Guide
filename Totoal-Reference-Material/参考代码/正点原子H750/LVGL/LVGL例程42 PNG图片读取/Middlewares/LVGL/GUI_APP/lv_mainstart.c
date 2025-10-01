/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL 显示PNG图片实验
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


/* PNG图片结构体 */
typedef struct
{
    char *img_path;    /* 图片路径 */
    char *label_text;  /* 图片名称 */
}img_info_t;

/* 定义PNG路径 */
const img_info_t PNG_PATH[] =
{
    {"0:/PICTURE/PNG/mlljt.png", "xiaomao.png"},
    {"0:/PICTURE/PNG/laji.png", "laji.png"},
};
/* 获取路径的个数 */
#define image_mun (int)(sizeof(PNG_PATH)/sizeof(PNG_PATH[0]))
  
lv_obj_t *img;

/**
 * @brief       创建PNG图片文件
 * @param       parent：父类
 * @param       path：  图片路径
 * @retval      返回图片控件
 */
lv_obj_t * lv_png_create_from_file(lv_obj_t * parent, const char * path)
{   
    lv_obj_t * img = lv_img_create(parent);
    lv_img_set_src(img, path); 
    lv_obj_align_to(img,NULL,LV_ALIGN_CENTER,0,0);

    return img;
}


/**
 * @brief       LVGL程序入口
 * @param       无
 * @retval      无
 */
void lv_mainstart()
{
    lv_png_init(); /* 初始化PNG解码库 */
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label,"PNG_Decoder");
    lv_obj_align_to(label,NULL,LV_ALIGN_TOP_MID,0,0);

    img = lv_png_create_from_file(lv_scr_act(),PNG_PATH[0].img_path); /* 创建PNG文件 */
}
