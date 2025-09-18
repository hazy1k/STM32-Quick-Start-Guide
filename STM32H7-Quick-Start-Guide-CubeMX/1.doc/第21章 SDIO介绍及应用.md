# 第二十一章 SDIO介绍及应用

## 1. SDIO 简介

SDIO（Secure Digital Input/Output Interface）是 STM32H750VBT6 中用于**连接 SD 卡、MMC 卡或 SDIO 外设**的高速接口，支持 **SD Memory Card**（存储卡）和 **SDIO Peripheral**（如 Wi-Fi 模块）的通信。SDIO 接口可实现 **高达 50 Mbps 的数据传输速率**（在 4 位宽模式下），是嵌入式系统中实现**大容量数据存储、固件更新、文件系统（如 FatFs）** 的关键外设。

> 🔍 **核心定位**：
> 
> - **SDIO ≠ 普通 SPI**，而是**专用的多线、高速、协议复杂**的存储接口
> - 支持 **1 位、4 位数据总线**（4 位模式推荐）
> - 支持 **SD 卡协议标准**：SD 1.0/2.0/3.0、MMC
> - 与 **DMA** 协同实现**零 CPU 开销的数据传输**
> - 是实现 **FAT 文件系统、日志记录、数据采集存储** 的核心

---

### 1.1 SDIO 核心特性（STM32H750VBT6）

| **特性**     | **参数**                   | **说明**          | **应用场景** |
| ---------- | ------------------------ | --------------- | -------- |
| **数据宽度**   | 1-bit / 4-bit            | 4-bit 模式提升带宽    | 高速读写     |
| **时钟频率**   | 最高 **48 MHz**            | 由 SDMMCCLK 分频得到 | 匹配卡能力    |
| **数据传输**   | 块读写（512 字节）              | 支持多块连续传输        | 文件系统     |
| **DMA 支持** | ✅（专用 DMA 通道）             | 零 CPU 开销        | 大文件传输    |
| **中断支持**   | 数据完成、错误、卡插入              | 实时通知 CPU        | 卡热插拔检测   |
| **卡类型支持**  | SDSC、SDHC（≤2TB）、SDXC、MMC | 兼容主流存储卡         | 工业存储     |
| **供电控制**   | 输出 3.3V（需外部电平转换）         | 通常接 3.3V 卡      | 注意电平匹配   |

📌 **STM32H750VBT6 专属优势**：

- **独立时钟域**：SDMMC 运行在 **48 MHz**（来自 PLL）
- **支持 SDIO 1.8V 模式**（UHS-I）：通过 `WIDBUS` 和 `SDIO_UHS` 配置
- **与 FatFs 文件系统无缝集成**
- **支持双缓冲 DMA**（提升传输效率）

---

### 1.2 SDIO 工作原理详解

#### 1.2.1 SDIO 信号线

| **信号**      | **方向** | **说明**        |
| ----------- | ------ | ------------- |
| **CK**      | 输出     | 时钟（最高 48 MHz） |
| **CMD**     | 输入/输出  | 命令/响应线（开漏）    |
| **D0–D3**   | 输入/输出  | 数据线（4 位模式）    |
| **CD/DAT2** | 输入     | 卡检测（可复用）      |
| **WP**      | 输入     | 写保护检测         |

> ⚠️ **注意**：
> 
> - STM32H750VBT6 的 SDIO 接口使用 **GPIOC 和 GPIOD**
> - 典型引脚分配：
>   - `CK` → PC12
>   - `CMD` → PD2
>   - `D0` → PC8
>   - `D1` → PC9
>   - `D2` → PC10
>   - `D3` → PC11

#### 1.2.2 SDIO 通信流程

1. **初始化阶段**：
   
   - 上电 → 发送 `CMD0`（复位）
   - 发送 `CMD8`（检测电压）
   - 发送 `ACMD41`（主机供电启动）
   - 获取 **RCA（Relative Card Address）**

2. **识别阶段**：
   
   - 发送 `CMD2`（获取 CID）
   - 发送 `CMD3`（获取 RCA）

3. **数据传输阶段**：
   
   - 发送 `CMD7`（选择卡）
   - 发送 `CMD16`（设置块大小）
   - 发送 `CMD17/18`（单块/多块读）
   - 发送 `CMD24/25`（单块/多块写）

---

### 1.3 关键寄存器操作

#### 1.3.1 SDIO 主要寄存器

| **寄存器**    | **关键位域**                                         | **功能**    | **说明**           |
| ---------- | ------------------------------------------------ | --------- | ---------------- |
| **POWER**  | PWRCTRL[1:0]                                     | 电源控制      | `01` = 上电        |
| **CLKCR**  | CLKDIV, PWRSAV, WIDBUS, HWFC_EN                  | 时钟分频、数据宽度 | 核心时钟配置           |
| **ARG**    | CMDARG[31:0]                                     | 命令参数      | 如 RCA、地址         |
| **CMD**    | CMDINDEX, WAITRESP, WAITINT, CPSMEN              | 发送命令      | `CPSMEN=1` 触发    |
| **RESPx**  | CARDSTATUS                                       | 响应寄存器     | 读取 R1/R2/R3/R6 等 |
| **DTIMER** | DATATIME                                         | 数据超时      | 通常设最大值           |
| **DLEN**   | DATALENGTH                                       | 数据长度（字节）  | 必须是块大小倍数         |
| **DCTRL**  | DTEN, DTDIR, DMAEN, DBLOCKSIZE                   | 数据控制      | 启动数据传输           |
| **STAR**   | CMDACT, TXACT, RXACT, DATAEND, DBCKEND, DCPSFAIL | 状态标志      | 中断/轮询依据          |
| **ICR**    | ALL CLEAR BITS                                   | 清除中断标志    | 写 1 清除           |

#### 1.3.2 配置步骤（初始化 SDIO 接口）

```c
// 1. 使能 SDIO 时钟和 GPIO 时钟
RCC->AHB3ENR |= RCC_AHB3ENR_SDMMC1EN;
RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN | RCC_AHB4ENR_GPIODEN;

// 2. 配置 GPIO（PC8–12, PD2）
// 以 PC12 (CK) 为例
GPIOC->MODER   |= GPIO_MODER_MODER12_1; // 复用
GPIOC->OTYPER  &= ~GPIO_OTYPER_OT12;    // 推挽
GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEED12; // 超高速
GPIOC->PUPDR   |= GPIO_PUPDR_PUPD12_0;   // 上拉
GPIOC->AFR[1]  |= 12 << 16;             // PC12 = AF12 (SDIO)

// ... 配置 PC8–11 (D0–D3), PD2 (CMD)

// 3. 上电 SDIO
SDMMC1->POWER = 0x03; // PWRCTRL = 11 → 上电

// 4. 配置时钟（假设 SDMMCCLK=48MHz，目标 400 kHz 初始化）
SDMMC1->CLKCR = 119 << 0; // CLKDIV = 119 → 48MHz/(119+2) ≈ 400kHz
SDMMC1->CLKCR |= SDMMC_CLKCR_CLKEN; // 使能时钟
```

#### 1.3.3 HAL 库简化操作

```c
SD_HandleTypeDef hsd1;

hsd1.Instance = SDMMC1;
hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
hsd1.Init.ClockDiv = 119; // 400 kHz

if (HAL_SD_Init(&hsd1) != HAL_OK) {
    Error_Handler();
}

// 读取卡信息
HAL_SD_GetCardInfo(&hsd1, &CardInfo);
```

### 1.4 SDIO vs SPI 方式读取 SD 卡对比

| **特性**     | **SDIO 模式** | **SPI 模式**     |
| ---------- | ----------- | -------------- |
| **数据线**    | 4 位（D0–D3）  | 1 位（MOSI/MISO） |
| **速率**     | 最高 50 Mbps  | 最高 25 Mbps     |
| **引脚数**    | 6–7         | 4              |
| **协议复杂度**  | 高（需状态机）     | 低（类似 SPI）      |
| **CPU 负载** | 低（DMA 支持）   | 中（SPI 传输）      |
| **文件系统兼容** | ✅（FatFs）    | ✅（FatFs）       |
| **卡识别**    | 原生支持        | 需模拟 SD 协议      |

> 💡 **选型建议**：
> 
> - **高速存储、大数据采集** → SDIO
> - **引脚紧张、简单应用** → SPI
> - **工业级可靠性** → SDIO + DMA + 错误重试机制

## 2. SDIO应用示例-STM32IDE

### 2.1 STM32Cube配置

![屏幕截图 2025-09-13 125342.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/09/13-13-14-51-屏幕截图%202025-09-13%20125342.png)

![屏幕截图 2025-09-13 125351.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/09/13-13-14-55-屏幕截图%202025-09-13%20125351.png)

### 2.2 用户代码

```c
#include "string.h"
#include "usart.h"
#include "sdmmc.h"

/**
 * @brief       设置中断向量表偏移地址
 * @param       baseaddr: 基址
 * @param       offset: 偏移量
 * @retval      无
 */
void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset)
{
    /* 设置NVIC的向量表偏移寄存器,VTOR低9位保留,即[8:0]保留 */
    SCB->VTOR = baseaddr | (offset & (uint32_t)0xFFFFFE00);
}

/**
 * @brief       执行: WFI指令(执行完该指令进入低功耗状态, 等待中断唤醒)
 * @param       无
 * @retval      无
 */
void sys_wfi_set(void)
{
    __ASM volatile("wfi");
}

/**
 * @brief       关闭所有中断(但是不包括fault和NMI中断)
 * @param       无
 * @retval      无
 */
void sys_intx_disable(void)
{
    __ASM volatile("cpsid i");
}

/**
 * @brief       开启所有中断
 * @param       无
 * @retval      无
 */
void sys_intx_enable(void)
{
    __ASM volatile("cpsie i");
}

/**
 * @brief       设置栈顶地址
 * @note        左侧的红X, 属于MDK误报, 实际是没问题的
 * @param       addr: 栈顶地址
 * @retval      无
 */
void sys_msr_msp(uint32_t addr)
{
    __set_MSP(addr);    /* 设置栈顶地址 */
}

SD_HandleTypeDef g_sd_handle;                    /* SD卡句柄 */
HAL_SD_CardInfoTypeDef g_sd_card_info_handle;    /* SD卡信息结构体 */

/**
 * @brief       初始化SD卡
 * @param       无
 * @retval      返回值:0 初始化正确；其他值，初始化错误
 */
uint8_t sd_init(void)
{
    uint8_t SD_Error;

    HAL_SD_DeInit(&g_sd_handle);                                                /* 清零相应参数 */

    /* 初始化时的时钟不能大于400KHZ */
    g_sd_handle.Instance = SDMMC1;
    g_sd_handle.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;                       /* 上升沿 */
    g_sd_handle.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;           /* 空闲时不关闭时钟电源 */
    g_sd_handle.Init.BusWide = SDMMC_BUS_WIDE_4B;                               /* 4位数据线 */
    g_sd_handle.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE; /* 关闭硬件流控 */
    g_sd_handle.Init.ClockDiv = SDMMC_NSpeed_CLK_DIV;                           /* SD传输时钟频率最大25MHZ */

    SD_Error = HAL_SD_Init(&g_sd_handle);
    if (SD_Error != HAL_OK)
    {
        return 1;
    }

    HAL_SD_GetCardInfo(&g_sd_handle, &g_sd_card_info_handle);   /* 获取SD卡信息 */
    return 0;
}

/**
 * @brief       SDMMC底层驱动，时钟使能，引脚配置
 * @param       hsd:SD卡句柄
 * @note        此函数会被HAL_SD_Init()调用
 * @retval      无
 */
void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{
    GPIO_InitTypeDef gpio_init_struct;

    __HAL_RCC_SDMMC1_CLK_ENABLE();  /* 使能SDMMC1时钟 */
    SD1_D0_GPIO_CLK_ENABLE();       /* D0引脚IO时钟使能 */
    SD1_D1_GPIO_CLK_ENABLE();       /* D1引脚IO时钟使能 */
    SD1_D2_GPIO_CLK_ENABLE();       /* D2引脚IO时钟使能 */
    SD1_D3_GPIO_CLK_ENABLE();       /* D3引脚IO时钟使能 */
    SD1_CLK_GPIO_CLK_ENABLE();      /* CLK引脚IO时钟使能 */
    SD1_CMD_GPIO_CLK_ENABLE();      /* CMD引脚IO时钟使能 */

    gpio_init_struct.Pin = SD1_D0_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* 推挽复用 */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;     /* 高速 */
    gpio_init_struct.Alternate = GPIO_AF12_SDIO1;           /* 复用为SDIO */
    HAL_GPIO_Init(SD1_D0_GPIO_PORT, &gpio_init_struct);     /* 初始化D0引脚 */

    gpio_init_struct.Pin = SD1_D1_GPIO_PIN;
    HAL_GPIO_Init(SD1_D1_GPIO_PORT, &gpio_init_struct);     /* 初始化D1引脚 */

    gpio_init_struct.Pin = SD1_D2_GPIO_PIN;
    HAL_GPIO_Init(SD1_D2_GPIO_PORT, &gpio_init_struct);     /* 初始化D2引脚 */

    gpio_init_struct.Pin = SD1_D3_GPIO_PIN;
    HAL_GPIO_Init(SD1_D3_GPIO_PORT, &gpio_init_struct);     /* 初始化D3引脚 */

    gpio_init_struct.Pin = SD1_CLK_GPIO_PIN;
    HAL_GPIO_Init(SD1_CLK_GPIO_PORT, &gpio_init_struct);    /* 初始化CLK引脚 */

    gpio_init_struct.Pin = SD1_CMD_GPIO_PIN;
    HAL_GPIO_Init(SD1_CMD_GPIO_PORT, &gpio_init_struct);    /* 初始化CMD引脚 */
}

/**
 * @brief       获取卡信息函数
 * @param       cardinfo:SD卡信息句柄
 * @retval      返回值:读取卡信息状态值
 */
uint8_t get_sd_card_info(HAL_SD_CardInfoTypeDef *cardinfo)
{
    uint8_t sta;
    sta = HAL_SD_GetCardInfo(&g_sd_handle, cardinfo);
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
  return ((HAL_SD_GetCardState(&g_sd_handle) == HAL_SD_CARD_TRANSFER ) ? SD_TRANSFER_OK : SD_TRANSFER_BUSY);
}

/**
 * @brief       读SD卡数据
 * @param       buf:读数据缓存区
 * @param       sector:扇区地址
 * @param       cnt:扇区个数
 * @retval      返回值:0,正常;其他,错误;
 */
uint8_t sd_read_disk(uint8_t *buf, uint32_t sector, uint32_t cnt)
{
    uint8_t sta = HAL_OK;
    uint32_t timeout = SD_TIMEOUT;
    long long lsector = sector;

    sys_intx_disable();                                                             /* 关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!) */
    sta = HAL_SD_ReadBlocks(&g_sd_handle, (uint8_t*)buf, lsector, cnt, SD_TIMEOUT); /* 多个sector的读操作 */

    /* 等待SD卡读完 */
    while (get_sd_card_state() != SD_TRANSFER_OK)
    {
        if (timeout-- == 0)
        {
            sta = SD_TRANSFER_BUSY;
            break;
        }
    }

    sys_intx_enable();  /* 开启总中断 */
    return sta;
}

/**
 * @brief       写数据到SD卡
 * @param       buf:写数据缓存区
 * @param       sector:扇区地址
 * @param       cnt:扇区个数
 * @retval      返回值:0,正常;其他,错误;
 */
uint8_t sd_write_disk(uint8_t *buf, uint32_t sector, uint32_t cnt)
{
    uint8_t sta = HAL_OK;
    uint32_t timeout = SD_TIMEOUT;
    long long lsector = sector;

    sys_intx_disable();                                                                 /* 关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!) */
    sta = HAL_SD_WriteBlocks(&g_sd_handle, (uint8_t*)buf, lsector, cnt, SD_TIMEOUT);    /* 多个sector的写操作 */

    /* 等待SD卡写完 */
    while (get_sd_card_state() != SD_TRANSFER_OK)
    {
        if (timeout-- == 0)
        {
            sta = SD_TRANSFER_BUSY;
            break;
        }
    }

    sys_intx_enable(); /* 开启总中断 */
    return sta;
}
```

```c
#include "main.h"
#include "bsp_init.h"
#include "sdmmc.h"
#include <stdio.h>

void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MPU_Config(void);

/**
 * @brief       通过串口打印SD卡相关信息
 * @param       无
 * @retval      无
 */
void show_sdcard_info(void)
{
    uint64_t card_capacity;       /* SD卡容量 */
    HAL_SD_CardCIDTypeDef sd_card_cid;

    HAL_SD_GetCardCID(&g_sd_handle, &sd_card_cid);       /* 获取CID */
    get_sd_card_info(&g_sd_card_info_handle);            /* 获取SD卡信息 */

    switch (g_sd_card_info_handle.CardType)
    {
        case CARD_SDSC:
        {
            if (g_sd_card_info_handle.CardVersion == CARD_V1_X)
            {
                printf("Card Type:SDSC V1\r\n");
            }
            else if (g_sd_card_info_handle.CardVersion == CARD_V2_X)
            {
                printf("Card Type:SDSC V2\r\n");
            }
        }
            break;

        case CARD_SDHC_SDXC:
            printf("Card Type:SDHC\r\n");
            break;
    }

    card_capacity = (uint64_t)(g_sd_card_info_handle.LogBlockNbr) * (uint64_t)(g_sd_card_info_handle.LogBlockSize); /* 计算SD卡容量 */
    printf("Card ManufacturerID:%d\r\n", sd_card_cid.ManufacturerID);                                               /* 制造商ID */
    printf("Card RCA:%lu\r\n", g_sd_card_info_handle.RelCardAdd);                                                   /* 卡相对地址 */
    printf("LogBlockNbr:%lu \r\n", (uint32_t)(g_sd_card_info_handle.LogBlockNbr));                                  /* 显示逻辑块数量 */
    printf("LogBlockSize:%lu \r\n", (uint32_t)(g_sd_card_info_handle.LogBlockSize));                                /* 显示逻辑块大小 */
    printf("Card Capacity:%lu MB\r\n", (uint32_t)(card_capacity >> 20));                                            /* 显示容量 */
    printf("Card BlockSize:%lu\r\n\r\n", g_sd_card_info_handle.BlockSize);                                          /* 显示块大小 */
}

/**
 * @brief       测试SD卡的读取
 *   @note      从secaddr地址开始,读取seccnt个扇区的数据
 * @param       secaddr : 扇区地址
 * @param       seccnt  : 扇区数
 * @retval      无
 */
void sd_test_read(uint32_t secaddr, uint32_t seccnt)
{
    uint32_t i;
    uint8_t buf[512];  // 为缓冲区分配内存
    uint8_t sta = 0;
    sta = sd_read_disk(buf, secaddr, seccnt);   /* 读取secaddr扇区开始的内容 */

    if (sta == 0)
    {
        printf("SECTOR %lu DATA:\r\n", secaddr);

        for (i = 0; i < seccnt * 512; i++)
        {
            printf("%x ", buf[i]);  /* 打印secaddr开始的扇区数据 */
        }

        printf("\r\nDATA ENDED\r\n");
    }
    else
    {
        printf("err:%d\r\n", sta);
    }
}

/**
 * @brief       测试SD卡的写入
 *   @note      从secaddr地址开始,写入seccnt个扇区的数据
 *              慎用!! 最好写全是0XFF的扇区,否则可能损坏SD卡.
 *
 * @param       secaddr : 扇区地址
 * @param       seccnt  : 扇区数
 * @retval      无
 */
void sd_test_write(uint32_t secaddr, uint32_t seccnt)
{
    uint32_t i;
    uint8_t buf[512];  // 为缓冲区分配内存
    uint8_t sta = 0;

    for (i = 0; i < seccnt * 512; i++)          /* 初始化写入的数据,是3的倍数. */
    {
        buf[i] = i * 3;
    }

    sta = sd_write_disk(buf, secaddr, seccnt);  /* 从secaddr扇区开始写入seccnt个扇区内容 */

    if (sta == 0)
    {
        printf("Write over!\r\n");
    }
    else
    {
        printf("err:%d\r\n", sta);
    }
}

int main(void)
{
  uint8_t key;
  uint8_t t = 0;
  //uint64_t card_capacity;       /* SD卡容量 */
  MPU_Config();
  HAL_Init();
  SystemClock_Config();
  PeriphCommonClock_Config();
  bsp_init();
  while(sd_init())           /* 初始化SD卡,直到成功 */
  {
      printf("SD Card init failed! Please check!\r\n");
      HAL_Delay(1000);
  }
  while (1)
  {
      key = key_scan(0);
      if(key == KEY0_PRES)
      {
          sd_test_read(0, 1);
      }
      t++;
      HAL_Delay(10);
      if(t == 20)
      {
          t = 0;
          HAL_GPIO_TogglePin(LED_RED_Port, LED_RED_Pin);
      }
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 120;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CKPER;
  PeriphClkInitStruct.CkperClockSelection = RCC_CLKPSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
```

## 3. SDMMC/SDIO相关函数总结（HAL库）

> **注**：STM32H7系列使用**SDMMC控制器**（SD/MMC卡接口），取代了旧系列的SDIO控制器。以下内容基于H7系列的SDMMC外设。

### 3.1 初始化与配置

- **核心配置流程**（五步关键操作）：
  
  1. **使能时钟**（SDMMC + GPIO）
  2. **配置GPIO复用功能**（CLK, CMD, D0-D3, D4-D7可选）
  3. **初始化SDMMC参数**（时钟/总线宽度等）
  4. **初始化SD卡**
  5. **配置NVIC中断**（若使用中断模式）

- `HAL_SD_Init(SD_HandleTypeDef *hsd)`  
  **基础配置示例**（4位总线模式）：
  
  ```c
  // 1. 使能SDMMC和GPIO时钟
  __HAL_RCC_SDMMC1_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  
  // 2. 配置GPIO（SDMMC1接口）
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  // CLK (PC12)
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
  // CMD (PD2)
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  
  // D0 (PC8), D1 (PC9), D2 (PC10), D3 (PC11)
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
  // 3. 配置SDMMC参数
  hsd.Instance = SDMMC1;
  hsd.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDMMC_BUS_WIDE_4B;           // 4位总线
  hsd.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 0;                          // 200MHz/2 = 100MHz (初始速度)
  
  // 4. 初始化SD卡
  HAL_SD_Init(&hsd);
  
  // 5. 配置卡参数
  if (HAL_SD_ConfigWideBusOperation(&hsd, SDMMC_BUS_WIDE_4B) != HAL_OK) {
      Error_Handler();
  }
  ```

- **`SD_InitTypeDef` 结构体成员说明**：
  
  | **成员**                | **说明** | **有效值**             | **H750特殊说明**          |
  | --------------------- | ------ | ------------------- | --------------------- |
  | `ClockEdge`           | 时钟边沿   | `RISING`, `FALLING` | 上升沿采样                 |
  | `ClockPowerSave`      | 时钟节能   | `ENABLE`, `DISABLE` | 建议禁用                  |
  | `BusWide`             | 总线宽度   | `1B`, `4B`, `8B`    | 4位最常用                 |
  | `HardwareFlowControl` | 硬件流控   | `ENABLE`, `DISABLE` | 禁用                    |
  | `ClockDiv`            | 时钟分频   | 0-254               | `SDMMCCLK/(CLKDIV+2)` |

- **SDMMC时钟配置**（核心！）：
  
  - **时钟源**：`SDMMCCLK = 200MHz`（来自PLL2）
  
  - **实际频率**：`f = SDMMCCLK / (ClockDiv + 2)`
  
  - **典型配置**：
    
    | **阶段**       | **ClockDiv** | **频率** | **用途**  |
    | ------------ | ------------ | ------ | ------- |
    | 初始化          | 198          | 1MHz   | 卡识别     |
    | 数据传输         | 0            | 100MHz | 高速读写    |
    | UHS-I SDR50  | 3            | 50MHz  | 50MB/s  |
    | UHS-I SDR104 | 0            | 100MHz | 104MB/s |

- **GPIO配置要点**：
  
  ```c
  // SDMMC引脚必须配置为复用推挽+上拉
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;      // 复用推挽
  GPIO_InitStruct.Pull = GPIO_PULLUP;          // 上拉电阻
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1; // 复用功能
  ```

### 3.2 SD卡操作核心函数

- **基础读写操作**：
  
  | **函数**                     | **原型**                                        | **特点** | **应用场景** |
  | -------------------------- | --------------------------------------------- | ------ | -------- |
  | `HAL_SD_ReadBlocks()`      | `(hsd, *pData, BlockAdd, BlockSize, Timeout)` | 阻塞读取   | 读取文件     |
  | `HAL_SD_WriteBlocks()`     | `(hsd, *pData, BlockAdd, BlockSize, Timeout)` | 阻塞写入   | 写入文件     |
  | `HAL_SD_ReadBlocks_DMA()`  | `(hsd, *pData, BlockAdd, BlockSize)`          | DMA读取  | 大文件      |
  | `HAL_SD_WriteBlocks_DMA()` | `(hsd, *pData, BlockAdd, BlockSize)`          | DMA写入  | 高速写入     |
  | `HAL_SD_ReadBlocks_IT()`   | `(hsd, *pData, BlockAdd, BlockSize)`          | 中断读取   | 非阻塞      |
  | `HAL_SD_WriteBlocks_IT()`  | `(hsd, *pData, BlockAdd, BlockSize)`          | 中断写入   |          |

- **块操作参数**：
  
  ```c
  // 读取多个块（512字节/块）
  uint8_t buffer[512];
  HAL_StatusTypeDef status = HAL_SD_ReadBlocks(&hsd, 
                                              buffer, 
                                              0x1000,     // 块地址
                                              512,        // 块大小
                                              1000);      // 超时
  
  // 写入多个块
  status = HAL_SD_WriteBlocks(&hsd, 
                             buffer, 
                             0x1000, 
                             512, 
                             1000);
  ```

- **卡信息获取**：
  
  ```c
  // 获取卡信息
  HAL_SD_GetCardInfo(&hsd, &card_info);
  
  // 信息结构体
  typedef struct {
      uint32_t CardType;           // 卡类型 (SDSC, SDHC, SDXC)
      uint32_t CardVersion;        // 卡版本
      uint32_t Class;              // 性能等级
      uint32_t RelCardAdd;         // 相对卡地址
      uint32_t BlockNbr;           // 总块数
      uint32_t BlockSize;          // 块大小 (512字节)
      uint32_t LogBlockNbr;        // 逻辑块数
      uint32_t LogBlockSize;       // 逻辑块大小
  } HAL_SD_CardInfoTypeDef;
  
  // 计算卡容量
  uint32_t capacity = card_info.BlockNbr * card_info.BlockSize; // 字节数
  ```

- **状态检查**：
  
  ```c
  // 检查卡状态
  uint32_t status;
  HAL_SD_GetCardStatus(&hsd, &status);
  
  // 检查是否准备好
  if (HAL_SD_CheckWriteProtect(&hsd) == SD_WRITE_PROTECTED) {
      // 卡写保护
  }
  
  if (HAL_SD_CheckCardStatus(&hsd) != HAL_OK) {
      // 卡未准备好
  }
  ```

### 3.3 高级功能与特性

- **DMA模式传输**：
  
  ```c
  // 启用DMA传输
  __HAL_LINKDMA(&hsd, hdmatx, hdma_sdmmc1_tx);
  __HAL_LINKDMA(&hsd, hdmarx, hdma_sdmmc1_rx);
  
  // DMA传输（非阻塞）
  HAL_SD_ReadBlocks_DMA(&hsd, dma_buffer, start_block, block_count);
  
  // 传输完成回调
  void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
  {
      if(hsd == &hsd1) {
          transmission_complete = 1;
      }
  }
  
  void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
  {
      if(hsd == &hsd1) {
          reception_complete = 1;
      }
  }
  ```

- **多块读写**：
  
  ```c
  // 多块读取
  HAL_SD_ReadBlocks(&hsd, buffer, start_block, 512, 1000);
  
  // 停止传输
  HAL_SD_StopTransfer(&hsd);
  ```

- **擦除操作**：
  
  ```c
  // 擦除特定块范围
  HAL_SD_Erase(&hsd, start_block, end_block);
  
  // 擦除整个卡
  HAL_SD_Erase(&hsd, 0, 0xFFFFFFFF);
  ```

- **SDMMC中断处理**：
  
  ```c
  // 中断服务函数
  void SDMMC1_IRQHandler(void)
  {
      HAL_SD_IRQHandler(&hsd1);
  }
  
  // 错误中断服务函数
  void SDMMC1_IRQHandler(void)
  {
      HAL_SD_IRQHandler(&hsd1);
  }
  
  // 回调函数
  void HAL_SD_ErrorCallback(SD_HandleTypeDef *hsd)
  {
      if(hsd == &hsd1) {
          sd_error_handler();
          // 重新初始化卡
          HAL_SD_Init(hsd);
          HAL_SD_ConfigWideBusOperation(hsd, SDMMC_BUS_WIDE_4B);
      }
  }
  ```

### 3.4 使用示例（完整流程）

#### 3.4.1 示例1：SD卡文件系统初始化

```c
SD_HandleTypeDef hsd1 = {0};
HAL_SD_CardInfoTypeDef card_info = {0};

// 1. 初始化SDMMC硬件
void SDMMC_HW_Init(void)
{
    __HAL_RCC_SDMMC1_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    // 配置GPIO (PC8-PC12, PD2)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

// 2. 初始化SD卡
HAL_StatusTypeDef SD_Card_Init(void)
{
    hsd1.Instance = SDMMC1;
    hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
    hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
    hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
    hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
    hsd1.Init.ClockDiv = 198;  // 1MHz用于初始化

    // 初始化SD卡
    if (HAL_SD_Init(&hsd1) != HAL_OK) {
        return HAL_ERROR;
    }

    // 配置4位总线
    if (HAL_SD_ConfigWideBusOperation(&hsd1, SDMMC_BUS_WIDE_4B) != HAL_OK) {
        return HAL_ERROR;
    }

    // 获取卡信息
    HAL_SD_GetCardInfo(&hsd1, &card_info);

    // 设置高速模式时钟（100MHz）
    hsd1.Init.ClockDiv = 0;
    __HAL_SDMMC_RESET_CLOCK(&hsd1);  // 重新配置时钟

    return HAL_OK;
}

// 3. 读取SD卡扇区
HAL_StatusTypeDef SD_Read_Sector(uint32_t sector, uint8_t *buffer)
{
    return HAL_SD_ReadBlocks(&hsd1, buffer, sector, 512, 1000);
}

// 4. 写入SD卡扇区
HAL_StatusTypeDef SD_Write_Sector(uint32_t sector, uint8_t *buffer)
{
    return HAL_SD_WriteBlocks(&hsd1, buffer, sector, 512, 1000);
}
```

#### 3.4.2 示例2：DMA模式高速数据记录

```c

```

## 4. 关键注意事项

1. **时钟配置陷阱**：
   
   - **初始化阶段**必须使用低速时钟（≤400kHz）
   
   - **识别完成后**才能切换到高速模式
   
   - **H750时钟源**：
     
     ```c
     // 配置SDMMC时钟源（来自PLL2）
     RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
     PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SDMMC;
     PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL;
     HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
     ```

2. **电源管理**：
   
   - **SDMMC电源域**必须稳定
   
   - **支持1.8V和3.3V**操作
   
   - **电平转换**：
   
   ```c
   // 配置1.8V操作（UHS-I）
   HAL_SD_ConfigVoltageWindow(&hsd1, SDMMC_1_8V);
   ```

3. **总线宽度**：
   
   | **模式** | **引脚** | **速度** | **H750配置**      |
   | ------ | ------ | ------ | --------------- |
   | 1位     | D0     | 低速     | `BUS_WIDE_1B`   |
   | 4位     | D0-D3  | 高速     | `BUS_WIDE_4B` ✅ |
   | 8位     | D0-D7  | 最高速    | `BUS_WIDE_8B`   |

4. **中断优先级**：
   
   - SDMMC中断优先级应高于DMA中断
   - 否则可能导致数据丢失
   
   ```c
   HAL_NVIC_SetPriority(SDMMC1_IRQn, 0, 0);  // 高优先级
   HAL_NVIC_EnableIRQ(SDMMC1_IRQn);
   ```

5. **物理层设计**：
   
   - **上拉电阻**：10-50kΩ（内部可提供）
   - **信号完整性**：
     - CLK走线长度匹配
     - 远离噪声源
     - 3.3V电源滤波

---

### 4.2 H750特有优化技巧

| **场景**    | **解决方案** | **性能提升** | **实现要点**            |
| --------- | -------- | -------- | ------------------- |
| **高速读取**  | 8位总线+DMA | 速度↑2倍    | `BUS_WIDE_8B`       |
| **低延迟**   | 中断模式     | 响应更快     | `IT`函数              |
| **大容量支持** | SDXC     | ≥64GB    | `HAL_SD_Init()`自动识别 |
| **电源效率**  | 时钟门控     | 功耗↓      | 不使用时关闭时钟            |

> **避坑指南**：
> 
> 1. **H750 SDMMC架构**：
>    
>    - 支持SDR104模式（100MHz时钟）
>    - 可配置FIFO阈值优化性能
> 
> 2. **DMA缓冲区对齐**：
>    
>    - 必须32字节对齐
>    - 使用`__attribute__((aligned(32)))`
> 
> 3. **卡热插拔检测**：
>    
>    ```c
>    // 使用GPIO检测卡插入
>    if (HAL_GPIO_ReadPin(CARD_DETECT_GPIO, CARD_DETECT_PIN) == GPIO_PIN_RESET) {
>        // 卡插入
>        SD_Card_Init();
>    }
>    ```
> 
> 4. **生产环境测试**：
>    
>    - 测试不同品牌/容量的SD卡
>    - 验证高温/低温下的可靠性
>    - 测试长时间连续读写

---

### 4.2 SDMMC模式性能对比

| **模式**        | **理论带宽**    | **实际可用**      | **时钟配置**         |
| ------------- | ----------- | ------------- | ---------------- |
| Default Speed | 25MB/s      | 20-22MB/s     | 25MHz, 4位        |
| High Speed    | 50MB/s      | 40-45MB/s     | 50MHz, 4位        |
| SDR12         | 12MB/s      | 10MB/s        | 1.8V             |
| SDR25         | 25MB/s      | 22MB/s        | 1.8V             |
| SDR50         | 50MB/s      | 45MB/s        | 1.8V, 50MHz      |
| **SDR104**    | **100MB/s** | **85-90MB/s** | **1.8V, 100MHz** |

> **重要提示**：
> 
> - SDMMC是**大容量存储**的主要接口
> - DMA模式可实现**持续高速数据流**
> - 4位总线是**性能和引脚数**的最佳平衡
> - 仔细设计PCB布局是稳定工作的**关键**

---
