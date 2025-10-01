/**
 ****************************************************************************************************
 * @file        images.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-04
 * @brief       图片库 代码
 *              提供images_update_image和images_init用于图片库更新和初始化
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
 * 修改说明
 * V1.0 20200404
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __IMAGE_H
#define __IMAGE_H

#include "./SYSTEM/sys/sys.h"



/* 图片信息保存首地址
 * 占41个字节,第1个字节用于标记图片库是否存在.后续每8个字节一组,分别保存起始地址和文件大小
 */
extern uint32_t IMAGEINFOADDR;

/* 图片库信息结构体定义
 * 用来保存图片库基本信息，地址，大小等
 */
typedef __PACKED_STRUCT
{
    uint8_t imageok;             /* 图片库存在标志，0XAA，图片库正常；其他，图片库不存在 */
    
    uint32_t lvgl_atk01addr;    /* LVGL_atk01地址 */
    uint32_t lvgl_atk01size;    /* LVGL_atk01的大小 */
  
    uint32_t lvgl_atk02addr;    /* LVGL_atk02地址 */
    uint32_t lvgl_atk02size;    /* LVGL_atk02的大小 */
  
    uint32_t lvgl_atk03addr;    /* LVGL_atk03地址 */
    uint32_t lvgl_atk03size;    /* LVGL_atk03的大小 */
  
    uint32_t lvgl_moneyaddr;    /* LVGL_money地址 */
    uint32_t lvgl_moneysize;    /* LVGL_money的大小 */

} _image_info;

/* 图片库信息结构体 */
extern _image_info ftinfo;


uint8_t images_update_image(uint16_t x, uint16_t y, uint8_t size, uint8_t *src, uint16_t color);  /* 更新全部图片库 */
uint8_t images_init(void);       /* 初始化图片库 */

#endif
