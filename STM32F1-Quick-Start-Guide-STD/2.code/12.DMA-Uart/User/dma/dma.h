#ifndef __DMA_H__
#define __DMA_H__ 

#include "stm32f10x.h" 

#define USART_DMA_CHx      DMA1_Channel4
#define DMA_CLK            RCC_AHBPeriph_DMA1
#define USART_DR_ADDRESS   (USART1_BASE + 0x04) // USART1 数据寄存器地址
#define DMA_TC_FLAG        DMA1_FLAG_TC4        // 通道4传输完成标志
#define DMA_BUFFER_SIZE    32                   // DMA缓存大小

extern uint8_t Send_DMA_BUFFER[DMA_BUFFER_SIZE]; // 发送缓冲区

void USART_DMA_Init(void);
void DMA_StartTransfer(void);
uint8_t DMA_IsTransferComplete(void);

#endif /* __DMA_H__ */
