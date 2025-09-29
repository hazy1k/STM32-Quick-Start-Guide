/**
 ****************************************************************************************************
 * @file        paint.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-画板 代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.1 20220526
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __PAINT_H
#define __PAINT_H

#include "common.h"

/* 各图标/图片路径 */
extern uint8_t *const PAINT_COLOR_TBL_PIC; /* 颜色表路径 */


/* RGB565位域定义 */
typedef struct
{
    uint16_t b: 5;
    uint16_t g: 6;
    uint16_t r: 5;
} PIX_RGB565;

void paint_new_pathname(uint8_t *pname);
void paint_show_colorval(uint16_t xr, uint16_t yr, uint16_t color);
uint8_t paint_pen_color_set(uint16_t x, uint16_t y, uint16_t *color, uint8_t *caption);
uint8_t paint_pen_size_set(uint16_t x, uint16_t y, uint16_t color, uint8_t *mode, uint8_t *caption);
void paint_draw_point(uint16_t x, uint16_t y, uint16_t color, uint8_t mode);
uint8_t paint_play(void);


#endif























