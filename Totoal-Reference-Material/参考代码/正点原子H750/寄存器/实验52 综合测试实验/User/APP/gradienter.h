/**
 ****************************************************************************************************
 * @file        gradienter.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-水平仪 代码
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

#ifndef __GRADIENTER_H
#define __GRADIENTER_H

#include "common.h"


void grad_draw_hline(short x0,short y0,uint16_t len,uint16_t color);
void grad_fill_circle(short x0,short y0,uint16_t r,uint16_t color);
uint8_t grad_load_font(void);
void grad_delete_font(void);

uint8_t grad_play(void); 

#endif



