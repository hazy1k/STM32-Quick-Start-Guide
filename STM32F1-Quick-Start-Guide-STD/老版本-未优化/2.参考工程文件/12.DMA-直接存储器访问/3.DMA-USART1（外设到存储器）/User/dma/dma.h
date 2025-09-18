#ifndef __DMA_H
#define __DMA_H

#include "led.h"

// DMA相关参数宏定义
#define USART_TX_DMA_CHANNEL DMA1_Channel5 
#define USART_DR_ADDRESS (USART1_BASE+0x04) // USART1数据寄存器地址
#define ReceBuff_Size 5000 // 发送缓冲区大小

void USARTx_DMA_Config(void);

#endif /* __DMA_H */
