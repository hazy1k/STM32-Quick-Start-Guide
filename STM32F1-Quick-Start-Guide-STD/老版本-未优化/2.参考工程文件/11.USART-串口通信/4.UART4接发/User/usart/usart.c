#include "usart.h"

static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	USARTx_GPIO_APBxClkCmd(USARTx_GPIO_CLK, ENABLE);
	USARTx_APBxCLKCmd(USARTx_CLK, ENABLE);
	// TX引脚配置
	GPIO_InitStructure.GPIO_Pin = USARTx_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure);
    // RX引脚配置
	GPIO_InitStructure.GPIO_Pin = USARTx_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 输入浮空
	GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);
	// USART 配置
	USART_InitStructure.USART_BaudRate = USARTx_BaudRate; // 波特率115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 字长8位数据
	USART_InitStructure.USART_StopBits = USART_StopBits_1; // 一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No; // 无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
	USART_Init(USARTx, &USART_InitStructure);
	// NVIC配置
	NVIC_Configuration();
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE); // 使能串口接收中断
	USART_Cmd(USARTx, ENABLE); // 使能串口
}

// ---------编写接发数据函数----------

// 发送一个字节函数
// 函数参数：USARTx：串口号，ch：发送的字节
void USART_SendByte(USART_TypeDef *pUSARTx, uint8_t ch)
{
	USART_SendData(pUSARTx, ch);
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET); // 等待发送完成
}

// 发送一个8位数组函数
// 函数参数：USARTx：串口号，data：发送的8位数据
void USART_SendArray(USART_TypeDef *pUSARTx, uint8_t *pArray, uint16_t len)
{
	uint8_t i;
	for(i = 0; i < len; i++)
	{
		USART_SendByte(pUSARTx, pArray[i]); // 循环写入数组
	}
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET); // 等待发送完成
}

// 发送一个字符串函数
// 函数参数：USARTx：串口号，pStr：发送的字符串
void USART_SendString(USART_TypeDef *pUSARTx, uint8_t *pStr)
{
	unsigned int k = 0;
	do{
		USART_SendByte(pUSARTx, *(pStr+k)); // 循环写入字符串
		k++;
	}while(*(pStr+k)!= '\0'); // 直到遇到结束符‘\0’
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET);
}

// 发送一个16位数据函数
// 函数参数：USARTx：串口号，ch：发送的16位数据
void USART_SendHalfWord(USART_TypeDef *pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l; // 16位数据要分为高位和低位，两次发送
	temp_h = (ch & 0xff00) >> 8; // 取高位
	temp_l = ch & 0x00ff; // 取低位
	USART_SendByte(pUSARTx, temp_h); // 发送高位
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	USART_SendByte(pUSARTx, temp_l); // 发送低位
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

// 重定向printf和scanf函数到串口
int USART_PutChar(int ch, FILE *f) // 函数参数：ch：要发送的字符，f：文件指针
{
	USART_SendByte(USARTx, (uint8_t)ch); // 发送字符
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
	return ch; // 返回发送的字符
}

int USART_GetChar(FILE *f) // 函数参数：f：文件指针
{
	while(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(USARTx); // 接收字符
}
