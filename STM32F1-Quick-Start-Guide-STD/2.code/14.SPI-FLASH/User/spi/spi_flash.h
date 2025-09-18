#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__ 

#include "stm32f10x.h"
#include "usart.h"
#include <stdio.h>

/* 超时时间配置 */
#define SPI_FLAG_TIMEOUT         ((uint32_t)0x1000)  // 单次操作超时计数
#define SPI_LONG_TIMEOUT         ((uint32_t)(10 * SPI_FLAG_TIMEOUT))  // 长操作超时计数

/* W25Q64 芯片参数 */
#define sFLASH_ID                0xEF4017    // W25Q64 的JEDEC ID
#define DUMMY_BYTE               0xFF        // 空字节(用于读取操作)
#define WIP_FLAG                 0x01        // 状态寄存器忙标志位

/* Flash 物理参数 */
#define SPI_FLASH_PageSize       256         // 页大小(字节)
#define SPI_FLASH_SectorSize     4096        // 扇区大小(字节)
#define SPI_FLASH_WorkPageSize   256         // 单次操作最大字节数

/* W25QXX 命令集 */
typedef enum {
    WRITE_ENABLE        = 0x06,     // 写使能
    WRITE_DISABLE       = 0x04,     // 写禁止
    READ_STATUS_REG     = 0x05,     // 读状态寄存器
    WRITE_STATUS_REG    = 0x01,     // 写状态寄存器
    READ_DATA           = 0x03,     // 标准数据读取
    FAST_READ_DATA      = 0x0B,     // 快速数据读取(需要额外dummy byte)
    PAGE_PROGRAM        = 0x02,     // 页编程命令
    SECTOR_ERASE        = 0x20,     // 4KB扇区擦除
    CHIP_ERASE          = 0xC7,     // 整片擦除
    POWER_DOWN          = 0xB9,     // 进入低功耗模式
    RELEASE_POWER_DOWN  = 0xAB,     // 唤醒设备
    JEDEC_DEVICE_ID     = 0x9F      // 读取JEDEC ID
} SPI_Flash_Cmd;

/* 调试输出配置 */
#define FLASH_DEBUG_ON           1           // 调试信息开关

// 信息输出宏(始终启用)
#define FLASH_INFO(fmt, ...)     printf("<<-FLASH-INFO->> " fmt "\n", ##__VA_ARGS__)
#define FLASH_ERROR(fmt, ...)    printf("<<-FLASH-ERROR->> " fmt "\n", ##__VA_ARGS__)

// 调试输出宏(受FLASH_DEBUG_ON控制)
#if FLASH_DEBUG_ON
    #define FLASH_DEBUG(fmt, ...) printf("<<-FLASH-DEBUG->> [%d] " fmt "\n", __LINE__, ##__VA_ARGS__)
#else
    #define FLASH_DEBUG(fmt, ...)
#endif

/* =============== SPI 硬件配置 =============== */
/* SPI 外设配置 */
#define FLASH_SPI                SPI1
#define FLASH_SPI_CLK            RCC_APB2Periph_SPI1

/* GPIO 引脚配置结构体 */
typedef struct {
    GPIO_TypeDef* port;       // GPIO端口
    uint16_t pin;             // 引脚号
    uint32_t rcc_clk;         // 对应的RCC时钟
} SPI_PinConfig;

// CS 引脚配置 (PC0)
#define FLASH_CS_CONFIG          {GPIOC, GPIO_Pin_0, RCC_APB2Periph_GPIOC}

// SCK 引脚配置 (PA5)
#define FLASH_SCK_CONFIG         {GPIOA, GPIO_Pin_5, RCC_APB2Periph_GPIOA}

// MISO 引脚配置 (PA6)
#define FLASH_MISO_CONFIG        {GPIOA, GPIO_Pin_6, RCC_APB2Periph_GPIOA}

// MOSI 引脚配置 (PA7)
#define FLASH_MOSI_CONFIG        {GPIOA, GPIO_Pin_7, RCC_APB2Periph_GPIOA}

/* 控制宏 */
#define SPI_FLASH_CS_LOW()       GPIO_ResetBits(FLASH_CS_CONFIG.port, FLASH_CS_CONFIG.pin)
#define SPI_FLASH_CS_HIGH()      GPIO_SetBits(FLASH_CS_CONFIG.port, FLASH_CS_CONFIG.pin)

/* 函数声明 */
void SPI_Flash_Init(void);                            // 初始化SPI Flash
uint32_t SPI_Flash_ReadID(void);                      // 读取设备ID
void SPI_Flash_EraseSector(uint32_t SectorAddr);      // 擦除扇区(4KB)
void SPI_Flash_WritePage(uint8_t* pBuffer,            // 页编程(最大256字节)
                        uint32_t WriteAddr, 
                        uint16_t NumByteToWrite);
void SPI_Flash_ReadBuffer(uint8_t* pBuffer,           // 读取数据
                         uint32_t ReadAddr, 
                         uint16_t NumByteToRead);
void SPI_Flash_ChipErase(void);                       // 全片擦除
void SPI_Flash_WriteEnable(void);                     // 写使能
void SPI_Flash_WriteDisable(void);                    // 写禁止
void SPI_Flash_PowerDown(void);                       // 进入低功耗
void SPI_Flash_WakeUp(void);                          // 唤醒设备
void SPI_Flash_WriteBuffer(uint8_t* buffer,           // 任意位置写入(自动分页)
                          uint32_t addr, 
                          uint32_t size);

#endif /* __SPI_FLASH_H__ */
