/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL SPI Flash读取图片库实验
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
#include "./BSP/NORFLASH/norflash.h"
#include "./BSP/NORFLASH/norflash_ex.h"
#include "./BSP/QSPI/qspi.h"
#include "./MALLOC/malloc.h"
#include "image.h"
#include "lvgl.h"
#include <stdio.h>


LV_FONT_DECLARE(myFont14) \
LV_FONT_DECLARE(myFont24)

/**
 * @brief       从SPI FLASH获取图片(BMP,JEG,PNG)
 * @param       image-存储图片信息的指针
 * @retval      0-加载成功 非 0-加载失败
 */
uint8_t lv_load_img(lv_img_dsc_t *image,uint32_t addr,uint32_t size)
{
    uint8_t *image_buffer;
    uint32_t image_header;
    uint8_t *image_jpeg;

    /* 申请图片内存,图片需要一直显示,内存不用释放 */
    image_buffer = mymalloc(SRAMIN,size); /* 申请内存 */
    
    norflash_ex_read(image_buffer,addr,size);
  
    /* 获取头 */
    image_header = (uint32_t)image_buffer[3] << 24;
    image_header |= (uint32_t)image_buffer[2] << 16;
    image_header |= (uint32_t)image_buffer[1] << 8;
    image_header |= (uint32_t)image_buffer[0];
    
    /* 获取图片数据流 */
    image_jpeg = (uint8_t *)image_buffer+4;
    
    image->header.cf = image_buffer[0];
    image->header.always_zero = 0;
    image->header.w = (uint16_t)(image_header >> 10);
    image->header.h = (uint16_t)(image_header >> 21);
    image->data_size = size - 4; /* 去掉4字节的头,剩余的就是图片数据流长度 */
    image->data = image_jpeg;

    return 0;
}

/* 定义个图片 */
lv_img_dsc_t image_bin1;
lv_obj_t *image;
lv_img_dsc_t image_bin2;
lv_obj_t *image1;
lv_img_dsc_t image_bin3;
lv_obj_t *image2;
int image_flag;
int image_number = 0;
int zoom_factor_size = 128;

/**
 * @brief       定时器函数
 * @param       task：任务控制块
 * @retval      无
 */
void lv_my_timer(lv_timer_t * t)
{
    uint32_t * user_data = t->user_data;

    if (*user_data == 0)
    {
        image_number = image_number + 100;   /* 0~3600转成0°~360°必须除以10 */
        lv_img_set_angle(image,image_number);/* 设置角度 */
        lv_img_set_antialias(image,true);    /* 抗锯齿 */
    }
    
    if (image_number >= 3600 || *user_data == 1)
    {
        *user_data = 1;
        lv_img_set_zoom(image,zoom_factor_size); /* 设置缩小放大 */
        zoom_factor_size = zoom_factor_size*2;   /* 128最小，256不变，512最大 */

        if (zoom_factor_size >= 512)
        {
            zoom_factor_size = 128;
            image_number = 0;
            *user_data = 0;
        }
    }
}

/**
 * @brief       测试代码
 * @param       无
 * @retval      无
 */
void lv_mainstart()
{
    static uint32_t user_data = 0;

    lv_obj_set_style_bg_color(lv_scr_act(),lv_color_white(),0);
    image_flag = lv_load_img(&image_bin1,ftinfo.lvgl_atk01addr,ftinfo.lvgl_atk01size); /* 从文件加载 bin 格式的图片到图片变量 */
    image_flag = lv_load_img(&image_bin2,ftinfo.lvgl_atk03addr,ftinfo.lvgl_atk03size); /* 从文件加载 bin 格式的图片到图片变量 */
    image_flag = lv_load_img(&image_bin3,ftinfo.lvgl_moneyaddr,ftinfo.lvgl_moneysize); /* 从文件加载 bin 格式的图片到图片变量 */
  
    if (image_flag == 0)
    {
        image = lv_img_create(lv_scr_act());                /* 创建 image 控件 */
        lv_img_set_src(image,&image_bin1);                  /* 为控件设置图片 */
        lv_obj_align(image,LV_ALIGN_CENTER,0,0);            /* 设置控件的对齐方式,相对坐标 */
        lv_img_set_pivot(image, image_bin1.header.w/2, image_bin1.header.h/2);   /* 设置中心点 */
      
        image1 = lv_img_create(lv_scr_act());               /* 创建 image 控件 */
        lv_img_set_src(image1,&image_bin2);                 /* 为控件设置图片 */
        lv_obj_align(image1,LV_ALIGN_TOP_RIGHT,0,0);        /* 设置控件的对齐方式,相对坐标 */
        lv_img_set_pivot(image1, image_bin2.header.w/2, image_bin2.header.h/2);  /* 设置中心点 */

        image2 = lv_img_create(lv_scr_act());                /* 创建 image 控件 */
        lv_img_set_src(image2,&image_bin3);                  /* 为控件设置图片 */
        lv_obj_align(image2,LV_ALIGN_BOTTOM_LEFT,0,0);       /* 设置控件的对齐方式,相对坐标 */
        lv_img_set_pivot(image2, image_bin3.header.w/2, image_bin3.header.h/2);  /* 设置中心点 */

        /* 创建一个“lv_timer” */
        lv_timer_create(lv_my_timer, 1000, &user_data);
        
    }
}
