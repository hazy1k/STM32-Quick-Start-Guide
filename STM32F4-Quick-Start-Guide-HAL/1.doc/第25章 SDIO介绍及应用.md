# 第二十五章 SDIO介绍及应用

## 1. STM32F4 SDIO简介

### 1.1 主要功能

- **多协议支持：** 完全兼容 MultiMediaCard (MMC) System Specification Version 4.2 及向下兼容，支持 SD 存储卡规范 Version 2.0，以及 SD I/O 卡规范 Version 2.0。此外，还完全支持 CE-ATA 功能，兼容 CE-ATA 数字协议 Version 1.1。
- **多总线宽度：** 支持 1 位（默认）、4 位和 8 位数据总线模式，以适应不同卡片和应用对速度的需求。MMC 卡支持 8 位模式，SD/SDIO 卡支持 4 位模式。
- **高速数据传输：** 最高数据传输速率可达 48MHz (在 8 位模式下)。
- **DMA 支持：** SDIO 外设可与 DMA 控制器无缝集成，实现数据传输的后台操作，显著降低 CPU 占用。
- **硬件处理：** 自动处理命令发送、响应接收、CRC 校验、数据块传输等底层协议细节，简化软件开发。
- **内部 FIFO：** 具有深度为 32 个字（128 字节）的内部发送/接收 FIFO，用于缓冲数据，优化数据流。

### 1.2 时钟

SDIO 的时钟是同步通信的关键。STM32F4 SDIO 的时钟特性：

- **时钟源：** SDIO 外设的时钟源通常来自于 APB2 总线时钟，经过专门的 PLL（锁相环）生成，最终的输入时钟频率通常为 **48MHz**。
- **卡时钟生成：** SDIO 模块内部包含一个时钟分频器，用于将内部 48MHz 时钟分频，生成 SD 卡所需的卡时钟 (SDIO_CLK)。分频因子可配置，以适应不同卡片的时钟要求（例如，初始化阶段通常要求较低频率，数据传输阶段要求较高频率）。
- **时钟控制寄存器 (SDIO_CLKCR)：** 该寄存器用于配置时钟分频器、时钟使能、时钟极性、时钟边缘等。
  - **CLKDIV (位 7:0)：** SDIO 时钟分频器，SDIO_CLK = HCLK/ (CLKDIV + 2)。
  - **CLKEN (位 8)：** 时钟使能。
  - **BYPASS (位 9)：** 时钟旁路使能，如果为 1，SDIO_CLK 等于 HCLK（即不分频，直接使用 48MHz）。
  - **PWRSAV (位 10)：** 省电使能，在总线空闲时关闭时钟。
  - **WIDBUS (位 13:12)：** 总线宽度选择（00：1位，01：4位，10：8位）。
  - **NEGEDGE (位 11)：** 时钟下降沿使能（0：上升沿捕获，1：下降沿捕获）。
  - **HWFLOWEN (位 14)：** 硬件流控制使能。

### 1.3 命令与响应

SDIO 接口的通信基于命令-响应机制。

- **命令 (Command)：** 微控制器发送给 SD/MMC/SDIO 卡的指令，告诉卡片执行何种操作（如读、写、初始化等）。
  - 命令由 **CMD 寄存器 (SDIO_CMD)** 配置并发送。
  - 每个命令都有一个索引（0-63）和一个参数。
  - 命令类型包括广播命令 (Broadcast)、寻址命令 (Addressed) 和数据传输命令 (Data Transfer)。
- **响应 (Response)：** 卡片对命令的回复，指示命令的执行结果或返回必要的信息。
  - 响应由 **响应寄存器 (SDIO_RESPCMD, SDIO_RESP1, SDIO_RESP2, SDIO_RESP3, SDIO_RESP4)** 接收。
  - 响应长度分为短响应（R1、R3、R4、R5、R6、R7，共 48 位）和长响应（R2，共 136 位）。
  - 响应通常包含卡片状态信息或请求的注册数据。

**命令和响应流程：**

1. CPU 将命令参数写入 **SDIO_ARG 寄存器**。
2. CPU 配置 **SDIO_CMD 寄存器**，指定命令索引、响应类型和命令使能位。
3. SDIO 硬件自动发送命令，并等待卡片响应。
4. 当响应到达时，SDIO 硬件自动接收并校验 CRC，并将响应数据存储在 **SDIO_RESPx 寄存器**中。
5. SDIO 硬件更新 **SDIO_STA 状态寄存器**中的相关标志（如 CMDREND：命令响应结束，CMDSENT：命令已发送），并可触发中断。
6. CPU 读取状态寄存器和响应寄存器以获取操作结果。

### 1.4 相关寄存器简介

STM32F4 SDIO 外设的控制和状态通过一系列专用寄存器进行操作。以下是一些关键寄存器的简要介绍：

- **SDIO_POWER (电源控制寄存器)：**
  - **PWRCTRL (位 1:0)：** 电源供应控制，用于开关 SDIO 模块的电源。
- **SDIO_CLKCR (时钟控制寄存器)：**
  - 如上所述，控制 SDIO 时钟分频、使能、总线宽度等。
- **SDIO_ARG (命令参数寄存器)：**
  - **CMDARG (位 31:0)：** 存储将要发送的命令的 32 位参数。
- **SDIO_CMD (命令寄存器)：**
  - **CMDINDEX (位 5:0)：** 命令索引（0-63）。
  - **WAITRESP (位 7:6)：** 期望的响应类型（无响应、短响应、长响应）。
  - **CPSMEN (位 10)：** 命令通道状态机使能。
  - **DDR (位 11)：** 双倍数据速率使能（用于 MMC 4.x）。
- **SDIO_RESPCMD (响应命令寄存器)：**
  - **RESPCMD (位 5:0)：** 存储卡片返回的响应命令索引。
- **SDIO_RESP1-4 (响应数据寄存器 1-4)：**
  - **CARDSTATUS1-4 (位 31:0)：** 存储卡片返回的响应数据。RESP1 存储短响应的大部分，RESP1-4 组合存储长响应。
- **SDIO_DTIMER (数据超时定时器寄存器)：**
  - **DATATIME (位 31:0)：** 数据传输超时计数器值。
- **SDIO_DLEN (数据长度寄存器)：**
  - **DATALENGTH (位 24:0)：** 要传输的数据字节总长度。
- **SDIO_DCTRL (数据控制寄存器)：**
  - **DTEN (位 0)：** 数据传输使能。
  - **DTDIR (位 1)：** 数据传输方向（0：控制器到卡，1：卡到控制器）。
  - **DTMODE (位 2)：** 数据传输模式（0：块模式，1：流模式）。
  - **DMAEN (位 3)：** DMA 使能。
  - **DBLOCKSIZE (位 7:4)：** 数据块大小（2^DBLOCKSIZE 字节）。
  - **RWSTART (位 8)：** 读等待开始。
  - **RWSTOP (位 9)：** 读等待停止。
  - **RWMOD (位 10)：** 读等待模式。
  - **SDIOEN (位 11)：** SDIO 使能（用于 CE-ATA）。
- **SDIO_DCOUNT (数据计数寄存器)：**
  - **DATACOUNT (位 24:0)：** 在数据传输过程中，指示剩余的待传输数据量。
- **SDIO_STA (状态寄存器)：**
  - 包含各种状态标志，如 `CMDACT` (命令正在传输)、`CMDRESPEND` (命令响应结束)、`DTACT` (数据正在传输)、`RXOVERR` (接收溢出)、`TXUNDERR` (发送下溢)、`DATAEND` (数据传输结束) 等。
- **SDIO_ICR (中断清除寄存器)：**
  - 通过写入 1 来清除 `SDIO_STA` 中的相应中断标志。
- **SDIO_FIFO (数据 FIFO 寄存器)：**
  - 这是一个读写寄存器，用于通过 CPU 或 DMA 传输数据。

## 2. SDIO应用示例

### 2.1 相关参数宏定义

```c
#ifndef __SDIO_H
#define __SDIO_H

#include "sys.h"

extern SD_HandleTypeDef sdcard_handle;
extern HAL_SD_CardInfoTypeDef sdcard_info_handle;

/* 超时时间 */
#define SD_TIMEOUT             ((uint32_t)100000000)    
#define SD_TRANSFER_OK         ((uint8_t)0x00)
#define SD_TRANSFER_BUSY       ((uint8_t)0x01)

/* 根据 SD_HandleTypeDef 定义的宏，用于快速计算容量 */
#define SD_TOTAL_SIZE_BYTE(__Handle__)  (((uint64_t)((__Handle__)->SdCard.LogBlockNbr) * ((__Handle__)->SdCard.LogBlockSize)) >> 0)
#define SD_TOTAL_SIZE_KB(__Handle__)    (((uint64_t)((__Handle__)->SdCard.LogBlockNbr) * ((__Handle__)->SdCard.LogBlockSize)) >> 10)
#define SD_TOTAL_SIZE_MB(__Handle__)    (((uint64_t)((__Handle__)->SdCard.LogBlockNbr) * ((__Handle__)->SdCard.LogBlockSize)) >> 20)
#define SD_TOTAL_SIZE_GB(__Handle__)    (((uint64_t)((__Handle__)->SdCard.LogBlockNbr) * ((__Handle__)->SdCard.LogBlockSize)) >> 30)

uint8_t sd_init(void);
uint8_t get_sd_card_info(HAL_SD_CardInfoTypeDef *cardinfo);
uint8_t get_sd_card_state(void);
uint8_t sd_read_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt);
uint8_t sd_write_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt);

#endif /* __SDIO_H */

```

### 2.2 SDIO初始化

```c
#include "sdio.h"

SD_HandleTypeDef sdcard_handle;
HAL_SD_CardInfoTypeDef sdcard_info_handle;

#define  SDIO_TRANSF_CLK_DIV  1   

uint8_t sd_init(void)
{
    uint8_t SD_status;
    sdcard_handle.Instance = SDIO;
    sdcard_handle.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING; // 采样时钟沿上升沿
    sdcard_handle.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE; // 空闲时不关闭时钟电源
    sdcard_handle.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE; // 不使用Bypass模式
    sdcard_handle.Init.BusWide = SDIO_BUS_WIDE_1B; // 1bit模式
    sdcard_handle.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE; // 不使用硬件流控
    /*  
    *  SD传输时钟分频，由于HAL库运行效率低，很容易产生上溢（读SD卡时）/下溢错误（写SD卡时）
    *  使用4bit模式时，需降低SDIO时钟频率，将该宏改为 1，SDIO时钟频率：48/( SDIO_TRANSF_CLK_DIV + 2 ) = 16M * 4bit = 64Mbps 
    *  使用1bit模式时，该宏SDIO_TRANSF_CLK_DIV改为 0，SDIO时钟频率：48/( SDIO_TRANSF_CLK_DIV + 2 ) = 24M * 1bit = 24Mbps 
    */
    sdcard_handle.Init.ClockDiv = SDIO_TRANSF_CLK_DIV;
    SD_status = HAL_SD_Init(&sdcard_handle);
    if(SD_status != HAL_OK)
    {
        return 1;
    }
    HAL_SD_GetCardInfo(&sdcard_handle, &sdcard_info_handle); // 获取SD卡信息
    SD_status = HAL_SD_ConfigWideBusOperation(&sdcard_handle, SDIO_BUS_WIDE_4B); // 开启4bit模式
    {
        return 2;
    }
    return 0;
}
```

### 2.3 SD卡读写

```c
/**
 * @brief       获取卡信息函数
 * @param       cardinfo:SD卡信息句柄
 * @retval      返回值:读取卡信息状态值
 */
uint8_t get_sd_card_info(HAL_SD_CardInfoTypeDef *cardinfo)
{
    uint8_t sta;
    sta = HAL_SD_GetCardInfo(&sdcard_handle, cardinfo);
    return sta;
}

/**
 * @brief       判断SD卡是否可以传输(读写)数据
 * @param       无
 * @retval      返回值:SD_TRANSFER_OK      传输完成，可以继续下一次传输
                       SD_TRANSFER_BUSY SD 卡正忙，不可以进行下一次传输
 */
uint8_t get_sd_card_state(void)
{
    return ((HAL_SD_GetCardState(&sdcard_handle) == HAL_SD_CARD_TRANSFER) ? SD_TRANSFER_OK : SD_TRANSFER_BUSY);
}

/**
 * @brief       读SD卡(fatfs/usb调用)
 * @param       pbuf  : 数据缓存区
 * @param       saddr : 扇区地址
 * @param       cnt   : 扇区个数
 * @retval      0, 正常;  其他, 错误代码(详见SD_Error定义);
 */
uint8_t sd_read_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt)
{
    uint8_t sta = HAL_OK;
    uint32_t timeout = SD_TIMEOUT;
    long long lsector = saddr;
    __disable_irq();                                                                       
    sta = HAL_SD_ReadBlocks(&sdcard_handle, (uint8_t *)pbuf, lsector, cnt, SD_TIMEOUT); /* 多个sector的读操作 */
    /* 等待SD卡读完 */
    while (get_sd_card_state() != SD_TRANSFER_OK)
    {
        if (timeout-- == 0)
        {
            sta = SD_TRANSFER_BUSY;
        }
    }
    __enable_irq(); 
    return sta;
}

/**
 * @brief       写SD卡(fatfs/usb调用)
 * @param       pbuf  : 数据缓存区
 * @param       saddr : 扇区地址
 * @param       cnt   : 扇区个数
 * @retval      0, 正常;  其他, 错误代码(详见SD_Error定义);
 */
uint8_t sd_write_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt)
{
    uint8_t sta = HAL_OK;
    uint32_t timeout = SD_TIMEOUT;
    long long lsector = saddr;  
    __disable_irq();                                                                        
    sta = HAL_SD_WriteBlocks(&sdcard_handle, (uint8_t *)pbuf, lsector, cnt, SD_TIMEOUT); 
    /* 等待SD卡写完 */
    while (get_sd_card_state() != SD_TRANSFER_OK)
    {
        if (timeout-- == 0)
        {
            sta = SD_TRANSFER_BUSY;
        }
    }
    __enable_irq();  
    return sta;
}
```

### 2.4 显示信息\测试读写

```c
// 通过串口打印SD卡相关信息
void show_sd_info(void)
{
  HAL_SD_CardCIDTypeDef CID;
  HAL_SD_GetCardCID(&sdcard_handle, &CID);
  get_sd_card_info(&sdcard_info_handle);
  switch(sdcard_info_handle.CardType)
  {
    case CARD_SDSC:
      if(sdcard_info_handle.CardVersion == CARD_V1_X)
      {
        printf("Card Type:SDSC V1\r\n");
      }
      else if(sdcard_info_handle.CardVersion == CARD_V2_X)
      {
        printf("Card Type:SDSC V2\r\n");
      }
      break;
    case CARD_SDHC_SDXC:
      printf("Card Type:SDHC\r\n");
      break;
    default:
      printf("Unknown Card Type\r\n");
      break;
  }
  printf("Card ManufacturerID:%d\r\n", CID.ManufacturerID);               /* 制造商ID */
  printf("Card RCA:%d\r\n", sdcard_info_handle.RelCardAdd);                    /* 卡相对地址 */
  printf("LogBlockNbr:%d \r\n", (uint32_t)(sdcard_info_handle.LogBlockNbr));   /* 显示逻辑块数量 */
  printf("LogBlockSize:%d \r\n", (uint32_t)(sdcard_info_handle.LogBlockSize)); /* 显示逻辑块大小 */
  printf("Card Capacity:%d MB\r\n", (uint32_t)SD_TOTAL_SIZE_MB(&sdcard_handle));  /* 显示容量 */
  printf("Card BlockSize:%d\r\n\r\n", sdcard_info_handle.BlockSize);           /* 显示块大小 */
}

// 测试SD卡读取,从addr地址开始,读取cnt个扇区的数据
// addr:扇区地址 cnt:扇区数
void test_sd_read(uint32_t addr, uint32_t cnt)
{
  uint32_t i;
  uint8_t *buf;
  uint8_t sta = 0;
  buf = mymalloc(SRAMIN, cnt * 512); // 申请内存
  sta = sd_read_disk(buf, addr, cnt); // 读取数据
  if(sta == 0)
  {
    LCD_ShowString(30,170,200,16,16,"USART1 Sending Data...");
     printf("SECTOR %d DATA:\r\n", addr);
     for(i = 0; i < cnt * 512; i++)
     {
      printf("%x ", buf[i]);
     }
     printf("\r\n Data End!\r\n");
     LCD_ShowString(30,170,200,16,16,"USART1 Send Data Done!");
  }
  else
  {
    printf("error: %d\r\n", sta);
    LCD_ShowString(30,170,200,16,16,"USART1 Send Data Failed!");
  }
  myfree(SRAMIN, buf); // 释放内存
}

// 从addr地址开始,写入cnt个扇区的数据
void test_sd_write(uint32_t addr, uint32_t cnt)
{
  uint32_t i;
  uint8_t *buf;
  uint8_t sta = 0;
  buf = mymalloc(SRAMIN, cnt * 512); // 申请内存
  // 填充数据
  for(i = 0; i < cnt * 512; i++)
  {
    buf[i] = i * 3;
  }
  sta = sd_write_disk(buf, addr, cnt); // 写入数据
  if(sta == 0)
  {
    printf("Write over\r\n");
  }
  else
  {
    printf("error: %d\r\n", sta);
  }
  myfree(SRAMIN, buf);
}
```

### 2.5 主函数测试

```c
#include "bsp_init.h"
#include "sdio.h"

void show_sd_info(void);
void test_sd_read(uint32_t addr, uint32_t cnt);
void test_sd_write(uint32_t addr, uint32_t cnt);

int main(void)
{
  uint32_t i = 0;
  uint8_t key_value;
  bsp_init();
  sd_init();
  show_sd_info();
  LCD_ShowString(30,110,200,16,16,"KEY0:Read Sector 0");
  LCD_ShowString(30,130,200,16,16,"SD Card OK    ");
  LCD_ShowString(30,150,200,16,16,"SD Card Size:     MB");
  LCD_ShowNum(30+13*8, 150, SD_TOTAL_SIZE_MB(&sdcard_handle),5,16);
	while(1)
  {
    key_value = key_scan(0);
    if(key_value == KEY0_Press)
    {
      test_sd_read(0,1);
    }
    i++;
    delay_ms(10);
    if(i == 20)
    {
      LED_TOGGLE(LED0_GPIO_Pin);
      i= 0;
    }
  }
}
```

## 3. SDIO相关函数（HAL库）

### 3.1 SD卡初始化与配置

#### 3.1.1 SD卡句柄结构体

```c
SD_HandleTypeDef hsd;
```

#### 3.1.2 SDIO初始化

```c
HAL_StatusTypeDef HAL_SD_Init(SD_HandleTypeDef *hsd);
HAL_StatusTypeDef HAL_SD_InitCard(SD_HandleTypeDef *hsd);
```

**配置示例**：

```c
void MX_SDIO_SD_Init(void) {
    hsd.Instance = SDIO;
    hsd.Init.ClockEdge           = SDIO_CLOCK_EDGE_RISING;
    hsd.Init.ClockBypass         = SDIO_CLOCK_BYPASS_DISABLE;
    hsd.Init.ClockPowerSave      = SDIO_CLOCK_POWER_SAVE_DISABLE;
    hsd.Init.BusWide             = SDIO_BUS_WIDE_1B;
    hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    hsd.Init.ClockDiv            = SDIO_INIT_CLK_DIV;  // 通常0x76 (400kHz)
    
    if (HAL_SD_Init(&hsd) != HAL_OK) {
        Error_Handler();
    }
    
    // 初始化SD卡
    if (HAL_SD_InitCard(&hsd) != HAL_OK) {
        Error_Handler();
    }
    
    // 配置宽总线模式（初始化后）
    if (HAL_SD_ConfigWideBusOperation(&hsd, SDIO_BUS_WIDE_4B) != HAL_OK) {
        Error_Handler();
    }
}
```

### 3.2 SD卡读写操作

#### 3.2.1 阻塞模式读写

```c
// 读单个块
HAL_StatusTypeDef HAL_SD_ReadBlocks(
  SD_HandleTypeDef *hsd,
  uint8_t *pData,       // 数据缓冲区
  uint32_t BlockAdd,    // 起始块地址
  uint32_t NumberOfBlocks, // 块数量
  uint32_t Timeout      // 超时时间
);

// 写单个块
HAL_StatusTypeDef HAL_SD_WriteBlocks(
  SD_HandleTypeDef *hsd,
  uint8_t *pData,
  uint32_t BlockAdd,
  uint32_t NumberOfBlocks,
  uint32_t Timeout
);

// 读多个块 (DMA优化)
HAL_StatusTypeDef HAL_SD_ReadBlocks_DMA(
  SD_HandleTypeDef *hsd,
  uint8_t *pData,
  uint32_t BlockAdd,
  uint32_t NumberOfBlocks
);

// 写多个块 (DMA优化)
HAL_StatusTypeDef HAL_SD_WriteBlocks_DMA(
  SD_HandleTypeDef *hsd,
  uint8_t *pData,
  uint32_t BlockAdd,
  uint32_t NumberOfBlocks
);
```

#### 3.2.2 中断模式读写

```c
// 读块中断模式
HAL_StatusTypeDef HAL_SD_ReadBlocks_IT(
  SD_HandleTypeDef *hsd,
  uint8_t *pData,
  uint32_t BlockAdd,
  uint32_t NumberOfBlocks
);

// 写块中断模式
HAL_StatusTypeDef HAL_SD_WriteBlocks_IT(
  SD_HandleTypeDef *hsd,
  uint8_t *pData,
  uint32_t BlockAdd,
  uint32_t NumberOfBlocks
);
```

### 3.3 SD卡擦除操作

```c
// 擦除指定块范围
HAL_StatusTypeDef HAL_SD_Erase(
  SD_HandleTypeDef *hsd,
  uint32_t BlockStartAdd,   // 起始块地址
  uint32_t BlockEndAdd      // 结束块地址
);
```

### 3.4 SD卡信息获取

```c
// 获取SD卡信息
HAL_StatusTypeDef HAL_SD_GetCardInfo(
  SD_HandleTypeDef *hsd,
  HAL_SD_CardInfoTypeDef *pCardInfo
);

// 卡信息结构体
typedef struct {
  uint32_t CardType;        // 卡类型: SDIO_STD_CARD, SDIO_HIGH_CAPACITY_CARD
  uint32_t CardVersion;     // 卡版本
  uint32_t Class;           // 卡类别
  uint32_t RelCardAdd;      // 相对卡地址 (RCA)
  uint32_t BlockNbr;        // 总块数
  uint32_t BlockSize;       // 块大小 (字节)
  uint32_t LogBlockNbr;     // 逻辑块数
  uint32_t LogBlockSize;    // 逻辑块大小
} HAL_SD_CardInfoTypeDef;

// 获取卡状态
HAL_StatusTypeDef HAL_SD_GetCardStatus(
  SD_HandleTypeDef *hsd,
  HAL_SD_CardStatusTypeDef *pStatus
);
```

### 3.5 回调函数 (中断/DMA模式)

```c
// 传输完成回调
void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd);
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd);

// 传输错误回调
void HAL_SD_ErrorCallback(SD_HandleTypeDef *hsd);

// 擦除完成回调
void HAL_SD_EraseCpltCallback(SD_HandleTypeDef *hsd);
```

### 3.6 SD卡控制函数

```c
// 配置宽总线模式
HAL_StatusTypeDef HAL_SD_ConfigWideBusOperation(
  SD_HandleTypeDef *hsd,
  uint32_t WideMode  // SDIO_BUS_WIDE_1B, SDIO_BUS_WIDE_4B
);

// 使能/禁用SDIO时钟
void HAL_SD_Write_IT(SD_HandleTypeDef *hsd);
void HAL_SD_Read_IT(SD_HandleTypeDef *hsd);

// 中止传输
HAL_StatusTypeDef HAL_SD_Abort(SD_HandleTypeDef *hsd);
HAL_StatusTypeDef HAL_SD_Abort_IT(SD_HandleTypeDef *hsd);
```

### 3.7 状态与错误处理

```c
// 获取SD卡状态
HAL_SD_StateTypeDef HAL_SD_GetState(SD_HandleTypeDef *hsd);

// 获取错误码
uint32_t HAL_SD_GetError(SD_HandleTypeDef *hsd);
```

### 3.8 关键配置参数

#### 3.8.1 SDIO 时钟计算

```c
// 时钟分频公式
SDIO_Clock = SDIOCLK / (CLKDIV + 2)

// 初始化阶段 (400kHz)
#define SDIO_INIT_CLK_DIV  ((uint32_t)0x76)  // 118分频 (48MHz/(118+2)=400kHz)

// 高速模式 (25MHz)
#define SDIO_HIGH_CLK_DIV  ((uint32_t)0x01)  // 1分频 (48MHz/(1+2)=16MHz)
```

#### 3.8.2 GPIO 配置 (STM32F407)

```c
// SDIO引脚配置 (4位模式)
void HAL_SD_MspInit(SD_HandleTypeDef* sdHandle) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    if(sdHandle->Instance == SDIO) {
        // 使能SDIO时钟
        __HAL_RCC_SDIO_CLK_ENABLE();
        
        // 使能GPIO时钟
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOD_CLK_ENABLE();
        
        // 配置SDIO引脚
        // PC8 - D0 | PC9 - D1 | PC10 - D2 | PC11 - D3
        // PC12 - CLK | PD2 - CMD
        GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
        
        GPIO_InitStruct.Pin = GPIO_PIN_2;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
        
        // 配置DMA (可选)
        hdma_sdio_rx.Instance = DMA2_Stream3;
        hdma_sdio_rx.Init.Channel = DMA_CHANNEL_4;
        hdma_sdio_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        // ... DMA配置
        HAL_DMA_Init(&hdma_sdio_rx);
        __HAL_LINKDMA(sdHandle, hdmarx, hdma_sdio_rx);
        
        // 启用中断
        HAL_NVIC_SetPriority(SDIO_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(SDIO_IRQn);
    }
}
```

### 3.9 高级功能与优化

#### 3.9.1 性能优化技巧

```c
// 启用高速模式
void Enable_High_Speed_Mode(void) {
    // 配置高速时钟 (25MHz)
    hsd.Instance->CLKCR &= ~SDIO_CLKCR_CLKDIV;
    hsd.Instance->CLKCR |= SDIO_HIGH_CLK_DIV;
    hsd.Instance->CLKCR |= SDIO_CLKCR_CLKEN;
}

// 启用DMA双缓冲
void Configure_DMA_Double_Buffer(void) {
    hdma_sdio_rx.Init.Mode = DMA_DOUBLE_BUFFER_MODE;
    hdma_sdio_rx.Init.MemoryBurst = DMA_MBURST_INC4;
    hdma_sdio_rx.Init.PeriphBurst = DMA_PBURST_INC4;
    HAL_DMA_Init(&hdma_sdio_rx);
}
```

#### 3.9.2 文件系统集成 (FatFS)

```c
// FatFS 磁盘接口函数
DSTATUS SD_disk_initialize(BYTE pdrv) {
    if (HAL_SD_Init(&hsd) != HAL_OK) return STA_NOINIT;
    return RES_OK;
}

DRESULT SD_disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count) {
    if (HAL_SD_ReadBlocks(&hsd, buff, sector, count, 1000) != HAL_OK) 
        return RES_ERROR;
    return RES_OK;
}

DRESULT SD_disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count) {
    if (HAL_SD_WriteBlocks(&hsd, (uint8_t*)buff, sector, count, 1000) != HAL_OK) 
        return RES_ERROR;
    return RES_OK;
}
```

#### 3.9.3 错误处理与恢复

```c
// SD卡错误恢复流程
void SD_Error_Recovery(SD_HandleTypeDef *hsd) {
    // 1. 中止当前操作
    HAL_SD_Abort(hsd);
    
    // 2. 重置SDIO外设
    __HAL_RCC_SDIO_FORCE_RESET();
    HAL_Delay(1);
    __HAL_RCC_SDIO_RELEASE_RESET();
    
    // 3. 重新初始化SD卡
    HAL_SD_DeInit(hsd);
    MX_SDIO_SD_Init();
    
    // 4. 重新挂载文件系统
    f_mount(&SDFatFS, SDPath, 1);
}
```

#### 3.9.4 多块读写优化

```c
// 高效多块写入
void Write_Multiple_Blocks(uint32_t start_sector, uint8_t *data, uint32_t count) {
    // 预配置DMA
    HAL_SD_ConfigWideBusOperation(&hsd, SDIO_BUS_WIDE_4B);
    
    // 启用高速模式
    Enable_High_Speed_Mode();
    
    // 分块写入 (每次最多32块)
    uint32_t chunks = count / 32;
    uint32_t remainder = count % 32;
    
    for (uint32_t i = 0; i < chunks; i++) {
        HAL_SD_WriteBlocks_DMA(&hsd, 
                              data + i * 32 * BLOCK_SIZE, 
                              start_sector + i * 32, 
                              32);
        // 等待传输完成
        while (HAL_SD_GetState(&hsd) != HAL_SD_STATE_READY) {}
    }
    
    // 写入剩余块
    if (remainder > 0) {
        HAL_SD_WriteBlocks_DMA(&hsd, 
                              data + chunks * 32 * BLOCK_SIZE, 
                              start_sector + chunks * 32, 
                              remainder);
    }
}
```

---


