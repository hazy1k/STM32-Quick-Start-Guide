#ifndef __USART_H__
#define __USART_H__

#include "stm32f10x.h"
#include "stdio.h"

#define USARTx USART1
#define USARTx_CLK RCC_APB2Periph_USART1
#define USARTx_GPIO_CLK RCC_APB2Periph_GPIOA
#define USARTx_GPIO_Port GPIOA
#define USARTx_Tx_Pin GPIO_Pin_9
#define USARTx_Rx_Pin GPIO_Pin_10

#define USARTx_IRQn USART1_IRQn
#define USARTx_IRQHandler USART1_IRQHandler

void USARTx_Init(void);
void usart_sendbyte(USART_TypeDef* pUSARTx, uint8_t ch);
void usart_sendstring(USART_TypeDef* pUSARTx, const char *str);
int fputc(int ch, FILE *f);
int fgetc(FILE *f);

#endif /*  __USART_H__ */
