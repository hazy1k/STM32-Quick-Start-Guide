#ifndef __USART_H
#define	__USART_H

#include "stm32f10x.h"
#include <stdio.h>

// 串口1-USART1
#define  USARTx            USART1
#define  USART_CLK         RCC_APB2Periph_USART1
#define  USART_APBxClkCmd  RCC_APB2PeriphClockCmd
#define  USART_BAUDRATE    115200

// USART GPIO 引脚宏定义
#define  USART_GPIO_CLK         (RCC_APB2Periph_GPIOA)
#define  USART_GPIO_APBxClkCmd  RCC_APB2PeriphClockCmd
#define  USART_TX_GPIO_PORT GPIOA   
#define  USART_TX_GPIO_PIN  GPIO_Pin_9
#define  USART_RX_GPIO_PORT GPIOA
#define  USART_RX_GPIO_PIN  GPIO_Pin_10

void USART_Config(void);

#endif /* __USART_H */
