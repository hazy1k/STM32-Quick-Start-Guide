#ifndef __DMA_H
#define __DMA_H
#include "sys.h"

extern DMA_HandleTypeDef  UART1TxDMA_Handler;      //DMA���

void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx);
void MYDMA_USART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);

#endif
