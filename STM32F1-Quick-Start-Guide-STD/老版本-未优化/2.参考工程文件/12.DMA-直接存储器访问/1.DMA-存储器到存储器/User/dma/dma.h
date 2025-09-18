#ifndef __DMA_H
#define __DMA_H

#include "stm32f10x.h"

// 选择使用的通道和时钟
#define DMA_CHx  DMA1_Channel6
#define DMA_CLK  RCC_AHBPeriph_DMA1
// 传输完成标志
#define DMA_FLAG_TC   DMA1_FLAG_TC6
// 要发送的数据的最长长度
#define DMA_BUFFER_SIZE  32

extern uint32_t Send_DMA_BUFFER[DMA_BUFFER_SIZE];
extern uint32_t Rece_DMA_BUFFER[DMA_BUFFER_SIZE];

void DMA_Config(void);

#endif /* __DMA_H */
