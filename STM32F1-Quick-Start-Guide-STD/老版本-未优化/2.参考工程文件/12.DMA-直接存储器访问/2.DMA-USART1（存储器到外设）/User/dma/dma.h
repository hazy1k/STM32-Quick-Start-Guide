#ifndef __DMA_H
#define __DMA_H

#include "led.h"

// DMA相关参数宏定义
#define USART_TX_DMA_CHANNEL DMA1_Channel4 // 使用DMA1通过通道4传输USART1发送数据
#define USART_DR_ADDRESS (USART1_BASE+0x04) // USART1数据寄存器地址
#define SendBuff_Size 5000 // 发送缓冲区大小

void USARTx_DMA_Init(void); // USART1 DMA初始化函数

#endif /* __DMA_H */
