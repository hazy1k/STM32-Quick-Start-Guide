#include "led.h"
#include "dma.h"

// DMA要传输的数据-存储在内部的FLASH中
uint32_t Send_DMA_BUFFER[DMA_BUFFER_SIZE] = {0x11223344, 0x55667788, 0x99AABBCC, 0xDDCCEEFF};
// DMA接收到的数据-存储在内部的SRAM中
uint32_t Rece_DMA_BUFFER[DMA_BUFFER_SIZE];

// 判断两个数据是否相等
uint8_t Buffer_Compare(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t Length)
{
	// 数据长度递增
	while(Length--)
	{
		if(*pBuffer != *pBuffer1)
		{
			return 0; // 如果不同，返回0
		}
		pBuffer++;
		pBuffer1++;
	}
	return 1; // 如果相同，返回1
}

void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}

int main()
{
	uint8_t Buffer_Result; // 两个数据比较结果
	LED_Init();
	LED_PURPLE();
	Delay(0xFFFFFF);
	DMA_Config(); // 启动DMA传输
	while(DMA_GetFlagStatus(DMA_FLAG_TC) == RESET); // 等待DMA传输完成
	// 比较通过DMA传输的数据和预设数据是否相同
	Buffer_Result = Buffer_Compare(Send_DMA_BUFFER, Rece_DMA_BUFFER, DMA_BUFFER_SIZE);
	if(Buffer_Result == 0)
	{
		LED_RED(); // 显示不同
	}
	else
	{
		LED_GREEN(); // 显示相同
	}
	while(1)
	{

	}
}
