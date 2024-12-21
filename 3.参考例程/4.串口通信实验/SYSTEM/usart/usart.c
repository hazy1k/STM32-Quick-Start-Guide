#include "usart.h"
#include "delay.h"

UART_HandleTypeDef UART1_Handler;

void UART_Init(uint32_t baudrate)
{
	UART1_Handler.Instance = USART1;
	UART1_Handler.Init.BaudRate = baudrate;
	UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B;// 8λ����λ
	UART1_Handler.Init.StopBits = UART_STOPBITS_1;     // 1λֹͣλ
	UART1_Handler.Init.Parity = UART_PARITY_NONE;      // ��У��λ
	UART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;// ��Ӳ������
	UART1_Handler.Init.Mode = UART_MODE_TX_RX;         // �շ�ģʽ
	HAL_UART_Init(&UART1_Handler);
}

// �˺����ᱻHAL_UART_Init()���ã����ڳ�ʼ�����������GPIO
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	if(huart->Instance==USART1) // ����Ǵ���1�����ʼ��GPIO��NVIC
	{
		__HAL_RCC_USART1_CLK_ENABLE(); // ʹ�ܴ���1ʱ��
		__HAL_RCC_GPIOA_CLK_ENABLE(); // ʹ��GPIOAʱ��
		// ��ʼ������1��TX��RX����
		GPIO_InitStructure.Pin = GPIO_PIN_9 | GPIO_PIN_10;
		GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStructure.Pull = GPIO_PULLUP;
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStructure.Alternate = GPIO_AF7_USART1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
		// NVIC����
		HAL_NVIC_EnableIRQ(USART1_IRQn);
		HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
	}
}
