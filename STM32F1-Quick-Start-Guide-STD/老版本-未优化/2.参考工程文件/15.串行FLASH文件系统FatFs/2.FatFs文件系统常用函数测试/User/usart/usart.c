#include "usart.h"

// USART GPIO 配置,工作参数配置
void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	USART_GPIO_APBxClkCmd(USART_GPIO_CLK, ENABLE);
	USART_APBxClkCmd(USART_CLK, ENABLE);
	// 将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART_TX_GPIO_PORT, &GPIO_InitStructure);
  	// 将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART_RX_GPIO_PORT, &GPIO_InitStructure);
	// 配置串口的工作参数
	USART_InitStructure.USART_BaudRate = USART_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// 初始化串口
	USART_Init(USARTx, &USART_InitStructure);
	USART_Cmd(USARTx, ENABLE);	    
}

// 重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据到串口 */
	USART_SendData(USARTx, (uint8_t) ch);	
	/* 等待发送完毕 */
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);		
	return (ch);
}

// 重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
	/* 等待串口输入数据 */
	while (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(USARTx);
}
