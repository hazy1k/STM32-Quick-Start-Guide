#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include <stdio.h>

void USARTx_Init(uint32_t baudrate);
void USART_SendChar(char ch);
char USART_ReceiveChar(void);
void USART_SendString(char *str);

#endif /* __USART_H */
