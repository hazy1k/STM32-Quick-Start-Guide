#ifndef _USART_H
#define _USART_H

#include "sys.h"
	
void UART_Init(uint32_t baudrate);
void HAL_UART_MspInit(UART_HandleTypeDef *huart);

#endif
