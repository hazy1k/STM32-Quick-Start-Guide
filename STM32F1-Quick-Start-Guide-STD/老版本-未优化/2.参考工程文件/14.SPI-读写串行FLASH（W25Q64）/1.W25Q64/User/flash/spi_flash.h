#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__

#include "stm32f10x.h"
#include <stdio.h>

/* 超时时间 */
#define SPIT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT ((uint32_t)(10*SPIT_FLAG_TIMEOUT))

/* W25QXX ID */
#define sFLASH_ID 0xEF4017 // W25Q64
// #define sFLASH_ID 0xEF4018 // W25Q128
// #define sFLASH_ID 0xEF4015 // W25Q16
// #define sFALSH_ID 0xEF3015 // W25X16

#define SPI_FLASH_PageSize 256 // W25Q64页大小为256字节
#define SPI_FLASH_PerWritePageSize 256 // W25Q64一次写操作最大为256字节

/* W25QXX命令 */
#define W25X_WriteEnable		    0x06 // 写使能 
#define W25X_WriteDisable		    0x04 // 写禁止 
#define W25X_ReadStatusReg		    0x05 // 读取状态寄存器 
#define W25X_WriteStatusReg		    0x01 // 写入状态寄存器
#define W25X_ReadData			    0x03 // 读取数据 
#define W25X_FastReadData		    0x0B // 快速读取数据 
#define W25X_FastReadDual		    0x3B // 快速读取双线 
#define W25X_PageProgram		    0x02 // 页编程 
#define W25X_BlockErase			    0xD8 // 块擦除 
#define W25X_SectorErase		    0x20 // 扇区擦除 
#define W25X_ChipErase			    0xC7 // 全片擦除 
#define W25X_PowerDown			    0xB9 // 掉电 
#define W25X_ReleasePowerDown	    0xAB // 复位掉电 
#define W25X_DeviceID			    0xAB // 读取设备ID 
#define W25X_ManufactDeviceID   	0x90 // 读取厂商ID 
#define W25X_JedecDeviceID		    0x9F // 读取Jedec ID 
/* WIP(BUSY)标志，表示FALSH内部在写入数据 */
#define WIP_Flag              0x01
#define Dummy_Byte            0xFF

/* 信息输出 */
#define FLASH_DEBUG_ON         1
#define FLASH_INFO(fmt,arg...)           printf("<<-FLASH-INFO->> "fmt"\n",##arg)
#define FLASH_ERROR(fmt,arg...)          printf("<<-FLASH-ERROR->> "fmt"\n",##arg)
#define FLASH_DEBUG(fmt,arg...)          do{\
                                          if(FLASH_DEBUG_ON)\
                                          printf("<<-FLASH-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)
/* SPI接口配置宏定义 */
// 1.SPI基础配置
#define FLASH_SPIx SPI1
#define FLASH_SPI_APBxClock_FUN RCC_APB2PeriphClockCmd
#define FLASH_SPI_CLK RCC_APB2Periph_SPI1
// 2.CS引脚配置(PC0)
#define FLASH_SPI_CS_APBxClock_FUN RCC_APB2PeriphClockCmd
#define FLASH_SPI_CS_CLK RCC_APB2Periph_GPIOC
#define FLASH_SPI_CS_PORT GPIOC
#define FLASH_SPI_CS_PIN GPIO_Pin_0
// 3.SCK引脚配置(PA5)
#define FLASH_SPI_SCK_APBxClock_FUN RCC_APB2PeriphClockCmd
#define FLASH_SPI_SCK_CLK RCC_APB2Periph_GPIOA
#define FLASH_SPI_SCK_PORT GPIOA
#define FLASH_SPI_SCK_PIN GPIO_Pin_5
// 4.MISO引脚配置(PA6)
#define FLASH_SPI_MISO_APBxClock_FUN RCC_APB2PeriphClockCmd
#define FLASH_SPI_MISO_CLK RCC_APB2Periph_GPIOA
#define FLASH_SPI_MISO_PORT GPIOA
#define FLASH_SPI_MISO_PIN GPIO_Pin_6
// 5.MOSI引脚配置(PA7)
#define FLASH_SPI_MOSI_APBxClock_FUN RCC_APB2PeriphClockCmd
#define FLASH_SPI_MOSI_CLK RCC_APB2Periph_GPIOA
#define FLASH_SPI_MOSI_PORT GPIOA
#define FLASH_SPI_MOSI_PIN GPIO_Pin_7
// 6.CS高低电平执行函数
#define SPI_FLASH_CS_LOW() GPIO_ResetBits(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN)
#define SPI_FLASH_CS_HIGH() GPIO_SetBits(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN)

// 函数声明
void SPI_FLASH_Init(void);
uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorcode);
uint8_t SPI_FLASH_SendByte(uint8_t byte);
uint16_t SPI_FLASH_SendHalfWord(uint16_t halfword);
uint8_t SPI_FLASH_ReadByte(void);
void SPI_FLASH_ReadStatusReg(uint32_t ReadAddr);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);
void SPI_FLASH_PowerDowm(void);
void SPI_FLASH_WakeUp(void);
void SPI_FLASH_SectorErase(uint32_t SectorAddress);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t SPI_FLASH_ReadDeviceID(void);
uint32_t SPI_FLASH_ReadID(void);

#endif /* __SPI_FLASH_H__ */
