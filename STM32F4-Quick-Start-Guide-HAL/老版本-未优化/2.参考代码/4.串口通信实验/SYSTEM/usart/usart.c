#include "usart.h"
#include "delay.h"

UART_HandleTypeDef UART1_Handler;

void UART_Init(uint32_t baudrate)
{
	UART1_Handler.Instance = USART1;
	UART1_Handler.Init.BaudRate = baudrate;
	UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B;// 8位数据位
	UART1_Handler.Init.StopBits = UART_STOPBITS_1;     // 1位停止位
	UART1_Handler.Init.Parity = UART_PARITY_NONE;      // 无校验位
	UART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;// 无硬件流控
	UART1_Handler.Init.Mode = UART_MODE_TX_RX;         // 收发模式
	HAL_UART_Init(&UART1_Handler);
}

// 此函数会被HAL_UART_Init()调用，用于初始化串口外设的GPIO
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	if(huart->Instance==USART1) // 如果是串口1，则初始化GPIO和NVIC
	{
		__HAL_RCC_USART1_CLK_ENABLE(); // 使能串口1时钟
		__HAL_RCC_GPIOA_CLK_ENABLE(); // 使能GPIOA时钟
		// 初始化串口1的TX、RX引脚
		GPIO_InitStructure.Pin = GPIO_PIN_9 | GPIO_PIN_10;
		GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStructure.Pull = GPIO_PULLUP;
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStructure.Alternate = GPIO_AF7_USART1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
		// NVIC配置
		HAL_NVIC_EnableIRQ(USART1_IRQn);
		HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
	}
}
