/**
 ****************************************************************************************************
 * @file        dma.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-26
 * @brief       DMA 驱动代码
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
 * V1.0 20200326
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __DMA_H
#define __DMA_H

#include "./SYSTEM/sys/sys.h"


void dma_init(DMA_Stream_TypeDef *dma_stream_handle, uint32_t ch);  /* 配置DMAx_CHx */
void dma_mux_init(DMA_Stream_TypeDef *DMA_Streamx, uint8_t ch);     /* DMA请求复用器初始化 */
void dma_enable(DMA_Stream_TypeDef *DMA_Streamx, uint16_t ndtr);    /* 使能一次DMA传输 */
#endif
