#include "led.h"
#include "spi_flash.h"
#include "usart.h"

__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;

void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

int main(void)
{
	uint8_t flag = 0;
	uint8_t k;
    long double doule_buffer[7] = {0}; // 小数数组缓冲区
	int long_buffer[7] = {0}; // 长整型数组缓冲区
	LED_Init();
	LED_BLUE();
	SPI_FLASH_Init();
	USART_Config();
	printf("SPI FLASH Test Program\r\n");
	DeviceID = SPI_FLASH_ReadDeviceID(); // 读取设备ID
	Delay(200);
	FlashID = SPI_FLASH_ReadID(); // 读取Flash ID
	printf("Device ID: 0x%X\r\n", DeviceID);
	printf("Flash ID: 0x%X\r\n", FlashID);
	if(FlashID == sFLASH_ID)
	{
		LED_GREEN();
		printf("检测到W25Q64\r\n");
		SPI_FLASH_BufferRead(&flag, SPI_FLASH_PageSize*0, 1);
		if(flag == 0xCD) // 检测数据标志
		{
			printf("之前已经写入数据\r\n");
			// 读取小数数据
			SPI_FLASH_BufferRead((void*)doule_buffer, SPI_FLASH_PageSize*1, sizeof(doule_buffer));
			// 读取长整型数据
			SPI_FLASH_BufferRead((void*)long_buffer, SPI_FLASH_PageSize*2, sizeof(long_buffer));
			printf("\r\n 从芯片读到的数据为: \r\n");
			for(k = 0; k < 7; k++)
			{
				printf("小数 = %LF \r\n", doule_buffer[k]);
				printf("长整型 = %d \r\n", long_buffer[k]);
			}
		}
		else // 未写入数据
		{
			printf("未写入数据, 准备写入数据\r\n");
			flag = 0xCD; // 数据标志
			SPI_FLASH_SectorErase(0); // 擦除第0页
			SPI_FLASH_BufferWrite(&flag, SPI_FLASH_PageSize*0, 1); // 写入数据标志
			for(k = 0; k < 7; k++)
			{
				doule_buffer[k] = k +0.01; // 写入小数数据
				long_buffer[k] = k + 1000000000; // 写入长整型数据
			}
			SPI_FLASH_BufferWrite((void*)doule_buffer, SPI_FLASH_PageSize*1, sizeof(doule_buffer));
			SPI_FLASH_BufferWrite((void*)long_buffer, SPI_FLASH_PageSize*2, sizeof(long_buffer));
			printf("向芯片写入的数据为: \r\n");
			for(k = 0; k < 7; k++)
			{
				printf("小数 = %LF \r\n", doule_buffer[k]);
				printf("长整型 = %d \r\n", long_buffer[k]);
			}
			printf("\r\n 复位开发板，进行数据效验 \r\n");
		}
	}
	else
	{
		LED_RED();
		printf("未检测到W25Q64\r\n");
	}
	while(1)
	{}
}
