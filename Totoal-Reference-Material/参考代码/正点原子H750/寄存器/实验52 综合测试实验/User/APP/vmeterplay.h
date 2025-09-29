/**
 ****************************************************************************************************
 * @file        vmeterplay.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-06-06
 * @brief       APP-电压表测试 代码
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

#ifndef __VMETERPLAY_H
#define __VMETERPLAY_H

#include "common.h"


void vmeter_show_7seg(uint16_t x,uint16_t y,uint16_t xend,uint16_t yend,uint16_t offset,uint16_t color,uint16_t size,uint8_t chr,uint8_t mode);
void vmeter_show_num(uint16_t x,uint16_t y,uint8_t len,uint16_t color,uint8_t size,long long num,uint8_t mode);
uint8_t vmeter_play(void);

#endif

