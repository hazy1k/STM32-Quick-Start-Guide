/**
 ****************************************************************************************************
 * @file        dcmi.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-03
 * @brief       DCMI 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200403
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef _DCMI_H
#define _DCMI_H

#include "./SYSTEM/sys/sys.h"


/* DCMI DMA接收回调函数,需要用户实现该函数 */
extern void (*dcmi_rx_callback)(void);

extern DCMI_HandleTypeDef g_dcmi_handle;          /* DCMI句柄 */
extern DMA_HandleTypeDef g_dma_dcmi_handle;       /* DMA句柄 */

/* 接口函数 */
void dcmi_init(void);
void dcmi_stop(void);
void dcmi_start(void);
void dcmi_dma_init(uint32_t mem0addr,uint32_t mem1addr,uint16_t memsize,uint32_t memblen,uint32_t meminc);

/* 测试用函数 */
void dcmi_cr_set(uint8_t pclk,uint8_t hsync,uint8_t vsync);
void dcmi_set_window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height);
#endif





















