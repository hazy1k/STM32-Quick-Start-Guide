#ifndef __DMA_H__
#define __DMA_H__ 

#include "stm32f10x.h" 

#define DMA_CHx DMA1_Channel6
#define DMA_CLK RCC_AHBPeriph_DMA1
#define DMA_FLAG_TC DMA1_FLAG_TC6 // 传输完成标志
#define DMA_BUFFER_SIZE 32 // DMA缓存大小

extern uint32_t Send_DMA_BUFFER[DMA_BUFFER_SIZE];
extern uint32_t Rece_DMA_BUFFER[DMA_BUFFER_SIZE];

void BSP_DMA_Init(void);

#endif /* __DMA_H__ */
