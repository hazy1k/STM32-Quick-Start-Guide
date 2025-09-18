#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include "stdio.h"

// USARTx 引脚宏定义
#define USARTx_GPIO_CLK RCC_APB2Periph_GPIOC
#define USARTx_GPIO_APBxClkCmd RCC_APB2PeriphClockCmd
// TX-发送数据输出引脚
#define USARTx_TX_GPIO_PORT GPIOC
#define USARTx_TX_GPIO_PIN GPIO_Pin_10
// RX-接收数据输入引脚
#define USARTx_RX_GPIO_PORT GPIOC
#define USARTx_RX_GPIO_PIN GPIO_Pin_11

// USARTx 相关参数宏定义
#define USARTx  UART4
#define USARTx_CLK RCC_APB1Periph_UART4
#define USARTx_APBxCLKCmd RCC_APB1PeriphClockCmd
#define USARTx_BaudRate 115200

// NVIC配置
#define USARTx_IRQ  UART4_IRQn
#define USARTx_IRQHandler UART4_IRQHandler

// 一些函数声明
void USART_Config(void);
void USART_SendByte(USART_TypeDef *pUSARTx, uint8_t ch);
void USART_SendArray(USART_TypeDef *pUSARTx, uint8_t *pArray, uint16_t len);
void USART_SendString(USART_TypeDef *pUSARTx, uint8_t *pStr);
void USART_SendHalfWord(USART_TypeDef *pUSARTx, uint16_t ch);

#endif
