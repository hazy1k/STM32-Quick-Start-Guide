#include "stm32f10x.h"
#include "led.h"
#include "spi_flash.h"
#include "usart.h"

// 定义测试用变量
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

// 获取缓冲区的长度
#define countof(a) (sizeof(a)/sizeof(*(a)))
#define TxBufferSize1 (countof(TxBuffer1)-1)
#define RxBufferSize1 (countof(RxBuffer1)-1)
#define BufferSize (countof(Tx_Buffer))

// 配置SPI FLASH 相关参数
#define FLASH_WriteAddress 0x00000 // 写地址
#define FLASH_ReadAddress FLASH_WriteAddress   // 读地址
#define FLASH_SectorToErase FLASH_WriteAddress // 扇区擦除地址

// 发送缓冲区初始化
uint8_t Tx_Buffer[] = "Hello";
uint8_t Rx_Buffer[BufferSize];

__IO uint32_t DeviceID = 0; // 设备ID
__IO uint32_t FlashID = 0; // FLASH ID
__IO TestStatus TransferStatus1 = FAILED; // 测试结果

// 比较两个缓冲区数据是否相等
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
	while(BufferLength--)
	{
		if(*pBuffer1!=*pBuffer2)
		{
			return FAILED;
		}
		pBuffer1++;
		pBuffer2++;
	}
	return PASSED;
}

void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

int main(void)
{
	LED_Init();
	LED_BLUE();
	USART_Config();
	SPI_FLASH_Init();
	DeviceID = SPI_FLASH_ReadDeviceID(); // 读取设备ID
	Delay(200);
	FlashID = SPI_FLASH_ReadID(); // 读取FLASH ID
	Delay(200);
	printf("DeviceID = 0x%08X\r\n", DeviceID);
	printf("FlashID = 0x%08X\r\n", FlashID);
	if(FlashID == sFLASH_ID)
	{
		printf("\r\n 检测到flash W25Q64 \r\n");
		SPI_FLASH_SectorErase(FLASH_SectorToErase); // 擦除扇区
		SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize); // 写数据
		printf("\r\n 写入的数据为：%s \r\n", Tx_Buffer);
		Delay(200);
		SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize); // 读数据
		printf("\r\n 读出的数据为：%s \r\n", Rx_Buffer);
		Delay(200);
		TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize); // 比较读写数据是否一致
		if(TransferStatus1 == PASSED)
		{
			LED_GREEN();
			printf("\r\n 读写测试成功 \r\n");
		}
		else
		{
			LED_RED();
			printf("\r\n 读写测试失败 \r\n");
		}
	}
	else
	{
		LED_YELLOW();
		printf("\r\n 未检测到flash W25Q64 \r\n");
	}
	while(1)
	{
	}
}
