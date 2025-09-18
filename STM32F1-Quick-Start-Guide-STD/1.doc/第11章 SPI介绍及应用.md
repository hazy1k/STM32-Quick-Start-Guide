# 第十一章 SPI介绍及应用

## 1. SPI简介

![屏幕截图 2025-06-29 131236.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/06/29-13-12-40-屏幕截图%202025-06-29%20131236.png)

SPI 通讯使用 3 条总线及片选线， 3 条总线分别为 SCK、 MOSI、 MISO，片选线为 SS，它们的作用介绍如下：

1. SS ( Slave Select)：从设备选择信号线，常称为片选信号线，也称为 NSS、 CS，以下用 NSS 表示。当有多个 SPI 从设备与 SPI 主机相连时，设备的其它信号线 SCK、 MOSI 及 MISO 同时并联到相同的 SPI 总线上，即无论有多少个从设备，都共同只使用这 3 条总线；而每个从设备都有独立的这一条 NSS 信号线，本信号线独占主机的一个引脚，即有多少个从设备，就有多少条片选信号线。 I2C 协议中通过设备地址来寻址、选中总线上的某个设备并与其进行通讯；而 SPI 协议中没有设备地址，它使用 NSS 信号线来寻址，当主机要选择从设备时，把该从设备的 NSS 信号线设置为低电平，该从设备即被选中，即片选有效，接着主机开始与被选中的从设备进行 SPI 通讯。所以 SPI 通讯以 NSS 线置低电平为开始信号，以 NSS 线被拉高作为结束信号。

2. SCK (Serial Clock)：时钟信号线，用于通讯数据同步。它由通讯主机产生，决定了通讯的速率，不同的设备支持的最高时钟频率不一样，如 STM32 的 SPI 时钟频率最大为 fpclk/2，两个设备之间通讯时，通讯速率受限于低速设备。

3. MOSI (Master Output， Slave Input)：主设备输出/从设备输入引脚。主机的数据从这条信号线输出，从机由这条信号线读入主机发送的数据，即这条线上数据的方向为主机到从机。

4. MISO (Master Input,， Slave Output)：主设备输入/从设备输出引脚。主机从这条信号线读入数据，从机的数据由这条信号线输出到主机，即在这条线上数据的方向为从机到主机。

STM32 的 SPI 外设可用作通讯的主机及从机，支持最高的 SCK 时钟频率为 fpclk/2 (STM32F103 型号的芯片默认 fpclk1 为 36MHz， fpclk2 为 72MHz)，完全支持 SPI 协议的 4 种模式，数据帧长度可设置为 8 位或 16 位，可设置数据 MSB 先行或 LSB 先行。它还支持双线全双工 (前面小节说明的都是这种模式)、双线单向以及单线模式。其中双线单向模式可以同时使用 MOSI 及 MISO 数据线向一个方向传输数据，可以加快一倍的传输速度。而单线模式则可以减少硬件接线，当然这样速率会受到影响。我们只讲解双线全双工模式。

## 2. SPI结构体

跟其它外设一样， STM32 标准库提供了 SPI 初始化结构体及初始化函数来配置 SPI 外设。初始化结构体及函数定义在库文件“stm32f10x_spi.h”及“stm32f10x_spi.c”中。

```c
typedef struct
{
    uint16_t SPI_Direction; // 设置 SPI 的单双向模式 
    uint16_t SPI_Mode; // 设置 SPI 的主/从机端模式 
    uint16_t SPI_DataSize; // 设置 SPI 的数据帧长度，可选 8/16 位 
    uint16_t SPI_CPOL; // 设置时钟极性 CPOL，可选高/低电平 
    uint16_t SPI_CPHA; // 设置时钟相位，可选奇/偶数边沿采样 
    uint16_t SPI_NSS; // 设置 NSS 引脚由 SPI 硬件控制还是软件控制
    uint16_t SPI_BaudRatePrescaler; // 设置时钟分频因子， fpclk/分频数 =fSCK 
    uint16_t SPI_FirstBit; // 设置 MSB/LSB 先行 
    uint16_t SPI_CRCPolynomial; // 设置 CRC 校验的表达式 
}SPI_InitTypeDef;
```

1. **`SPI_Direction`**:
   
   - **功能**：设置 SPI 的数据传输方向。
   - **可能值**：
     - `SPI_DIRECTION_2LINES`：双向数据传输（全双工模式）。
     - `SPI_DIRECTION_2LINES_RXONLY`：只接收数据（半双工模式）。
     - `SPI_DIRECTION_1LINE`：单向数据传输（单工模式）。

2. **`SPI_Mode`**:
   
   - **功能**：设置 SPI 的工作模式（主模式或从模式）。
   - **可能值**：
     - `SPI_MODE_MASTER`：SPI 工作在主模式。
     - `SPI_MODE_SLAVE`：SPI 工作在从模式。

3. **`SPI_DataSize`**:
   
   - **功能**：设置 SPI 的数据帧长度，可以是 8 位或 16 位。
   - **可能值**：
     - `SPI_DATASIZE_8BIT`：8 位数据帧。
     - `SPI_DATASIZE_16BIT`：16 位数据帧。

4. **`SPI_CPOL`**:
   
   - **功能**：设置 SPI 的时钟极性 (CPOL)，定义时钟信号的空闲状态。
   - **可能值**：
     - `SPI_CPOL_LOW`：时钟空闲时为低电平。
     - `SPI_CPOL_HIGH`：时钟空闲时为高电平。

5. **`SPI_CPHA`**:
   
   - **功能**：设置 SPI 的时钟相位 (CPHA)，决定数据采样的时钟边沿。
   - **可能值**：
     - `SPI_CPHA_1EDGE`：数据在第一个时钟沿（上升沿或下降沿）采样。
     - `SPI_CPHA_2EDGE`：数据在第二个时钟沿（与CPOL相关）采样。

6. **`SPI_NSS`**:
   
   - **功能**：设置 NSS（片选）引脚的控制方式。
   - **可能值**：
     - `SPI_NSS_HARD`：NSS 由 SPI 硬件控制。
     - `SPI_NSS_SOFT`：NSS 由软件控制。

7. **`SPI_BaudRatePrescaler`**:
   
   - **功能**：设置 SPI 时钟的分频因子，决定 SPI 时钟的频率。计算公式为：`f_SCK = f_PCLK / Prescaler`，即时钟源频率除以分频因子，得到 SPI 的时钟频率。
   - **可能值**：此值通常为 2、4、8、16、32、64、128 或 256。

8. **`SPI_FirstBit`**:
   
   - **功能**：设置 SPI 数据的传输顺序。
   - **可能值**：
     - `SPI_FIRSTBIT_MSB`：数据的最高位（MSB）先行。
     - `SPI_FIRSTBIT_LSB`：数据的最低位（LSB）先行。

9. **`SPI_CRCPolynomial`**:
   
   - **功能**：设置 SPI CRC 校验的多项式，用于校验数据的正确性。此值设置了 CRC 校验的多项式表达式，只有在启用了 CRC 校验时才有效。
   - **可能值**：用户根据具体的 CRC 配置要求设置多项式。

## 3. SPI使用示例

本实验板中的 FLASH 芯片 (型号： W25Q64) 是一种使用 SPI 通讯协议的 NOR FLASH 存储器，它的 CS/CLK/DIO/DO 引脚分别连接到了 STM32 对应的 SPI 引脚 NSS/SCK/MOSI/MISO 上，其中STM32 的 NSS 引脚是一个普通的 GPIO，不是 SPI 的专用 NSS 引脚，所以程序中我们要使用软件控制的方式。

FLASH 芯片中还有 WP 和 HOLD 引脚。 WP 引脚可控制写保护功能，当该引脚为低电平时，禁止写入数据。我们直接接电源，不使用写保护功能。 HOLD 引脚可用于暂停通讯，该引脚为低电平时，通讯暂停，数据输出引脚输出高阻抗状态，时钟和数据输入引脚无效。我们直接接电源，不使用通讯暂停功能。

![屏幕截图 2025-06-29 132614.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/06/29-13-26-19-屏幕截图%202025-06-29%20132614.png)

### 3.1 spi_flash相关参数宏定义

```c
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
```

### 3.2 spi_flash配置和主要功能函数

```c
#include "spi_flash.h"

/* =============== 硬件配置 =============== */
static SPI_PinConfig CS_Config = FLASH_CS_CONFIG;
static SPI_PinConfig SCK_Config = FLASH_SCK_CONFIG;
static SPI_PinConfig MISO_Config = FLASH_MISO_CONFIG;
static SPI_PinConfig MOSI_Config = FLASH_MOSI_CONFIG;

/* =============== 局部变量 =============== */
static __IO uint32_t SPITimeout = SPI_FLAG_TIMEOUT;  // SPI操作超时计数器

/* =============== 辅助函数 =============== */

// 简化CS控制宏
#define CS_LOW()    GPIO_ResetBits(CS_Config.port, CS_Config.pin)
#define CS_HIGH()   GPIO_SetBits(CS_Config.port, CS_Config.pin)

/**
  * @brief  SPI超时回调函数
  * @param  errorcode: 错误代码(0=发送超时, 1=接收超时)
  * @retval 总是返回0(用于SPI传输)
  */
static uint8_t SPI_TIMEOUT_Callback(uint8_t errorcode)
{
    FLASH_ERROR("SPI timeout! Error code: %d", errorcode);
    return 0;  // 返回0作为安全值
}

/**
  * @brief  SPI单字节传输
  * @param  data: 要发送的数据
  * @retval 接收到的数据
  */
static uint8_t SPI_TransferByte(uint8_t data)
{
    // 等待发送缓冲区空(TXE=1)
    uint32_t timeout = SPI_FLAG_TIMEOUT;
    while (!SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_TXE)) {
        if (timeout-- == 0) return SPI_TIMEOUT_Callback(0);
    }
    
    // 写入数据到数据寄存器
    SPI_I2S_SendData(FLASH_SPI, data);
    
    // 等待接收缓冲区非空(RXNE=1)
    timeout = SPI_FLAG_TIMEOUT;
    while (!SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_RXNE)) {
        if (timeout-- == 0) return SPI_TIMEOUT_Callback(1);
    }
    
    // 读取接收到的数据
    return SPI_I2S_ReceiveData(FLASH_SPI);
}

/**
  * @brief  发送24位地址
  * @param  addr: 24位地址(0-16MB范围)
  */
static void SPI_SendAddress(uint32_t addr)
{
    SPI_TransferByte((addr >> 16) & 0xFF); // 地址高字节(23-16位)
    SPI_TransferByte((addr >> 8) & 0xFF);  // 地址中字节(15-8位)
    SPI_TransferByte(addr & 0xFF);         // 地址低字节(7-0位)
}

/**
  * @brief  等待Flash操作完成
  * @note   轮询状态寄存器，直到WIP标志清除
  */
static void SPI_Flash_WaitBusy(void)
{
    uint8_t status;
    uint32_t timeout = SPI_LONG_TIMEOUT;  // 使用长超时等待擦除/写入完成
    
    do {
        CS_LOW();
        SPI_TransferByte(READ_STATUS_REG);  // 发送读状态寄存器命令
        status = SPI_TransferByte(DUMMY_BYTE); // 读取状态寄存器值
        CS_HIGH();
        
        if (timeout-- == 0) {
            FLASH_ERROR("WaitBusy timeout!");
            break;
        }
    } while (status & WIP_FLAG);  // 检查忙标志(WIP)
}

/* =============== 公共函数 =============== */

/**
  * @brief  初始化SPI Flash
  * @note   配置SPI引脚和SPI外设
  */
void SPI_Flash_Init(void)
{
    SPI_InitTypeDef  spiInit = {0};
    GPIO_InitTypeDef gpioInit = {0};
    
    // 1. 使能时钟 (SPI1和所有GPIO端口)
    RCC_APB2PeriphClockCmd(FLASH_SPI_CLK | CS_Config.rcc_clk | 
                          SCK_Config.rcc_clk | MISO_Config.rcc_clk | 
                          MOSI_Config.rcc_clk, ENABLE);
    
    // 2. 配置CS引脚 (推挽输出)
    gpioInit.GPIO_Pin = CS_Config.pin;
    gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CS_Config.port, &gpioInit);
    
    // 3. 配置SCK和MOSI引脚 (复用推挽输出)
    gpioInit.GPIO_Pin = SCK_Config.pin | MOSI_Config.pin;
    gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;  // 复用推挽输出
    GPIO_Init(SCK_Config.port, &gpioInit);
    
    // 4. 配置MISO引脚 (浮空输入)
    gpioInit.GPIO_Pin = MISO_Config.pin;
    gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // 浮空输入模式
    GPIO_Init(MISO_Config.port, &gpioInit);
    
    // 5. 初始CS高电平(不选中芯片)
    CS_HIGH();
    
    // 6. 配置SPI参数
    spiInit.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // 双线全双工
    spiInit.SPI_Mode = SPI_Mode_Master;         // 主机模式
    spiInit.SPI_DataSize = SPI_DataSize_8b;     // 8位数据
    spiInit.SPI_CPOL = SPI_CPOL_High;           // 时钟极性:空闲时高电平
    spiInit.SPI_CPHA = SPI_CPHA_2Edge;          // 时钟相位:第二个边沿采样
    spiInit.SPI_NSS = SPI_NSS_Soft;             // 软件控制NSS
    spiInit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // SPI时钟=系统时钟/4
    spiInit.SPI_FirstBit = SPI_FirstBit_MSB;    // 高位在前
    spiInit.SPI_CRCPolynomial = 7;              // CRC多项式(未使用)
    SPI_Init(FLASH_SPI, &spiInit);
    
    // 7. 使能SPI外设
    SPI_Cmd(FLASH_SPI, ENABLE);
    
    FLASH_DEBUG("SPI Flash initialized");
}

/**
  * @brief  读取设备JEDEC ID
  * @retval 24位设备ID
  */
uint32_t SPI_Flash_ReadID(void)
{
    uint32_t id = 0;
    
    CS_LOW();
    SPI_TransferByte(JEDEC_DEVICE_ID);  // 发送JEDEC ID命令
    id = SPI_TransferByte(DUMMY_BYTE) << 16; // 制造商ID
    id |= SPI_TransferByte(DUMMY_BYTE) << 8; // 存储器类型
    id |= SPI_TransferByte(DUMMY_BYTE);      // 容量标识
    CS_HIGH();
    
    FLASH_DEBUG("Device ID: 0x%06X", id);
    return id;
}

/**
  * @brief  擦除扇区 (4KB)
  * @param  sectorAddr: 扇区内的任意地址
  */
void SPI_Flash_EraseSector(uint32_t sectorAddr)
{
    // 确保地址对齐到4KB边界
    sectorAddr = sectorAddr & ~(SPI_FLASH_SectorSize - 1);
    
    // 发送写使能命令
    SPI_Flash_WriteEnable();
    
    // 发送扇区擦除命令
    CS_LOW();
    SPI_TransferByte(SECTOR_ERASE);  // 扇区擦除命令(0x20)
    SPI_SendAddress(sectorAddr);     // 发送24位地址
    CS_HIGH();
    
    // 等待擦除完成
    SPI_Flash_WaitBusy();
    
    FLASH_DEBUG("Sector 0x%06X erased", sectorAddr);
}

/**
  * @brief  页编程 (最大256字节)
  * @param  buffer: 数据缓冲区
  * @param  addr: 写入地址(必须页对齐)
  * @param  size: 写入字节数(不能超过页边界)
  */
void SPI_Flash_WritePage(uint8_t* buffer, uint32_t addr, uint16_t size)
{
    // 确保写入不跨页
    uint16_t pageOffset = addr % SPI_FLASH_PageSize;
    if (size > (SPI_FLASH_PageSize - pageOffset)) {
        size = SPI_FLASH_PageSize - pageOffset;
        FLASH_ERROR("Write size truncated to %d bytes", size);
    }
    
    // 发送写使能命令
    SPI_Flash_WriteEnable();
    
    // 发送页编程命令
    CS_LOW();
    SPI_TransferByte(PAGE_PROGRAM);  // 页编程命令(0x02)
    SPI_SendAddress(addr);           // 发送24位地址
    
    // 写入数据
    for (uint16_t i = 0; i < size; i++) {
        SPI_TransferByte(buffer[i]);
    }
    
    CS_HIGH();
    
    // 等待写入完成
    SPI_Flash_WaitBusy();
    
    FLASH_DEBUG("Written %d bytes to 0x%06X", size, addr);
}

/**
  * @brief  读取数据
  * @param  buffer: 数据缓冲区
  * @param  addr: 读取起始地址
  * @param  size: 读取字节数
  */
void SPI_Flash_ReadBuffer(uint8_t* buffer, uint32_t addr, uint16_t size)
{
    CS_LOW();
    SPI_TransferByte(READ_DATA);  // 标准读命令(0x03)
    SPI_SendAddress(addr);        // 发送24位地址
    
    // 连续读取数据
    for (uint16_t i = 0; i < size; i++) {
        buffer[i] = SPI_TransferByte(DUMMY_BYTE);
    }
    
    CS_HIGH();
    
    FLASH_DEBUG("Read %d bytes from 0x%06X", size, addr);
}

/**
  * @brief  全片擦除
  * @note   耗时较长(典型值几秒)，谨慎使用!
  */
void SPI_Flash_ChipErase(void)
{
    // 发送写使能命令
    SPI_Flash_WriteEnable();
    
    CS_LOW();
    SPI_TransferByte(CHIP_ERASE);  // 整片擦除命令(0xC7)
    CS_HIGH();
    
    // 等待擦除完成
    SPI_Flash_WaitBusy();
    
    FLASH_DEBUG("Chip erase completed");
}

/**
  * @brief  写使能
  * @note   在执行写/擦除操作前必须调用
  */
void SPI_Flash_WriteEnable(void)
{
    CS_LOW();
    SPI_TransferByte(WRITE_ENABLE);  // 写使能命令(0x06)
    CS_HIGH();
}

/**
  * @brief  写禁止
  */
void SPI_Flash_WriteDisable(void)
{
    CS_LOW();
    SPI_TransferByte(WRITE_DISABLE);  // 写禁止命令(0x04)
    CS_HIGH();
}

/**
  * @brief  进入低功耗模式
  */
void SPI_Flash_PowerDown(void)
{
    CS_LOW();
    SPI_TransferByte(POWER_DOWN);  // 掉电命令(0xB9)
    CS_HIGH();
    // 等待命令完成(小延迟)
    for(volatile int i = 0; i < 100; i++);
}

/**
  * @brief  唤醒设备
  */
void SPI_Flash_WakeUp(void)
{
    CS_LOW();
    SPI_TransferByte(RELEASE_POWER_DOWN);  // 唤醒命令(0xAB)
    CS_HIGH();
    // 等待芯片唤醒(典型值3us)
    for(volatile int i = 0; i < 1000; i++);
}

/**
  * @brief  任意位置写入数据 (自动处理页边界)
  * @param  buffer: 数据缓冲区
  * @param  addr: 起始地址
  * @param  size: 写入字节数
  */
void SPI_Flash_WriteBuffer(uint8_t* buffer, uint32_t addr, uint32_t size)
{
    uint32_t currentAddr = addr;
    uint32_t bytesRemaining = size;
    
    while (bytesRemaining > 0) {
        // 计算当前页剩余空间
        uint16_t pageOffset = currentAddr % SPI_FLASH_PageSize;
        uint16_t bytesInPage = SPI_FLASH_PageSize - pageOffset;
        
        // 如果剩余数据小于页剩余空间，则全部写入
        if (bytesInPage > bytesRemaining) {
            bytesInPage = bytesRemaining;
        }
        
        // 写入当前页
        SPI_Flash_WritePage(buffer, currentAddr, bytesInPage);
        
        // 更新指针和计数器
        buffer += bytesInPage;
        currentAddr += bytesInPage;
        bytesRemaining -= bytesInPage;
    }
    
    FLASH_DEBUG("Written %lu bytes starting from 0x%06X", size, addr);
}

```

### 3.3 主函数测试

```c
#include "stm32f10x.h"
#include "SysTick.h"
#include "usart.h"
#include "spi_flash.h"
#include <stdio.h>
#include <string.h>

// 测试数据缓冲区
#define TEST_DATA_SIZE 256
uint8_t write_buffer[TEST_DATA_SIZE];
uint8_t read_buffer[TEST_DATA_SIZE];

// 测试地址 (使用第一个扇区的起始地址)
#define TEST_ADDRESS 0x000000

/**
  * @brief  填充测试数据
  * @param  buffer: 数据缓冲区
  * @param  size: 数据大小
  */
void fill_test_data(uint8_t* buffer, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        // 生成可读性较好的测试数据
        buffer[i] = (i % 26) + 'A';  // 循环填充A-Z
    }
}

/**
  * @brief  比较两个缓冲区数据
  * @param  buf1: 缓冲区1
  * @param  buf2: 缓冲区2
  * @param  size: 比较大小
  * @retval 0: 相等, 其他: 不相等的位置
  */
int compare_buffers(uint8_t* buf1, uint8_t* buf2, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        if (buf1[i] != buf2[i]) {
            return i + 1;  // 返回不相等的位置(从1开始)
        }
    }
    return 0;
}

int main(void) 
{
    uint32_t flash_id;
    int compare_result;
    
    // 初始化外设
    SysTick_Init();
    USARTx_Init();
    
    printf("\r\n===== SPI Flash Function Test =====\r\n");
    
    // 初始化SPI Flash
    printf("Initializing SPI Flash...\r\n");
    SPI_Flash_Init();
    printf("SPI Flash initialized successfully!\r\n");
    
    // 测试1: 读取设备ID
    printf("\r\n[Test 1] Reading Device ID...\r\n");
    flash_id = SPI_Flash_ReadID();
    printf("Device ID: 0x%06lX\r\n", flash_id);
    
    // 检查ID是否正确 (W25Q64的ID是0xEF4017)
    if (flash_id == sFLASH_ID) {
        printf("Device ID verification successful!\r\n");
    } else {
        printf("Error: Device ID mismatch!\r\n");
    }
    
    // 测试2: 扇区擦除
    printf("\r\n[Test 2] Erasing Sector (0x%06lX)...\r\n", TEST_ADDRESS);
    SPI_Flash_EraseSector(TEST_ADDRESS);
    printf("Sector erase completed!\r\n");
    
    // 测试3: 写入数据
    printf("\r\n[Test 3] Writing test data...\r\n");
    
    // 填充测试数据
    fill_test_data(write_buffer, TEST_DATA_SIZE);
    
    // 写入数据到Flash
    SPI_Flash_WriteBuffer(write_buffer, TEST_ADDRESS, TEST_DATA_SIZE);
    printf("Written %d bytes of data\r\n", TEST_DATA_SIZE);
    
    // 打印部分写入数据
    printf("First 16 bytes of written data: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", write_buffer[i]);
    }
    printf("\r\n");
    
    // 测试4: 读取数据
    printf("\r\n[Test 4] Reading data...\r\n");
    SPI_Flash_ReadBuffer(read_buffer, TEST_ADDRESS, TEST_DATA_SIZE);
    printf("Read %d bytes of data\r\n", TEST_DATA_SIZE);
    
    // 打印部分读取数据
    printf("First 16 bytes of read data: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", read_buffer[i]);
    }
    printf("\r\n");
    
    // 测试5: 数据验证
    printf("\r\n[Test 5] Verifying data...\r\n");
    compare_result = compare_buffers(write_buffer, read_buffer, TEST_DATA_SIZE);
    
    if (compare_result == 0) {
        printf("Data verification successful! Write and read data match.\r\n");
    } else {
        printf("Error: Data mismatch at position: %d\r\n", compare_result);
        printf("Expected: 0x%02X, Actual: 0x%02X\r\n", 
               write_buffer[compare_result-1], 
               read_buffer[compare_result-1]);
    }
    
    // 测试6: 低功耗模式测试
    printf("\r\n[Test 6] Power down mode test...\r\n");
    printf("Entering power down mode...\r\n");
    SPI_Flash_PowerDown();
    Delay_ms(1000);  // 保持1秒低功耗状态
    
    printf("Waking up device...\r\n");
    SPI_Flash_WakeUp();
    Delay_ms(100);  // 等待唤醒完成
    printf("Device awakened successfully!\r\n");
    
    // 最终状态指示
    printf("\r\n===== Test Completed =====\r\n");
    
    while(1) {
        // 主循环中不执行任何操作
    }
}

```

## 4. SPI常见函数（STD库）

### 4.1 SPI配置步骤

#### 4.1.1 使能时钟

```c
RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);  // SPI1时钟
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // GPIO时钟
```

#### 4.1.2 配置GPIO引脚

```c
GPIO_InitTypeDef GPIO_InitStruct;
// SCK/MOSI引脚配置（复用推挽输出）
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;  // PA5=SCK, PA7=MOSI
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;         // 复用推挽输出
GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOA, &GPIO_InitStruct);

// MISO引脚配置（浮空输入）
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;               // PA6=MISO
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
GPIO_Init(GPIOA, &GPIO_InitStruct);

// NSS引脚配置（软件控制）
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;               // PA4=NSS
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;        // 推挽输出
GPIO_Init(GPIOA, &GPIO_InitStruct);
GPIO_SetBits(GPIOA, GPIO_Pin_4);                     // 初始高电平
```

#### 4.1.3 SPI参数配置

```c
SPI_InitTypeDef SPI_InitStruct;
SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // 全双工
SPI_InitStruct.SPI_Mode = SPI_Mode_Master;           // 主模式
SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;       // 8位数据
SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;              // 时钟极性
SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;            // 时钟相位
SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;               // 软件NSS控制
SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // 18MHz
SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;      // 高位在前
SPI_InitStruct.SPI_CRCPolynomial = 7;                // CRC多项式

SPI_Init(SPI1, &SPI_InitStruct);  // 应用配置
SPI_Cmd(SPI1, ENABLE);            // 使能SPI
```

### 4.2 核心操作函数

#### 4.2.1 发送/接收数据

```c
// 阻塞式发送单字节
void SPI_SendByte(SPI_TypeDef* SPIx, uint8_t data) {
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET); // 等待发送缓冲区空
    SPI_I2S_SendData(SPIx, data);
}

// 阻塞式接收单字节
uint8_t SPI_ReceiveByte(SPI_TypeDef* SPIx) {
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET); // 等待接收完成
    return SPI_I2S_ReceiveData(SPIx);
}

// 全双工传输（发送同时接收）
uint8_t SPI_TransmitReceive(SPI_TypeDef* SPIx, uint8_t txData) {
    SPI_SendByte(SPIx, txData);
    return SPI_ReceiveByte(SPIx);
}
```

#### 4.2.2 多字节传输

```c
void SPI_SendBuffer(SPI_TypeDef* SPIx, uint8_t* pBuffer, uint16_t len) {
    while (len--) {
        SPI_SendByte(SPIx, *pBuffer++);
    }
}

void SPI_ReceiveBuffer(SPI_TypeDef* SPIx, uint8_t* pBuffer, uint16_t len) {
    while (len--) {
        *pBuffer++ = SPI_ReceiveByte(SPIx);
    }
}
```

### 4.3 片选控制

```c
// 软件控制片选
void SPI_CS_Enable(void) {
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);  // 拉低片选
}

void SPI_CS_Disable(void) {
    GPIO_SetBits(GPIOA, GPIO_Pin_4);    // 拉高片选
}

// 使用示例
SPI_CS_Enable();
SPI_SendByte(SPI1, 0xAA);  // 发送命令
SPI_CS_Disable();
```

### 4.4 中断配置

#### 4.4.1 使能中断

```c
// 使能TXE（发送缓冲区空中断）和RXNE（接收缓冲区非空中断）
SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE | SPI_I2S_IT_RXNE, ENABLE);

// 配置NVIC
NVIC_InitTypeDef NVIC_InitStruct;
NVIC_InitStruct.NVIC_IRQChannel = SPI1_IRQn;
NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStruct);
```

#### 4.4.2 中断服务函数

```c
volatile uint8_t spi_tx_complete = 0;
volatile uint8_t spi_rx_complete = 0;

void SPI1_IRQHandler(void) {
    // 发送缓冲区空中断
    if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_TXE) != RESET) {
        // 填充发送数据
        static uint8_t tx_data = 0;
        SPI_I2S_SendData(SPI1, tx_data++);
        if (tx_data == 0) {
            SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, DISABLE); // 关闭发送中断
            spi_tx_complete = 1;
        }
    }
    
    // 接收缓冲区非空中断
    if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_RXNE) != RESET) {
        uint8_t rx_data = SPI_I2S_ReceiveData(SPI1);
        // 处理接收数据...
        spi_rx_complete = 1;
    }
}
```

### 4.5 DMA 配置

#### 4.5.1 使能SPI DMA请求

```c
// 使能SPI发送DMA请求
SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);

// 使能SPI接收DMA请求
SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
```

#### 4.5.2 配置DMA通道

```c
// SPI1_TX使用DMA1通道3
DMA_InitTypeDef DMA_InitStruct;
DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR;
DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)tx_buffer;
DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;  // 内存->外设
DMA_InitStruct.DMA_BufferSize = BUFFER_SIZE;
// ...其他DMA配置
DMA_Init(DMA1_Channel3, &DMA_InitStruct);
```
