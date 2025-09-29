/**
 ****************************************************************************************************
 * @file        rgbledplay.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-06-06
 * @brief       APP-RGB彩灯测试 代码
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
 * V1.1 20220606
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __RGBLEDPLAY_H
#define __RGBLEDPLAY_H

#include "common.h"
#include "./BSP/LED/led.h"


#define LEDR(X)     LED0(X)
#define LEDG(X)     LED1(X)
#define LEDB(X)     LED2(X)



uint8_t rgbled_palette_draw(uint16_t x,uint16_t y,uint16_t width,uint16_t height);
void rgbled_show_colorval(uint16_t x,uint16_t y,uint8_t size,uint16_t color);
void rgbled_io_config(uint8_t af);
void rgbled_pwm_set(uint16_t color);
uint8_t rgbled_play(void); 

#endif










