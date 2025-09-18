# 第二十章 SPI介绍及应用

## 1. SPI简介

### 1.1 基本特点

| 特性         | 说明                  |
| ---------- | ------------------- |
| **通信方式**   | 同步串行通信（由时钟同步）       |
| **数据方向**   | 全双工（可同时发送与接收）       |
| **通信速度**   | 高速（可达几 MHz 到几十 MHz） |
| **连接方式**   | 主从架构，支持多从机          |
| **信号线**    | 至少 4 根线（典型四线制）      |
| **无地址机制**  | 使用片选（CS）选择从设备       |
| **无固定协议层** | 灵活性高，由开发者自定义数据格式    |

### 1.2 SPI 四大信号线（4线制标准）

SPI 通常使用以下 **4 条信号线**：

| 信号           | 名称                                          | 方向    | 说明              |
| ------------ | ------------------------------------------- | ----- | --------------- |
| **SCLK**     | Serial Clock<br>（也称 SCK）                    | 主 → 从 | 由主设备产生，同步数据传输   |
| **MOSI**     | Master Out, Slave In                        | 主 → 从 | 主设备发送，从设备接收的数据线 |
| **MISO**     | Master In, Slave Out                        | 从 → 主 | 从设备发送，主设备接收的数据线 |
| **CS / NSS** | Chip Select / Slave Select<br>（也称 SS 或 nCS） | 主 → 从 | 低电平有效，用于选择目标从设备 |

> ✅ 注意：
> 
> - 所有信号均为**推挽输出**，无需上拉电阻。
> - CS 可以是多个（每个从设备一根），也可以通过译码器管理。

### 1.3 SPI 的四种工作模式（Clock Polarity & Phase）

SPI 有 **4 种工作模式**，由两个参数决定：

| 参数       | 缩写             | 含义                        |
| -------- | -------------- | ------------------------- |
| **CPOL** | Clock Polarity | 时钟空闲状态（0: 低电平，1: 高电平）     |
| **CPHA** | Clock Phase    | 数据采样边沿（0: 第一个边沿，1: 第二个边沿） |

| 模式         | CPOL | CPHA | 采样边沿     | 更新边沿 | 常见设备             |
| ---------- | ---- | ---- | -------- | ---- | ---------------- |
| **Mode 0** | 0    | 0    | SCLK 上升沿 | 下降沿  | 多数设备（如nRF24L01）  |
| **Mode 1** | 0    | 1    | SCLK 下降沿 | 上升沿  | 少数传感器            |
| **Mode 2** | 1    | 0    | SCLK 下降沿 | 上升沿  | 一些Flash          |
| **Mode 3** | 1    | 1    | SCLK 上升沿 | 下降沿  | MAX6675、部分EEPROM |

## 2. SPI应用示例

### 2.1 SPI初始化

```c
#include "spi.h"
#include "usart.h"
#include <stdio.h>

SPI_HandleTypeDef spi_handle;

void spi_init(void)
{
    __HAL_RCC_SPI1_CLK_ENABLE();
    spi_handle.Instance = SPI1;
    spi_handle.Init.Mode = SPI_MODE_MASTER; // 设置SPI工作模式为主模式
    spi_handle.Init.Direction = SPI_DIRECTION_2LINES; // 设置SPI数据传输方向，2线模式
    spi_handle.Init.DataSize = SPI_DATASIZE_8BIT; // 设置SPI的数据大小为8位
    spi_handle.Init.CLKPolarity = SPI_POLARITY_HIGH; // 时钟极性为高
    spi_handle.Init.CLKPhase = SPI_PHASE_2EDGE; // 第二个时钟沿采样
    spi_handle.Init.NSS = SPI_NSS_SOFT; // 设置NSS信号由硬件管理
    spi_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; // 设置波特率预分频值为256
    spi_handle.Init.TIMode = SPI_TIMODE_DISABLED; // 禁用TI模式
    spi_handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED; // 禁用CRC计算
    spi_handle.Init.CRCPolynomial = 7; // 设置CRC计算多项式为7
    __HAL_SPI_ENABLE(&spi_handle);
    spi1_read_write_byte(0xFF);
    printf("SPI init success!\r\n");
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if(hspi->Instance==SPI1)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        // SPI1_SCK PB3 SPI1_MISO PB4 SPI1_MOSI PB5
        GPIO_InitStruct.Pin = GPIO_PIN_3; // SCK复用输出
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        GPIO_InitStruct.Pin = GPIO_PIN_4; // MISO复用输输出
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        GPIO_InitStruct.Pin = GPIO_PIN_5; // MOSI复用输出
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}
/**
 * @brief       SPI1速度设置函数
 *   @note      SPI1时钟选择来自APB1, 即PCLK1, 为 42MHz
 *              SPI速度 = PCLK1 / 2^(speed + 1)
 * @param       speed   : SPI1时钟分频系数
                        取值为SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
 * @retval      无
 */
void spi1_set_speed(uint8_t speed)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(speed));// 判断有效性 
    __HAL_SPI_DISABLE(&spi_handle);                // 关闭SPI 
    spi_handle.Instance->CR1 &= 0XFFC7;            //位3-5清零，用来设置波特率 
    spi_handle.Instance->CR1 |= speed << 3;        //设置SPI速度
    __HAL_SPI_ENABLE(&spi_handle);                 // 使能SPI 
}

uint8_t spi1_read_write_byte(uint8_t txdata)
{
    uint8_t rxdata;
    HAL_SPI_TransmitReceive(&spi_handle, &txdata, &rxdata, 1, 1000);
    return rxdata;
}
```

### 2.2 flash相关参数宏定义

```c
#ifndef __FLASH_H
#define __FLASH_H

#include "sys.h"

/* FLASH芯片列表 */
#define W25Q80      0XEF13          /* W25Q80   芯片ID */
#define W25Q16      0XEF14          /* W25Q16   芯片ID */
#define W25Q32      0XEF15          /* W25Q32   芯片ID */
#define W25Q64      0XEF16          /* W25Q64   芯片ID */
#define W25Q128     0XEF17          /* W25Q128  芯片ID */
#define W25Q256     0XEF18          /* W25Q256  芯片ID */
#define BY25Q64     0X6816          /* BY25Q64  芯片ID */
#define BY25Q128    0X6817          /* BY25Q128 芯片ID */
#define NM25Q64     0X5216          /* NM25Q64  芯片ID */
#define NM25Q128    0X5217          /* NM25Q128 芯片ID */

/* 指令表 */
#define FLASH_WriteEnable           0x06 
#define FLASH_WriteDisable          0x04 
#define FLASH_ReadStatusReg1        0x05 
#define FLASH_ReadStatusReg2        0x35 
#define FLASH_ReadStatusReg3        0x15 
#define FLASH_WriteStatusReg1       0x01 
#define FLASH_WriteStatusReg2       0x31 
#define FLASH_WriteStatusReg3       0x11 
#define FLASH_ReadData              0x03 
#define FLASH_FastReadData          0x0B 
#define FLASH_FastReadDual          0x3B 
#define FLASH_FastReadQuad          0xEB  
#define FLASH_PageProgram           0x02 
#define FLASH_PageProgramQuad       0x32 
#define FLASH_BlockErase            0xD8 
#define FLASH_SectorErase           0x20 
#define FLASH_ChipErase             0xC7 
#define FLASH_PowerDown             0xB9 
#define FLASH_ReleasePowerDown      0xAB 
#define FLASH_DeviceID              0xAB 
#define FLASH_ManufactDeviceID      0x90 
#define FLASH_JedecDeviceID         0x9F 
#define FLASH_Enable4ByteAddr       0xB7
#define FLASH_Exit4ByteAddr         0xE9
#define FLASH_SetReadParam          0xC0 
#define FLASH_EnterQPIMode          0x38
#define FLASH_ExitQPIMode           0xFF

extern uint16_t myflash_type;      /* 定义FLASH芯片型号 */

/* CS引脚控制 */
#define FLASH_CS(x) do{ x ? \
                        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET) : \
                        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); \
                    }while(0)

/* 函数声明 */
static void flash_wait_busy(void);
void flash_write_enable(void);
static void flash_send_address(uint32_t address);
uint8_t flash_read_sr(uint8_t regno);
void flash_write_sr(uint8_t regno, uint8_t sr);
uint16_t flash_read_id(void);
void flash_read(uint8_t *pbuf, uint32_t addr, uint16_t datalen);
static void flash_write_page(uint8_t *pbuf, uint32_t addr, uint16_t datalen);
static void flash_write_nocheck(uint8_t *pbuf, uint32_t addr, uint16_t datalen);
void flash_write(uint8_t *pbuf, uint32_t addr, uint16_t datalen);
void flash_erase_chip(void);
void flash_erase_sector(uint32_t saddr);
void spi_flash_init(void);

#endif /* __FLASH_H */
```

### 2.3 flash初始化

```c
// SPI FLASH GPIO Configuration
void spi_flash_init(void)
{
    uint8_t temp;
    __HAL_RCC_GPIOB_CLK_ENABLE(); // CS-PB14
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = GPIO_PIN_14;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
    FLASH_CS(0); // 取消片选
    spi_init(); // 初始化SPI
    spi1_set_speed(SPI_SPEED_4); // 设置SPI速度
    myflash_type = flash_read_id();
    if(myflash_type == W25Q256)
    {
        temp = flash_read_sr(3); // 读取状态寄存器3，判断地址模式
        if((temp & 0x01) == 0) // 如果不受4字节地址模式，进入4字节地址模式
        {
            flash_write_enable();
            temp |= 1<<1;
            flash_write_sr(3, temp);
            FLASH_CS(0);
            spi1_read_write_byte(FLASH_Enable4ByteAddr); // 发送4字节地址使能命令
            FLASH_CS(1);
        }
    }
    printf("SPI_FLASH Init Success\r\n");
}
```

### 2.4 flash相关功能函数

```c
#include "flash.h"
#include "delay.h"
#include "usart.h"
#include "spi.h"

uint16_t myflash_type = W25Q128;

static void flash_wait_busy(void)
{
    while((flash_read_sr(1) & 0x01) == 0x01); // 等待BUSY位清空
}

// 25QXX写使能-将S1寄存器的WEL置位
void flash_write_enable(void)
{
    FLASH_CS(0);
    spi1_read_write_byte(FLASH_WriteEnable); // 发送写使能
    FLASH_CS(1);
}

/**
 * @brief       25QXX发送地址
 *   @note      根据芯片型号的不同, 发送24ibt / 32bit地址
 * @param       address : 要发送的地址
 * @retval      无
 */
static void flash_send_address(uint32_t address)
{
    if(myflash_type == W25Q256) // 只有W25Q256支持4字节地址模式
    {
        spi1_read_write_byte((uint8_t)((address)>>24)); // 发送 bit31 ~ bit24 地址 
    } 
    spi1_read_write_byte((uint8_t)((address)>>16)); // 发送 bit23 ~ bit16 地址 
    spi1_read_write_byte((uint8_t)((address)>>8));  // 发送 bit15 ~ bit8  地址 
    spi1_read_write_byte((uint8_t)address);         // 发送 bit7  ~ bit0  地址 
}

/**
 * @brief       读取25QXX的状态寄存器，25QXX一共有3个状态寄存器
 *   @note      状态寄存器1：
 *              BIT7  6   5   4   3   2   1   0
 *              SPR   RV  TB BP2 BP1 BP0 WEL BUSY
 *              SPR:默认0,状态寄存器保护位,配合WP使用
 *              TB,BP2,BP1,BP0:FLASH区域写保护设置
 *              WEL:写使能锁定
 *              BUSY:忙标记位(1,忙;0,空闲)
 *              默认:0x00
 *
 *              状态寄存器2：
 *              BIT7  6   5   4   3   2   1   0
 *              SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
 *
 *              状态寄存器3：
 *              BIT7      6    5    4   3   2   1   0
 *              HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
 *
 * @param       regno: 状态寄存器号，范:1~3
 * @retval      状态寄存器值
 */
uint8_t flash_read_sr(uint8_t regno)
{
    uint8_t byte = 0, command = 0;
    switch (regno)
    {
        case 1:
            command = FLASH_ReadStatusReg1; // 读状态寄存器1指令
            break;
        case 2:
            command = FLASH_ReadStatusReg2; // 读状态寄存器2指令 
            break;
        case 3:
            command = FLASH_ReadStatusReg3; // 读状态寄存器3指令 
            break;
        default:
            command = FLASH_ReadStatusReg1;
            break;
    }
    FLASH_CS(0);
    spi1_read_write_byte(command);    // 发送读寄存器命令
    byte = spi1_read_write_byte(0Xff);// 读取一个字节 
    FLASH_CS(1);
    return byte;
}

/**
 * @brief       写25QXX状态寄存器
 *   @note      寄存器说明见norflash_read_sr函数说明
 * @param       regno: 状态寄存器号，范:1~3
 * @param       sr   : 要写入状态寄存器的值
 * @retval      无
 */
void flash_write_sr(uint8_t regno, uint8_t sr)
{
    uint8_t command = 0;
    switch (regno)
    {
        case 1:
            command = FLASH_WriteStatusReg1; // 写状态寄存器1指令 
            break;
        case 2:
            command = FLASH_WriteStatusReg2; // 写状态寄存器2指令 
            break;
        case 3:
            command = FLASH_WriteStatusReg3; // 写状态寄存器3指令 
            break;
        default:
            command = FLASH_WriteStatusReg1;
            break;
    }
    FLASH_CS(0);
    spi1_read_write_byte(command);  
    spi1_read_write_byte(sr); // 写入一个字节
    FLASH_CS(1);
}

/**
 * @brief       读取芯片ID
 * @param       无
 * @retval      FLASH芯片ID
 *   @note      芯片ID列表见: flash.h, 芯片列表部分
 */
// 修改后完整函数
uint16_t flash_read_id(void)
{
    uint16_t deviceid;
    FLASH_CS(0);
    spi1_read_write_byte(FLASH_JedecDeviceID); // 0x9F
    deviceid = spi1_read_write_byte(0xFF) << 8;   // 制造商ID
    deviceid |= spi1_read_write_byte(0xFF);       // 设备ID高字节
    spi1_read_write_byte(0xFF);                  // 消耗容量字节
    FLASH_CS(1);
    return deviceid;
}

/**
 * @brief       读取SPI FLASH
 *   @note      在指定地址开始读取指定长度的数据
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始读取的地址(最大32bit)
 * @param       datalen : 要读取的字节数(最大65535)
 * @retval      无
 */
void flash_read(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t i;
    FLASH_CS(0);
    spi1_read_write_byte(FLASH_ReadData); // 发送读取命令
    flash_send_address(addr);          // 发送地址
    for (i = 0; i < datalen; i++)
    {
        pbuf[i] = spi1_read_write_byte(0XFF);
    }
    FLASH_CS(1);
}

/**
 * @brief       SPI在一页(0~65535)内写入少于256个字节的数据
 *   @note      在指定地址开始写入最大256字节的数据
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始写入的地址(最大32bit)
 * @param       datalen : 要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
 * @retval      无
 */
static void flash_write_page(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t i;
    flash_write_enable();                   
    FLASH_CS(0);
    spi1_read_write_byte(FLASH_PageProgram); // 发送写页命令
    flash_send_address(addr);               
    for (i = 0; i < datalen; i++)
    {
        spi1_read_write_byte(pbuf[i]);         
    }
    FLASH_CS(1);
    flash_wait_busy(); // 等待写入结束
}

/**
 * @brief       无检验写SPI FLASH
 *   @note      必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
 *              具有自动换页功能
 *              在指定地址开始写入指定长度的数据,但是要确保地址不越界!
 *
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始写入的地址(最大32bit)
 * @param       datalen : 要写入的字节数(最大65535)
 * @retval      无
 */
static void flash_write_nocheck(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t pageremain;
    pageremain = 256 - addr % 256; // 单页剩余的字节数 
    if (datalen <= pageremain)     // 不大于256个字节 
    {
        pageremain = datalen;
    }
    while (1)
    {
        /* 当写入字节比页内剩余地址还少的时候, 一次性写完
         * 当写入直接比页内剩余地址还多的时候, 先写完整个页内剩余地址, 然后根据剩余长度进行不同处理
         */
        flash_write_page(pbuf, addr, pageremain);
        if (datalen == pageremain) // 写入结束了
        {
            break;
        }
        else // 当前页写不完, 继续写下一页
        {
            pbuf += pageremain;       // pbuf指针地址偏移,前面已经写了pageremain字节 
            addr += pageremain;       // 写地址偏移,前面已经写了pageremain字节 
            datalen -= pageremain;    // 写入总长度减去已经写入了的字节数 
            if (datalen > 256)        //  剩余数据还大于一页,可以一次写一页 
            {
                pageremain = 256;     // 一次可以写入256个字节 
            }
            else                      // 剩余数据小于一页,可以一次写完 
            {
                pageremain = datalen; // 不够256个字节了
            }
        }
    }
}

/**
 * @brief       写SPI FLASH
 *   @note      在指定地址开始写入指定长度的数据 , 该函数带擦除操作!
 *              SPI FLASH 一般是: 256个字节为一个Page, 4Kbytes为一个Sector, 16个扇区为1个Block
 *              擦除的最小单位为Sector.
 *
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始写入的地址(最大32bit)
 * @param       datalen : 要写入的字节数(最大65535)
 * @retval      无
 */
uint8_t g_norflash_buf[4096];   /* 扇区缓存 */
void flash_write(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;
    uint8_t *norflash_buf;
    norflash_buf = g_norflash_buf;
    secpos = addr / 4096;       // 扇区地址 
    secoff = addr % 4096;       // 在扇区内的偏移 
    secremain = 4096 - secoff;  // 扇区剩余空间大小 
    //printf("ad:%X,nb:%X\r\n", addr, datalen); /* 测试用 */
    if (datalen <= secremain)
    {
        secremain = datalen; // 不大于4096个字节
    }
    while (1)
    {
        flash_read(norflash_buf, secpos * 4096, 4096); // 读出整个扇区的内容 
        for (i = 0; i < secremain; i++) // 校验数据
        {
            if (norflash_buf[secoff + i] != 0XFF)
            {
                break;  // 需要擦除, 直接退出for循环 
            }
        }
        if (i < secremain) // 需要擦除 
        {
            flash_erase_sector(secpos); // 擦除这个扇区 
            for (i = 0; i < secremain; i++) 
            {
                norflash_buf[i + secoff] = pbuf[i];
            }
            flash_write_nocheck(norflash_buf, secpos * 4096, 4096); // 写入整个扇区
        }
        else // 写已经擦除了的,直接写入扇区剩余区间.
        {
            flash_write_nocheck(pbuf, addr, secremain);             
        }
        if (datalen == secremain)
        {
            break; // 写入结束了
        }
        else // 写入未结束 
        {
            secpos++;             // 扇区地址增1 
            secoff = 0;           // 偏移位置为0 
            pbuf += secremain;    // 指针偏移
            addr += secremain;    // 写地址偏移
            datalen -= secremain; // 字节数递减 
            if (datalen > 4096)
            {
                secremain = 4096; // 下一个扇区还是写不完 
            }
            else
            {
                secremain = datalen; // 下一个扇区可以写完了 
            }
        }
    }
}

// 擦除整个芯片
void flash_erase_chip(void)
{
    flash_write_enable();   
    flash_wait_busy();      
    FLASH_CS(0);
    spi1_read_write_byte(FLASH_ChipErase); 
    FLASH_CS(1);
    flash_wait_busy();      
}

/**
 * @brief       擦除一个扇区
 *   @note      注意,这里是扇区地址,不是字节地址!!
 *              擦除一个扇区的最少时间:150ms
 * 
 * @param       saddr : 扇区地址 根据实际容量设置
 * @retval      无
 */
void flash_erase_sector(uint32_t saddr)
{
    //printf("fe:%x\r\n", saddr);   /* 监视falsh擦除情况,测试用 */
    saddr *= 4096;
    flash_write_enable();      
    flash_wait_busy();           
    FLASH_CS(0);
    spi1_read_write_byte(FLASH_SectorErase);   
    flash_send_address(saddr);  
    FLASH_CS(1);
    flash_wait_busy();      
}
```

### 2.5 主函数测试

```c
#include "bsp_init.h"
#include "flash.h"
#include "spi.h"

const uint8_t text_buf[] = "STM32 SPI TEST";
#define TEXT_SIZE sizeof(text_buf)

int main(void)
{
    uint8_t key_value;
    uint16_t i = 0;
    uint8_t data_buf[TEXT_SIZE];
    uint32_t flash_size;
    bsp_init();   
    spi_flash_init();
    LCD_ShowString(30,110,200,16,16,"KEY1:Write  KEY0:Read");
    LCD_ShowString(30,130,200,16,16,"SPI FLASH Ready!");
    while(1)
    {
        key_value = key_scan(0);
        if(key_value == KEY1_Press)
        {
            LCD_ShowString(30,150,200,16,16,"Start Write FLASH....");
            sprintf((char*)data_buf, "%s%d", (char*)text_buf, i);
            flash_write((uint8_t*)data_buf, flash_size-100, TEXT_SIZE);
            LCD_ShowString(30,150,200,16,16,"FLASH Write Finished!");
        }
        if(key_value == KEY0_Press)
        {
            LCD_ShowString(30,150,200,16,16,"Start Read FLASH... .");
            flash_read(data_buf, flash_size-100, TEXT_SIZE);
            LCD_ShowString(30,150,200,16,16,"FLASH Read Finished! ");
            LCD_ShowString(30,170,200,16,16,(char*)data_buf);
        }
        i++;
        if(i == 20)
        {
            LED_TOGGLE(LED1_GPIO_Pin);
            i = 0;
        }
        delay_ms(10); 
    }
}
```

## 3. SPI常见函数（HAL库）

### 3.1 SPI 初始化与配置

#### 3.1.1 `HAL_SPI_Init()`

```c
HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi);
```

**配置结构体**：

```c
typedef struct {
 SPI_TypeDef *Instance; // SPI实例 (SPI1, SPI2, SPI3)
 uint32_t Mode; // 模式: SPI_MODE_MASTER/SLAVE
 uint32_t Direction; // 方向: 
// SPI_DIRECTION_2LINES (全双工)
 // SPI_DIRECTION_2LINES_RXONLY
 // SPI_DIRECTION_1LINE (半双工)
 uint32_t DataSize; // 数据大小: SPI_DATASIZE_8BIT/16BIT
 uint32_t CLKPolarity; // 时钟极性: SPI_POLARITY_LOW/HIGH
 uint32_t CLKPhase; // 时钟相位: SPI_PHASE_1EDGE/2EDGE
 uint32_t NSS; // 片选模式: 
// SPI_NSS_SOFT (软件控制)
 // SPI_NSS_HARD_INPUT/OUTPUT
 uint32_t BaudRatePrescaler; // 波特率分频: 
// SPI_BAUDRATEPRESCALER_2/4/...
 uint32_t FirstBit; // 位序: SPI_FIRSTBIT_MSB/LSB
 uint32_t TIMode; // TI模式: SPI_TIMODE_DISABLE
 uint32_t CRCCalculation;// CRC计算: SPI_CRCCALCULATION_DISABLE
 uint32_t CRCPolynomial; // CRC多项式
} SPI_InitTypeDef;
```

**主模式初始化示例**：

```c
SPI_HandleTypeDef hspi1;

void SPI_Init(void) {
 hspi1.Instance = SPI1;
 hspi1.Init.Mode = SPI_MODE_MASTER;
 hspi1.Init.Direction = SPI_DIRECTION_2LINES;
 hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
 hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
 hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
 hspi1.Init.NSS = SPI_NSS_SOFT;
 hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
 hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;

if (HAL_SPI_Init(&hspi1) != HAL_OK) {
 Error_Handler();
 }
}
```

### 3.2 SPI 数据传输函数

#### 3.2.1 阻塞模式

```c
// 发送数据
HAL_StatusTypeDef HAL_SPI_Transmit(
 SPI_HandleTypeDef *hspi,
 uint8_t *pData,
 uint16_t Size,
 uint32_t Timeout);

// 接收数据
HAL_StatusTypeDef HAL_SPI_Receive(
 SPI_HandleTypeDef *hspi,
 uint8_t *pData,
 uint16_t Size,
 uint32_t Timeout);

// 同时发送和接收（最常用）
HAL_StatusTypeDef HAL_SPI_TransmitReceive(
 SPI_HandleTypeDef *hspi,
 uint8_t *pTxData,
 uint8_t *pRxData,
 uint16_t Size,
 uint32_t Timeout);
```

**示例**：

```c
// 发送命令并接收响应
uint8_t txData[2] = {0x90, 0x00}; // 读取ID命令
uint8_t rxData[2];

HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // 拉低片选
HAL_SPI_TransmitReceive(&hspi1, txData, rxData, 2, 100);
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // 释放片选
```

#### 3.2.2 中断模式

```c
// 中断发送
HAL_StatusTypeDef HAL_SPI_Transmit_IT(...);

// 中断接收
HAL_StatusTypeDef HAL_SPI_Receive_IT(...);

// 中断发送接收
HAL_StatusTypeDef HAL_SPI_TransmitReceive_IT(...);
```

#### 3.2.3 DMA模式

```c
// DMA发送
HAL_StatusTypeDef HAL_SPI_Transmit_DMA(...);

// DMA接收
HAL_StatusTypeDef HAL_SPI_Receive_DMA(...);

// DMA发送接收（高效方式）
HAL_StatusTypeDef HAL_SPI_TransmitReceive_DMA(...);
```

### 3.3 SPI 回调函数

```c
// 发送完成回调
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);

// 接收完成回调
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);

// 发送接收完成回调
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);

// 错误回调
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi);
```

**示例实现**：

```c
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
 if(hspi->Instance == SPI1) {
 // 处理SPI1传输完成
 data_ready_flag = 1;
 }
}
```

### 3.4 SPI 中断处理

```c
// SPI中断服务函数
void SPIx_IRQHandler(void) {
 HAL_SPI_IRQHandler(&hspix);
}
```

**示例**：

```c
// stm32f4xx_it.c中
void SPI1_IRQHandler(void) {
 HAL_SPI_IRQHandler(&hspi1);
}
```

### 3.5 SPI 状态管理

#### 3.5.1 状态获取

```c
// 获取SPI状态
HAL_SPI_StateTypeDef HAL_SPI_GetState(SPI_HandleTypeDef *hspi);

// 获取错误代码
uint32_t HAL_SPI_GetError(SPI_HandleTypeDef *hspi);
```

#### 3.5.2 错误代码常量

```c
HAL_SPI_ERROR_NONE // 无错误
HAL_SPI_ERROR_MODF // 模式错误
HAL_SPI_ERROR_CRC // CRC错误
HAL_SPI_ERROR_OVR // 溢出错误
HAL_SPI_ERROR_FRE // 帧格式错误
HAL_SPI_ERROR_DMA // DMA传输错误
HAL_SPI_ERROR_FLAG // 标志错误
```

### 3.6 SPI 高级控制

```c
// 中止传输
HAL_StatusTypeDef HAL_SPI_Abort(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_Abort_IT(SPI_HandleTypeDef *hspi);

// 控制CRC计算
HAL_StatusTypeDef HAL_SPI_EnableCRC(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_DisableCRC(SPI_HandleTypeDef *hspi);
```

---


