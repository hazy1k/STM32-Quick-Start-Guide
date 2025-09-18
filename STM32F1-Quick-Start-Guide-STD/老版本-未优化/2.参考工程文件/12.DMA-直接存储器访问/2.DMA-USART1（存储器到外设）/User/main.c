#include "led.h"
#include "usart.h"
#include "dma.h"

extern uint8_t SendBuff[SendBuff_Size];
static void Delay(__IO uint32_t nCount) // 简单的延时函数
{
    for(; nCount != 0; nCount--);
}

int main()
{
	uint16_t i;
	LED_Init();
	USART_Config();
	USARTx_DMA_Init();
	// for循环填充要发送的数据
	for(i = 0; i < SendBuff_Size; i++)
	{
		SendBuff[i] = 'A';
	}
	// USART向DMA发出TX请求
	USART_DMACmd(USARTx, USART_DMAReq_Tx, ENABLE);
	while(1)
	{
		// DMA不消耗CPU资源，我们可以做其他事，这里我们只是让LED闪烁
		LED1_ON();
		Delay(0xFFFFFF);
		LED1_OFF();
		Delay(0xFFFFFF);
	}
}
