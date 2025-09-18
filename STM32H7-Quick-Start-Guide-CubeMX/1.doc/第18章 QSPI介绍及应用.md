# 第十八章 QSPI介绍及应用

QSPI（Quad-SPI，四线串行外设接口）是 STM32H750VBT6 中用于**高速访问外部串行存储器**的专用外设，支持 **单线、双线、四线 SPI 模式**，可将外部 NOR Flash 映射到 CPU 地址空间，实现 **XIP（eXecute In Place）** ——即**直接从外部 Flash 运行代码**。QSPI 极大扩展了 STM32 的存储能力，是图形显示、固件存储、音频播放等大容量数据应用的核心接口。

> 🔍 **核心定位**：
> 
> - **QSPI ≠ 普通 SPI**，而是**专为外部 Flash 优化的“存储控制器”**
> - 支持 **最高 133 MHz 时钟**（DTR 模式可达 266 Mbps）
> - 支持 **内存映射模式**（Memory-Mapped Mode）
> - 可连接 **串行 NOR Flash、NAND Flash、SRAM**
> - 是 STM32H7 实现 **大容量代码与数据存储** 的关键外设

---

### 1.1 QSPI 核心特性（STM32H750VBT6）

| **特性**     | **参数**               | **说明**          | **应用场景**   |
| ---------- | -------------------- | --------------- | ---------- |
| **数据线**    | IO0–IO3（可配置为单/双/四线）  | 四线并行传输          | 高带宽通信      |
| **时钟**     | CLK（输出）              | 最高 133 MHz（SDR） | 高速读写       |
| **片选**     | NCS（低有效）             | 支持多设备           | 多 Flash 挂载 |
| **工作模式**   | 间接模式、直接内存映射模式、自动轮询模式 | 灵活访问策略          | XIP、固件更新   |
| **XIP 支持** | ✅                    | 代码直接从 Flash 执行  | 扩展程序存储     |
| **DMA 支持** | ✅                    | 零 CPU 开销数据传输    | 大文件加载      |
| **DTR 模式** | ✅（双倍数据速率）            | CLK 上升/下降沿均采样   | 带宽翻倍       |
| **地址宽度**   | 24-bit 或 32-bit      | 支持大容量 Flash     | ≥16 MB 设备  |

📌 **STM32H750VBT6 专属优势**：

- **内置 48 MHz HSI48 时钟**：无需外部晶振即可驱动 QSPI
- **支持 HyperBus 协议**：兼容 Micron、Cypress 等高速存储器
- **与 FMC 协同**：可实现 Flash + PSRAM 组合存储系统
- **安全功能**：支持 **Flash 加密**（通过 AES）和 **写保护**

---

### 1.2 QSPI 工作原理详解

#### 1.2.1 通信模式对比

| **模式**     | **数据线**         | **传输方向** | **速率** | **用途**         |
| ---------- | --------------- | -------- | ------ | -------------- |
| **单线 SPI** | IO0             | 单向       | 低      | 兼容传统 SPI Flash |
| **双线 SPI** | IO0（输出），IO1（输入） | 半双工      | 中      | 平衡速度与引脚        |
| **四线 SPI** | IO0–IO3         | 全双工（读）   | 高      | XIP、高速读取       |

- **DTR 模式（双倍数据速率）**：
  - 在 CLK 的**上升沿和下降沿**都采样数据
  - 实际速率翻倍（如 133 MHz → 266 Mbps）
  - 需 Flash 支持 DTR（如 MX25LM51245G）

#### 1.2.2 QSPI 三种工作模式

1. **间接模式（Indirect Mode）**
   
   - CPU 通过 QSPI 寄存器发送命令，读写数据
   - 适合 **写入、擦除、配置 Flash**
   - 代码示例：写入 256 字节数据

2. **直接内存映射模式（Memory-Mapped Mode）**
   
   - 外部 Flash 映射到地址 `0x90000000`
   - CPU 可像访问内部 Flash 一样读取代码
   - 实现 **XIP（代码执行）**
   - 启动后可从 QSPI Flash 运行 `main()`

3. **自动轮询模式（Auto-Polling Mode）**
   
   - 自动发送读状态命令，等待 Flash 操作完成
   - 用于 **轮询写/擦除完成标志**
   - 无需 CPU 轮询，提高效率

---

### 1.3 关键寄存器操作

#### 1.3.1 QSPI 主要寄存器

| **寄存器** | **功能**  | **关键位域**                                      | **说明**       |
| ------- | ------- | --------------------------------------------- | ------------ |
| **CR**  | 控制寄存器   | `EN`, `FMODE`, `SMM`, `DMM`, `DTR`            | 启用、模式选择      |
| **DCR** | 设备配置寄存器 | `CSHOLD`, `CSSCK`, `DEVSIZE`                  | 时序与设备大小      |
| **SR**  | 状态寄存器   | `TC`, `FTF`, `BSY`                            | 传输完成、FIFO 状态 |
| **FCR** | 标志清除寄存器 | `CTCF`, `CSMFC`                               | 写 1 清除中断     |
| **DLR** | 数据长度寄存器 | `DL`                                          | 传输字节数        |
| **ABR** | 地址寄存器   | `ADDRESS`                                     | 目标地址         |
| **CCR** | 通信配置寄存器 | `IMODE`, `ADMODE`, `ABMODE`, `DMODE`, `FMODE` | 命令、地址、数据模式   |
| **TDR** | 发送数据寄存器 | `TD`                                          | 写入发送数据       |
| **RDR** | 接收数据寄存器 | `RD`                                          | 读取接收数据       |

#### 1.3.2 配置流程（间接模式写数据）

```c
// 1. 使能 QSPI 时钟
RCC->AHB3ENR |= RCC_AHB3ENR_QSPIEN;

// 2. 配置 GPIO（PB2=CLK, PB6=NC, PB10–11=IO0–1, PB1–3=IO2–3）
// 以 PB1/2/6/10/11 为例（AF9 = QSPI）
GPIOB->MODER |= GPIO_MODER_MODER1_1 | GPIO_MODER_MODER2_1 | 
                GPIO_MODER_MODER6_1 | GPIO_MODER_MODER10_1 | GPIO_MODER_MODER11_1;
GPIOB->OTYPER &= ~(0x00002C44); // 推挽
GPIOB->OSPEEDR |= 0x0000AAAA;  // 超高速
GPIOB->AFR[0] |= 9 << 4;     // PB1 = AF9
GPIOB->AFR[0] |= 9 << 8;     // PB2 = AF9
GPIOB->AFR[0] |= 9 << 24;    // PB6 = AF9
GPIOB->AFR[1] |= 9 << 8;     // PB10 = AF9
GPIOB->AFR[1] |= 9 << 12;    // PB11 = AF9

// 3. 配置 QSPI
QUADSPI->CR = 0; // 确保未使能
QUADSPI->DCR = (1 << 16) | (1 << 8) | (31 << 0); // CSHOLD=1, CSSCK=1, DEVSIZE=31 (4GB)

// 4. 配置通信（四线写，24-bit 地址）
QUADSPI->CCR = 
       QSPI_CCR_FMODE_0                // 间接写
     | QSPI_CCR_DMODE_2                // 四线数据
     | QSPI_CCR_ADMODE_2               // 四线地址
     | QSPI_CCR_IMODE_2                // 四线指令
     | (0x02 << 16);                   // INSTRUCTION = 0x02 (快速写)

// 5. 设置地址
QUADSPI->AR = 0x00000000; // 写入地址 0

// 6. 设置数据长度
QUADSPI->DLR = 255; // 256 字节

// 7. 使能 QSPI
QUADSPI->CR |= QSPI_CR_EN;

// 8. 写入数据（循环）
for (int i = 0; i < 256; i++) {
    while (!(QUADSPI->SR & QSPI_SR_FTF)); // FIFO 可写
    QUADSPI->TDR = data_buffer[i];
}

// 9. 等待传输完成
while (!(QUADSPI->SR & QSPI_SR_TC));
QUADSPI->FCR = QSPI_FCR_CTCF; // 清除标志
```

#### 1.3.3 HAL 库简化操作

```c
QSPI_CommandTypeDef sCommand = {0};

sCommand.InstructionMode = QSPI_INSTRUCTION_4_LINES;
sCommand.Instruction = 0x02; // 四线写
sCommand.AddressMode = QSPI_ADDRESS_4_LINES;
sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
sCommand.Address = 0x000000;
sCommand.DataMode = QSPI_DATA_4_LINES;
sCommand.DummyCycles = 0;
sCommand.NbData = 256;
sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_TIMEOUT_DEFAULT) != HAL_OK) {
    Error_Handler();
}

if (HAL_QSPI_Transmit(&hqspi, data_buffer, HAL_TIMEOUT_DEFAULT) != HAL_OK) {
    Error_Handler();
}
```

### 1.4 QSPI vs FMC 对比

| **特性**     | **QSPI**            | **FMC**                |
| ---------- | ------------------- | ---------------------- |
| **接口类型**   | 串行（4 线）             | 并行（16+ 线）              |
| **速度**     | ~50–100 MB/s（四线）    | ~100 MB/s（SRAM）        |
| **引脚数**    | 6–8                 | 30–50                  |
| **存储类型**   | NOR Flash, HyperRAM | SRAM, PSRAM, NAND, LCD |
| **XIP 支持** | ✅                   | ✅（NOR）                 |
| **成本**     | 低（Flash 便宜）         | 高（PSRAM 贵）             |
| **功耗**     | 低                   | 高                      |
| **典型应用**   | 代码存储、音频             | 图形缓冲、高速缓存              |

> 💡 **选型建议**：
> 
> - **代码存储、固件升级** → QSPI + NOR Flash
> - **图形显示、大数据缓存** → FMC + PSRAM
> - **低成本大存储** → QSPI
> - **高性能实时访问** → FMC

## 2. QSPI应用示例-STM32IDE

### 2.1 STM32Cube配置

![屏幕截图 2025-09-11 140940.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/09/11-14-48-21-屏幕截图%202025-09-11%20140940.png)

### 2.2 用户代码

```c
#include "quadspi.h"

QSPI_HandleTypeDef g_qspi_handle;    /* QSPI句柄 */

/**
 * @brief       等待状态标志
 * @param       flag : 需要等待的标志位
 * @param       sta  : 需要等待的状态
 * @param       wtime: 等待时间
 * @retval      0, 等待成功; 1, 等待失败.
 */
uint8_t qspi_wait_flag(uint32_t flag, uint8_t sta, uint32_t wtime)
{
    uint8_t flagsta = 0;

    while (wtime)
    {
        flagsta = (QUADSPI->SR & flag) ? 1 : 0; /* 获取状态标志 */

        if (flagsta == sta)break;

        wtime--;
    }

    if (wtime)return 0;
    else return 1;
}

/**
 * @brief       初始化QSPI接口
 * @param       无
 * @retval      0, 成功; 1, 失败.
 */
uint8_t qspi_init(void)
{
    g_qspi_handle.Instance = QUADSPI;                                  /* QSPI */
    g_qspi_handle.Init.ClockPrescaler = 2;                             /* QPSI分频比，BY25Q128最大频率为108M，
                                                                          所以此处应该为2，QSPI频率就为220/(1+1)=110MHZ
                                                                          稍微有点超频，可以正常就好，不行就只能降低频率 */
    g_qspi_handle.Init.FifoThreshold = 4;                              /* FIFO阈值为4个字节 */
    g_qspi_handle.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;/* 采样移位半个周期(DDR模式下,必须设置为0) */
    g_qspi_handle.Init.FlashSize = 25-1;                               /* SPI FLASH大小，BY25Q128大小为32M字节,2^25，所以取权值25-1=24 */
    g_qspi_handle.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_3_CYCLE; /* 片选高电平时间为3个时钟(9.1*3=27.3ns),即手册里面的tSHSL参数 */
    g_qspi_handle.Init.ClockMode = QSPI_CLOCK_MODE_3;                  /* 模式3 */
    g_qspi_handle.Init.FlashID = QSPI_FLASH_ID_1;                      /* 第一片flash */
    g_qspi_handle.Init.DualFlash = QSPI_DUALFLASH_DISABLE;             /* 禁止双闪存模式 */

    if(HAL_QSPI_Init(&g_qspi_handle) == HAL_OK)
    {
        return 0;      /* QSPI初始化成功 */
    }
    else
    {
        return 1;
    }
}

/**
 * @brief       QSPI底层驱动,引脚配置，时钟使能
 * @param       hqspi:QSPI句柄
 * @note        此函数会被HAL_QSPI_Init()调用
 * @retval      0, 成功; 1, 失败.
 */
void HAL_QSPI_MspInit(QSPI_HandleTypeDef *hqspi)
{
    GPIO_InitTypeDef gpio_init_struct;

    __HAL_RCC_QSPI_CLK_ENABLE();      /* 使能QSPI时钟 */
    __HAL_RCC_GPIOB_CLK_ENABLE();     /* GPIOB时钟使能 */
    __HAL_RCC_GPIOD_CLK_ENABLE();     /* GPIOD时钟使能 */
    __HAL_RCC_GPIOE_CLK_ENABLE();     /* GPIOE时钟使能 */

    gpio_init_struct.Pin = QSPI_BK1_NCS_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;                     /* 复用 */
    gpio_init_struct.Pull = GPIO_PULLUP;                         /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;          /* 高速 */
    gpio_init_struct.Alternate = GPIO_AF10_QUADSPI;              /* 复用为QSPI */
    HAL_GPIO_Init(QSPI_BK1_NCS_GPIO_PORT, &gpio_init_struct);    /* 初始化QSPI_BK1_NCS引脚 */

    gpio_init_struct.Pin = QSPI_BK1_CLK_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;                     /* 复用 */
    gpio_init_struct.Pull = GPIO_PULLUP;                         /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;          /* 高速 */
    gpio_init_struct.Alternate = GPIO_AF9_QUADSPI;               /* 复用为QSPI */
    HAL_GPIO_Init(QSPI_BK1_CLK_GPIO_PORT, &gpio_init_struct);    /* 初始化QSPI_BK1_CLK引脚 */

    gpio_init_struct.Pin = QSPI_BK1_IO0_GPIO_PIN;
    HAL_GPIO_Init(QSPI_BK1_IO0_GPIO_PORT, &gpio_init_struct);    /* 初始化QSPI_BK1_IO0引脚 */

    gpio_init_struct.Pin = QSPI_BK1_IO1_GPIO_PIN;
    HAL_GPIO_Init(QSPI_BK1_IO1_GPIO_PORT, &gpio_init_struct);    /* 初始化QSPI_BK1_IO1引脚 */

    gpio_init_struct.Pin = QSPI_BK1_IO2_GPIO_PIN;
    HAL_GPIO_Init(QSPI_BK1_IO2_GPIO_PORT, &gpio_init_struct);    /* 初始化QSPI_BK1_IO2引脚 */

    gpio_init_struct.Pin = QSPI_BK1_IO3_GPIO_PIN;
    HAL_GPIO_Init(QSPI_BK1_IO3_GPIO_PORT, &gpio_init_struct);    /* 初始化QSPI_BK1_IO3引脚 */
}

/**
 * @brief       QSPI发送命令
 * @param       cmd : 要发送的指令
 * @param       addr: 发送到的目的地址
 * @param       mode: 模式,详细位定义如下:
 *   @arg       mode[1:0]: 指令模式; 00,无指令;  01,单线传输指令; 10,双线传输指令; 11,四线传输指令.
 *   @arg       mode[3:2]: 地址模式; 00,无地址;  01,单线传输地址; 10,双线传输地址; 11,四线传输地址.
 *   @arg       mode[5:4]: 地址长度; 00,8位地址; 01,16位地址;     10,24位地址;     11,32位地址.
 *   @arg       mode[7:6]: 数据模式; 00,无数据;  01,单线传输数据; 10,双线传输数据; 11,四线传输数据.
 * @param       dmcycle: 空指令周期数
 * @retval      无
 */
void qspi_send_cmd(uint8_t cmd, uint32_t addr, uint8_t mode, uint8_t dmcycle)
{
    QSPI_CommandTypeDef qspi_command_handle;

    qspi_command_handle.Instruction = cmd;                              /* 指令 */
    qspi_command_handle.Address = addr;                                 /* 地址 */
    qspi_command_handle.DummyCycles = dmcycle;                          /* 设置空指令周期数 */

    if(((mode >> 0) & 0x03) == 0)
    qspi_command_handle.InstructionMode = QSPI_INSTRUCTION_NONE;        /* 指令模式 */
    else if(((mode >> 0) & 0x03) == 1)
    qspi_command_handle.InstructionMode = QSPI_INSTRUCTION_1_LINE;      /* 指令模式 */
    else if(((mode >> 0) & 0x03) == 2)
    qspi_command_handle.InstructionMode = QSPI_INSTRUCTION_2_LINES;     /* 指令模式 */
    else if(((mode >> 0) & 0x03) == 3)
    qspi_command_handle.InstructionMode = QSPI_INSTRUCTION_4_LINES;     /* 指令模式 */

    if(((mode >> 2) & 0x03) == 0)
    qspi_command_handle.AddressMode = QSPI_ADDRESS_NONE;                /* 地址模式 */
    else if(((mode >> 2) & 0x03) == 1)
    qspi_command_handle.AddressMode = QSPI_ADDRESS_1_LINE;              /* 地址模式 */
    else if(((mode >> 2) & 0x03) == 2)
    qspi_command_handle.AddressMode = QSPI_ADDRESS_2_LINES;             /* 地址模式 */
    else if(((mode >> 2) & 0x03) == 3)
    qspi_command_handle.AddressMode = QSPI_ADDRESS_4_LINES;             /* 地址模式 */

    if(((mode >> 4)&0x03) == 0)
    qspi_command_handle.AddressSize = QSPI_ADDRESS_8_BITS;              /* 地址长度 */
    else if(((mode >> 4) & 0x03) == 1)
    qspi_command_handle.AddressSize = QSPI_ADDRESS_16_BITS;             /* 地址长度 */
    else if(((mode >> 4) & 0x03) == 2)
    qspi_command_handle.AddressSize = QSPI_ADDRESS_24_BITS;             /* 地址长度 */
    else if(((mode >> 4) & 0x03) == 3)
    qspi_command_handle.AddressSize = QSPI_ADDRESS_32_BITS;             /* 地址长度 */

    if(((mode >> 6) & 0x03) == 0)
    qspi_command_handle.DataMode=QSPI_DATA_NONE;                        /* 数据模式 */
    else if(((mode >> 6) & 0x03) == 1)
    qspi_command_handle.DataMode = QSPI_DATA_1_LINE;                    /* 数据模式 */
    else if(((mode >> 6) & 0x03) == 2)
    qspi_command_handle.DataMode = QSPI_DATA_2_LINES;                   /* 数据模式 */
    else if(((mode >> 6) & 0x03) == 3)
    qspi_command_handle.DataMode = QSPI_DATA_4_LINES;                   /* 数据模式 */

    qspi_command_handle.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;            /* 每次都发送指令 */
    qspi_command_handle.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;  /* 无交替字节 */
    qspi_command_handle.DdrMode = QSPI_DDR_MODE_DISABLE;                /* 关闭DDR模式 */
    qspi_command_handle.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;

    HAL_QSPI_Command(&g_qspi_handle, &qspi_command_handle, 5000);
}

/**
 * @brief       QSPI接收指定长度的数据
 * @param       buf     : 接收数据缓冲区首地址
 * @param       datalen : 要传输的数据长度
 * @retval      0, 成功; 其他, 错误代码.
 */
uint8_t qspi_receive(uint8_t *buf, uint32_t datalen)
{
    g_qspi_handle.Instance->DLR = datalen - 1;   /* 配置数据长度 */
    if (HAL_QSPI_Receive(&g_qspi_handle, buf, 5000) == HAL_OK)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
 * @brief       QSPI发送指定长度的数据
 * @param       buf     : 发送数据缓冲区首地址
 * @param       datalen : 要传输的数据长度
 * @retval      0, 成功; 其他, 错误代码.
 */
uint8_t qspi_transmit(uint8_t *buf, uint32_t datalen)
{
    g_qspi_handle.Instance->DLR = datalen - 1; /* 配置数据长度 */
    if (HAL_QSPI_Transmit(&g_qspi_handle, buf, 5000) == HAL_OK)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
 * @brief       QSPI进入内存映射模式（执行QSPI代码必备前提）
 *   @note      必须根据所使用QSPI FLASH的容量设置正确的ftype值!
 * @param       ftype: flash类型
 *   @arg           0, 普通FLASH, 容量在128Mbit及以内的
 *   @arg           1, 大容量FLASH, 容量在256Mbit及以上的.
 * @retval      无
 */
void sys_qspi_enable_memmapmode(uint8_t ftype)
{
    uint32_t tempreg = 0;
    GPIO_InitTypeDef qspi_gpio;

    __HAL_RCC_GPIOB_CLK_ENABLE();                            /* 使能PORTB时钟 */
    __HAL_RCC_GPIOD_CLK_ENABLE();                            /* 使能PORTD时钟 */
    __HAL_RCC_GPIOE_CLK_ENABLE();                            /* 使能PORTE时钟 */
    __HAL_RCC_QSPI_CLK_ENABLE();                             /* QSPI时钟使能 */

    qspi_gpio.Pin = GPIO_PIN_6;                              /* PB6 AF10 */
    qspi_gpio.Mode = GPIO_MODE_AF_PP;
    qspi_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    qspi_gpio.Pull = GPIO_PULLUP;
    qspi_gpio.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOB, &qspi_gpio);

    qspi_gpio.Pin = GPIO_PIN_2;                              /* PB2 AF9 */
    qspi_gpio.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOB, &qspi_gpio);

    qspi_gpio.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13; /* PD11,12,13 AF9 */
    qspi_gpio.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOD, &qspi_gpio);

    qspi_gpio.Pin = GPIO_PIN_2;                              /* PE2 AF9 */
    qspi_gpio.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOE, &qspi_gpio);

    /* QSPI设置，参考QSPI实验的QSPI_Init函数 */
    RCC->AHB3RSTR |= 1 << 14;       /* 复位QSPI */
    RCC->AHB3RSTR &= ~(1 << 14);    /* 停止复位QSPI */

    while (QUADSPI->SR & (1 << 5)); /* 等待BUSY位清零 */

    /* QSPI时钟源已经在sys_stm32_clock_init()函数中设置 */
    QUADSPI->CR = 0X01000310;       /* 设置CR寄存器, 这些值怎么来的，请参考QSPI实验/看H750参考手册寄存器描述分析 */
    QUADSPI->DCR = 0X00180201;      /* 设置DCR寄存器(FLASH容量32M(最大容量设置为32M, 默认用16M的), tSHSL = 3个时钟) */
    QUADSPI->CR |= 1 << 0;          /* 使能QSPI */

    /*
     *  注意:QSPI QE位的使能，在QSPI烧写算法里面，就已经设置了
     *  所以, 这里可以不用设置QE位，否则需要加入对QE位置1的代码
     *  不过, 代码必须通过仿真器下载, 直接烧录到外部QSPI FLASH, 是不可用的
     *  如果想直接烧录到外部QSPI FLASH也可以用, 则需要在这里添加QE位置1的代码
     *
     *  另外, 对与W25Q256,还需要使能4字节地址模式,或者设置S3的ADP位为1.
     *  我们在QSPI烧写算法里面已经设置了ADP=1(上电即32位地址模式),因此这里也
     *  不需要发送进入4字节地址模式指令/设置ADP=1了, 否则还需要设置ADP=1
     */

    /* BY/W25QXX 写使能（0X06指令）*/
    while (QUADSPI->SR & (1 << 5)); /* 等待BUSY位清零 */

    QUADSPI->CCR = 0X00000106;      /* 发送0X06指令，BY/W25QXX写使能 */

    while ((QUADSPI->SR & (1 << 1)) == 0);/* 等待指令发送完成 */

    QUADSPI->FCR |= 1 << 1;         /* 清除发送完成标志位 */

    /* MemroyMap 模式设置 */
    while (QUADSPI->SR & (1 << 5)); /* 等待BUSY位清零 */

    QUADSPI->ABR = 0;               /* 交替字节设置为0，实际上就是25QXX 0XEB指令的, M0~M7 = 0 */
    tempreg = 0XEB;                 /* INSTRUCTION[7:0] = 0XEB, 发送0XEB指令（Fast Read QUAD I/O） */
    tempreg |= 1 << 8;              /* IMODE[1:0] = 1, 单线传输指令 */
    tempreg |= 3 << 10;             /* ADDRESS[1:0] = 3, 四线传输地址 */
    tempreg |= (2 + ftype) << 12;   /* ADSIZE[1:0] = 2/3, 24位(ftype = 0) / 32位(ftype = 1)地址长度 */
    tempreg |= 3 << 14;             /* ABMODE[1:0] = 3, 四线传输交替字节 */
    tempreg |= 0 << 16;             /* ABSIZE[1:0] = 0, 8位交替字节(M0~M7) */
    tempreg |= 4 << 18;             /* DCYC[4:0] = 4, 4个dummy周期 */
    tempreg |= 3 << 24;             /* DMODE[1:0] = 3, 四线传输数据 */
    tempreg |= 3 << 26;             /* FMODE[1:0] = 3, 内存映射模式 */
    QUADSPI->CCR = tempreg;         /* 设置CCR寄存器 */

    /* 设置QSPI FLASH空间的MPU保护 */
    SCB->SHCSR &= ~(1 << 16);       /* 禁止MemManage */
    MPU->CTRL &= ~(1 << 0);         /* 禁止MPU */
    MPU->RNR = 0;                   /* 设置保护区域编号为0(1~7可以给其他内存用) */
    MPU->RBAR = 0X90000000;         /* 基地址为0X9000 000, 即QSPI的起始地址 */
    MPU->RASR = 0X0303002D;         /* 设置相关保护参数(禁止共用, 允许cache, 允许缓冲), 详见MPU实验的解析 */
    MPU->CTRL = (1 << 2) | (1 << 0);/* 使能PRIVDEFENA, 使能MPU */
    SCB->SHCSR |= 1 << 16;          /* 使能MemManage */
}

```

```c
#include "quadspi.h"
#include "flash.h"
#include "delay.h"

uint16_t g_norflash_type = W25Q128;     /* 默认是W25Q128 */

/* SPI FLASH 地址位宽 */
volatile uint8_t g_norflash_addrw = 2;  /* SPI FLASH地址位宽, 在norflash_read_id函数里面被修改
                                         * 2, 表示24bit地址宽度
                                         * 3, 表示32bit地址宽度
                                         */

/**
 * @brief       初始化SPI NOR FLASH
 * @param       无
 * @retval      无
 */
void norflash_init(void)
{
    uint8_t temp;
    qspi_init();                /* 初始化QSPI */
    norflash_qspi_disable();    /* 退出QPI模式(避免芯片之前进入这个模式,导致下载失败) */
    norflash_qe_enable();       /* 使能QE位 */
    g_norflash_type = norflash_read_id();/* 读取FLASH ID. */

    if (g_norflash_type == W25Q256)   /* SPI FLASH为W25Q256, 必须使能4字节地址模式 */
    {
        temp = norflash_read_sr(3); /* 读取状态寄存器3，判断地址模式 */

        if ((temp & 0X01) == 0)     /* 如果不是4字节地址模式,则进入4字节地址模式 */
        {
            norflash_write_enable();/* 写使能 */
            temp |= 1 << 1;         /* ADP=1, 上电4位地址模式 */
            norflash_write_sr(3, temp);  /* 写SR3 */

             norflash_write_enable();/* 写使能 */
             /* QPI,使能4字节地址指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据 */
            qspi_send_cmd(FLASH_Enable4ByteAddr, 0, (0 << 6) | (0 << 4) | (0 << 2) | (1 << 0), 0);

        }
    }

    //printf("ID:%x\r\n", g_norflash_type);
}

/**
 * @brief       等待空闲
 * @param       无
 * @retval      无
 */
static void norflash_wait_busy(void)
{
    while ((norflash_read_sr(1) & 0x01) == 0x01);   /*  等待BUSY位清空 */
}

/**
 * @brief       退出QSPI模式
 * @param       无
 * @retval      无
 */
static void norflash_qspi_disable(void)
{
    /* 写command指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据 */
    qspi_send_cmd(FLASH_ExitQPIMode, 0, (0 << 6) | (0 << 4) | (0 << 2) | (3 << 0), 0);
}

/**
 * @brief       使能FLASH QE位，使能IO2/IO3
 * @param       无
 * @retval      无
 */
static void norflash_qe_enable(void)
{
    uint8_t stareg2 = 0;
    stareg2 = norflash_read_sr(2);  /* 先读出状态寄存器2的原始值 */

    //printf("stareg2:%x\r\n", stareg2);
    if ((stareg2 & 0X02) == 0)      /* QE位未使能 */
    {
        norflash_write_enable();    /* 写使能 */
        stareg2 |= 1 << 1;          /* 使能QE位 */
        norflash_write_sr(2, stareg2);  /* 写状态寄存器2 */
    }
}

/**
 * @brief       25QXX写使能
 *   @note      将S1寄存器的WEL置位
 * @param       无
 * @retval      无
 */
void norflash_write_enable(void)
{
    /* SPI,写使能指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据 */
    qspi_send_cmd(FLASH_WriteEnable, 0, (0 << 6) | (0 << 4) | (0 << 2) | (1 << 0), 0);
}

/**
 * @brief       25QXX写禁止
 *   @note      将S1寄存器的WEL清零
 * @param       无
 * @retval      无
 */
void norflash_write_disable(void)
{
    /* SPI,写禁止指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据 */
    qspi_send_cmd(FLASH_WriteDisable, 0, (0 << 6) | (0 << 4) | (0 << 2) | (1 << 0), 0);
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
 * @param       regno: 状态寄存器号，范围:1~3
 * @retval      状态寄存器值
 */
uint8_t norflash_read_sr(uint8_t regno)
{
    uint8_t byte = 0, command = 0;

    switch (regno)
    {
        case 1:
            command = FLASH_ReadStatusReg1;  /* 读状态寄存器1指令 */
            break;

        case 2:
            command = FLASH_ReadStatusReg2;  /* 读状态寄存器2指令 */
            break;

        case 3:
            command = FLASH_ReadStatusReg3;  /* 读状态寄存器3指令 */
            break;

        default:
            command = FLASH_ReadStatusReg1;
            break;
    }

    /* SPI,写command指令,地址为0,单线传数据_8位地址_无地址_单线传输指令,无空周期,1个字节数据 */
    qspi_send_cmd(command, 0, (1 << 6) | (0 << 4) | (0 << 2) | (1 << 0), 0);
    qspi_receive(&byte, 1);
    return byte;
}

/**
 * @brief       写25QXX状态寄存器
 *   @note      寄存器说明见norflash_read_sr函数说明
 * @param       regno: 状态寄存器号，范围:1~3
 * @param       sr   : 要写入状态寄存器的值
 * @retval      无
 */
void norflash_write_sr(uint8_t regno, uint8_t sr)
{
    uint8_t command = 0;

    switch (regno)
    {
        case 1:
            command = FLASH_WriteStatusReg1;  /* 写状态寄存器1指令 */
            break;

        case 2:
            command = FLASH_WriteStatusReg2;  /* 写状态寄存器2指令 */
            break;

        case 3:
            command = FLASH_WriteStatusReg3;  /* 写状态寄存器3指令 */
            break;

        default:
            command = FLASH_WriteStatusReg1;
            break;
    }

    /* SPI,写command指令,地址为0,单线传数据_8位地址_无地址_单线传输指令,无空周期,1个字节数据 */
    qspi_send_cmd(command, 0, (1 << 6) | (0 << 4) | (0 << 2) | (1 << 0), 0);
    qspi_transmit(&sr, 1);
}

/**
 * @brief       读取芯片ID
 * @param       无
 * @retval      FLASH芯片ID
 *   @note      芯片ID列表见: norflash.h, 芯片列表部分
 */
uint16_t norflash_read_id(void)
{
    uint8_t temp[2];
    uint16_t deviceid;
    qspi_init();          /* 进行库函数调用前要先初始化 */
    /* SPI,读id,地址为0,单线传输数据_24位地址_单线传输地址_单线传输指令,无空周期,2个字节数据 */
    qspi_send_cmd(FLASH_ManufactDeviceID, 0, (1 << 6) | (2 << 4) | (1 << 2) | (1 << 0), 0);
    qspi_receive(temp, 2);
    deviceid = (temp[0] << 8) | temp[1];

    if (deviceid == W25Q256)
    {
        g_norflash_addrw = 3;   /* 如果是W25Q256, 标记32bit地址宽度 */
    }

    return deviceid;
}

/**
 * @brief       读取SPI FLASH,仅支持QSPI模式
 *   @note      在指定地址开始读取指定长度的数据
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始读取的地址(最大32bit)
 * @param       datalen : 要读取的字节数(最大65535)
 * @retval      无
 */
void norflash_read(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    /* QSPI,快速读数据,地址为addr,4线传输数据_24/32位地址_4线传输地址_1线传输指令,6空周期,datalen个数据 */
    qspi_send_cmd(FLASH_FastReadQuad, addr, (3 << 6) | (g_norflash_addrw << 4) | (3 << 2) | (1 << 0), 6);
    qspi_receive(pbuf, datalen);
}

/**
 * @brief       SPI在一页(0~65535)内写入少于256个字节的数据
 *   @note      在指定地址开始写入最大256字节的数据
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始写入的地址(最大32bit)
 * @param       datalen : 要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
 * @retval      无
 */
static void norflash_write_page(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    norflash_write_enable();        /* 写使能 */

    /* QSPI,页写指令,地址为addr,4线传输数据_24/32位地址_1线传输地址_1线传输指令,无空周期,datalen个数据 */
    qspi_send_cmd(FLASH_PageProgramQuad, addr, (3 << 6) | (g_norflash_addrw << 4) | (1 << 2) | (1 << 0), 0);

    qspi_transmit(pbuf, datalen);   /* 发送数据 */
    norflash_wait_busy();           /* 等待写入结束 */
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
static void norflash_write_nocheck(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t pageremain;
    pageremain = 256 - addr % 256;  /* 单页剩余的字节数 */

    if (datalen <= pageremain)      /* 不大于256个字节 */
    {
        pageremain = datalen;
    }

    while (1)
    {
        /* 当写入字节比页内剩余地址还少的时候, 一次性写完
         * 当写入直接比页内剩余地址还多的时候, 先写完整个页内剩余地址, 然后根据剩余长度进行不同处理
         */
        norflash_write_page(pbuf, addr, pageremain);

        if (datalen == pageremain)   /* 写入结束了 */
        {
            break;
        }
        else     /* datalen > pageremain */
        {
            pbuf += pageremain;         /* pbuf指针地址偏移,前面已经写了pageremain字节 */
            addr += pageremain;         /* 写地址偏移,前面已经写了pageremain字节 */
            datalen -= pageremain;      /* 写入总长度减去已经写入了的字节数 */

            if (datalen > 256)          /* 剩余数据还大于一页,可以一次写一页 */
            {
                pageremain = 256;       /* 一次可以写入256个字节 */
            }
            else     /* 剩余数据小于一页,可以一次写完 */
            {
                pageremain = datalen;   /* 不够256个字节了 */
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

void norflash_write(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;
    uint8_t *norflash_buf;

    norflash_buf = g_norflash_buf;
    secpos = addr / 4096;       /* 扇区地址 */
    secoff = addr % 4096;       /* 在扇区内的偏移 */
    secremain = 4096 - secoff;  /* 扇区剩余空间大小 */

    //printf("ad:%X,nb:%X\r\n", addr, datalen); /* 测试用 */
    if (datalen <= secremain)
    {
        secremain = datalen;    /* 不大于4096个字节 */
    }

    while (1)
    {
        norflash_read(norflash_buf, secpos * 4096, 4096);   /* 读出整个扇区的内容 */

        for (i = 0; i < secremain; i++)   /* 校验数据 */
        {
            if (norflash_buf[secoff + i] != 0XFF)
            {
                break;      /* 需要擦除, 直接退出for循环 */
            }
        }

        if (i < secremain)   /* 需要擦除 */
        {
            norflash_erase_sector(secpos);  /* 擦除这个扇区 */

            for (i = 0; i < secremain; i++)   /* 复制 */
            {
                norflash_buf[i + secoff] = pbuf[i];
            }

            norflash_write_nocheck(norflash_buf, secpos * 4096, 4096);  /* 写入整个扇区 */
        }
        else        /* 写已经擦除了的,直接写入扇区剩余区间. */
        {
            norflash_write_nocheck(pbuf, addr, secremain);  /* 直接写扇区 */
        }

        if (datalen == secremain)
        {
            break;  /* 写入结束了 */
        }
        else        /* 写入未结束 */
        {
            secpos++;               /* 扇区地址增1 */
            secoff = 0;             /* 偏移位置为0 */

            pbuf += secremain;      /* 指针偏移 */
            addr += secremain;      /* 写地址偏移 */
            datalen -= secremain;   /* 字节数递减 */

            if (datalen > 4096)
            {
                secremain = 4096;   /* 下一个扇区还是写不完 */
            }
            else
            {
                secremain = datalen;/* 下一个扇区可以写完了 */
            }
        }
    }
}

/**
 * @brief       擦除整个芯片
 *   @note      等待时间超长...
 * @param       无
 * @retval      无
 */
void norflash_erase_chip(void)
{
    norflash_write_enable();    /* 写使能 */
    norflash_wait_busy();       /* 等待空闲 */
    /* QPI,写全片擦除指令,地址为0,无数据_8位地址_无地址_1线传输指令,无空周期,0个字节数据 */
    qspi_send_cmd(FLASH_ChipErase, 0, (0 << 6) | (0 << 4) | (0 << 2) | (1 << 0), 0);
    norflash_wait_busy();       /* 等待芯片擦除结束 */
}

/**
 * @brief       擦除一个扇区
 *   @note      注意,这里是扇区地址,不是字节地址!!
 *              擦除一个扇区的最少时间:150ms
 *
 * @param       saddr : 扇区地址 根据实际容量设置
 * @retval      无
 */
void norflash_erase_sector(uint32_t saddr)
{
    //printf("fe:%x\r\n", saddr);   /* 监视falsh擦除情况,测试用 */
    saddr *= 4096;
    norflash_write_enable();        /* 写使能 */
    norflash_wait_busy();           /* 等待空闲 */

    /* QPI,写扇区擦除指令,地址为0,无数据_24/32位地址_1线传输地址_1线传输指令,无空周期,0个字节数据 */
    qspi_send_cmd(FLASH_SectorErase, saddr, (0 << 6) | (g_norflash_addrw << 4) | (1 << 2) | (1 << 0), 0);

    norflash_wait_busy();           /* 等待擦除完成 */
}

```

```c
 /* 因为STM32H7不支持QSPI接口读时写,因此该代码用于实现QSPI FLASH的数据写入,原理是：
 *  qspi.c、flash.c和flash_ex.c等3部分代码全部存储在H7的内部FLASH,保证操作
 *  QSPI FLASH的时候,不需要访问QSPI FLASH的代码. 从而实现QSPI FLASH数据写入.
 */
#include "quadspi.h"
#include "flash.h"
#include "flash_ex.h"

extern uint8_t g_norflash_addrw;    /* 表示当前是24bit/32bit数据位宽, 在norflash.c里面定义 */

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
 * @brief       QSPI接口进入内存映射模式
 *   @note      调用该函数之前务必已经初始化了QSPI接口
 *              sys_qspi_enable_memmapmode or norflash_init
 * @param       无
 * @retval      无
 */
static void norflash_ex_enter_mmap(void)
{
    uint32_t tempreg = 0;

    /* BY/W25QXX 写使能（0X06指令） */
    while (QUADSPI->SR & (1 << 5)); /* 等待BUSY位清零 */

    QUADSPI->CCR = 0X00000106;      /* 发送0X06指令，BY/W25QXX写使能 */

    while ((QUADSPI->SR & (1 << 1)) == 0);  /* 等待指令发送完成 */

    QUADSPI->FCR |= 1 << 1;

    if (qspi_wait_flag(1 << 5, 0, 0XFFFF) == 0) /* 等待BUSY空闲 */
    {
        tempreg = 0XEB;         /* INSTRUCTION[7:0]=0XEB,发送0XEB指令（Fast Read QUAD I/O） */
        tempreg |= 1 << 8;      /* IMODE[1:0]=1,单线传输指令 */
        tempreg |= 3 << 10;     /* ADDRESS[1:0]=3,四线传输地址 */
        tempreg |= (uint32_t)g_norflash_addrw << 12;    /* ADSIZE[1:0]=2,24/32位地址长度 */
        tempreg |= 3 << 14;     /* ABMODE[1:0]=3,四线传输交替字节 */
        tempreg |= 0 << 16;     /* ABSIZE[1:0]=0,8位交替字节(M0~M7) */
        tempreg |= 4 << 18;     /* DCYC[4:0]=4,4个dummy周期 */
        tempreg |= 3 << 24;     /* DMODE[1:0]=3,四线传输数据 */
        tempreg |= 3 << 26;     /* FMODE[1:0]=3,内存映射模式 */
        QUADSPI->CCR = tempreg; /* 设置CCR寄存器 */
    }
    sys_intx_enable();          /* 开启中断 */
}

/**
 * @brief       QSPI接口退出内存映射模式
 *   @note      调用该函数之前务必已经初始化了QSPI接口
 *              sys_qspi_enable_memmapmode or norflash_init
 * @param       无
 * @retval      0, OK;  其他, 错误代码
 */
static uint8_t norflash_ex_exit_mmap(void)
{
    uint8_t res = 0;

    sys_intx_disable();         /* 关闭中断 */
    SCB_InvalidateICache();     /* 清空I CACHE */
    SCB_InvalidateDCache();     /* 清空D CACHE */
    QUADSPI->CR &= ~(1 << 0);   /* 关闭 QSPI 接口 */
    QUADSPI->CR |= 1 << 1;      /* 退出MEMMAPED模式 */
    res = qspi_wait_flag(1 << 5, 0, 0XFFFF);    /* 等待BUSY空闲 */

    if (res == 0)
    {
        QUADSPI->CCR = 0;       /* CCR寄存器清零 */
        QUADSPI->CR |= 1 << 0;  /* 使能 QSPI 接口 */
    }

    return res;
}

/**
 * @brief       往 QSPI FLASH写入数据
 *   @note      在指定地址开始写入指定长度的数据
 *              该函数带擦除操作!
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始写入的地址(最大32bit)
 * @param       datalen : 要写入的字节数(最大65535)
 * @retval      0, OK;  其他, 错误代码
 */
uint8_t norflash_ex_write(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint8_t res = 0;
    res = norflash_ex_exit_mmap();  /* 退出内存映射模式 */

    if (res == 0)
    {
        norflash_write(pbuf, addr, datalen);
    }

    norflash_ex_enter_mmap();       /* 进入内存映射模式 */
    return res;
}

/**
 * @brief       从 QSPI FLASH 读取数据
 *   @note      在指定地址开始读取指定长度的数据（必须处于内存映射模式下，才可以执行）
 *
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始读取的地址(最大32bit)
 * @param       datalen : 要读取的字节数(最大65535)
 * @retval      0, OK;  其他, 错误代码
 */
void norflash_ex_read(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t i = 0;
    addr += 0X90000000;     /* 使用内存映射模式读取，QSPI的基址是0X90000000，所以这里要加上基址 */
    sys_intx_disable();     /* 关闭中断 */

    for (i = 0; i < datalen; i++)
    {
        pbuf[i] = *(volatile uint8_t *)(addr + i);
    }

    sys_intx_enable();      /* 开启中断 */
}

/**
 * @brief       读取QSPI FLASH的ID
 * @param       无
 * @retval      NOR FLASH ID
 */
uint16_t norflash_ex_read_id(void)
{
    uint8_t res = 0;
    uint16_t id = 0;
    res = norflash_ex_exit_mmap();  /* 退出内存映射模式 */

    if (res == 0)
    {
        id = norflash_read_id();
    }

    norflash_ex_enter_mmap();       /* 进入内存映射模式 */
    return id;
}

/**
 * @brief       擦除QSPI FLASH的某个扇区
 *   @note      注意,这里是扇区地址,不是字节地址!!
 *              擦除一个扇区的最少时间:150ms
 *
 * @param       saddr: 扇区地址
 * @retval      无
 */
void norflash_ex_erase_sector(uint32_t addr)
{
    uint8_t res = 0;
    res = norflash_ex_exit_mmap();  /* 退出内存映射模式 */

    if (res == 0)
    {
        norflash_erase_sector(addr);
    }

    norflash_ex_enter_mmap();       /* 进入内存映射模式 */
}

/**
 * @brief       擦除QSPI FLASH整个芯片
 *   @note      等待时间超长...
 *
 * @param       无
 * @retval      无
 */
void norflash_ex_erase_chip(void)
{
    uint8_t res = 0;
    res = norflash_ex_exit_mmap();  /* 退出内存映射模式 */

    if (res == 0)
    {
        norflash_erase_chip();
    }

    norflash_ex_enter_mmap();       /* 进入内存映射模式 */
}

```

```c
#include "main.h"
#include "bsp_init.h"
#include "quadspi.h"
#include "flash.h"
#include "flash_ex.h"
#include "delay.h"
#include <stdio.h>

void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MPU_Config(void);

// 要写入到FLASH的字符串
const uint8_t test_str[] = "Hello QSPI Flash!";
#define text_size sizeof(test_str)

int main(void)
{
  uint8_t temp_data[text_size];
  uint32_t flash_size;
  uint16_t flash_id;

  MPU_Config();
  HAL_Init();
  SystemClock_Config();
  PeriphCommonClock_Config();
  bsp_init();
  delay_init(480);
  sys_qspi_enable_memmapmode(0); /* 使能QSPI内存映射模式, FLASH容量为普通类型 */

  flash_id = norflash_ex_read_id(); /* 读取QSPI FLASH ID */
  printf("QSPI Flash ID: 0x%X\r\n", flash_id);

  while((flash_id == 0)||(flash_id == 0xFFFF)) // 检测不到flash芯片
  {
    printf("flash check failed!");
    HAL_Delay(500);
    HAL_GPIO_TogglePin(LED_RED_Port, LED_RED_Pin);
  }
  printf("QSPI FLASH Ready!\r\n");
  flash_size = 16 * 1024 *1024;

  while (1)
  {
    printf("Start Write FLASH....\r\n");
    delay_ms(1000);
    norflash_ex_write((uint8_t*)temp_data, flash_size-100, text_size);
    HAL_GPIO_WritePin(LED_GREEN_Port, LED_GREEN_Pin, RESET);
    delay_ms(1000);
    printf("Write Finished!\r\n");
    delay_ms(5000);

	printf("Start Read FLASH....\r\n");
	delay_ms(1000);
	norflash_ex_read(temp_data, flash_size-100, text_size);
	printf("Read Finished!\r\n");
	printf("Data:");
	printf((char*)temp_data);
	delay_ms(5000);
	delay_ms(2000);
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
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 240;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
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

## 3. QSPI相关函数总结（HAL库）

### 3.1 初始化与配置

- **核心配置流程**（五步关键操作）：
  
  1. **使能时钟**（QSPI + GPIO）
  2. **配置GPIO复用功能**（IO0-IO3, CLK, CS, DQS）
  3. **初始化QSPI参数**（模式/时钟分频等）
  4. **配置闪存参数**（大小/块保护等）
  5. **使能QSPI控制器**

- `HAL_QSPI_Init(QSPI_HandleTypeDef *hqspi)`  
  **基础配置示例**（四线SPI模式）：
  
  ```c
  // 1. 使能QSPI和GPIO时钟
  __HAL_RCC_QSPI_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  
  // 2. 配置GPIO（QSPI接口）
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  // CLK (PB2)
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_QSPI;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  // CS (PG6)
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Alternate = GPIO_AF10_QSPI;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  
  // IO0 (PF8), IO1 (PF9), IO2 (PF7), IO3 (PF6)
  GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
  GPIO_InitStruct.Alternate = GPIO_AF10_QSPI;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  
  // 3. 配置QSPI参数
  hqspi.Instance = QUADSPI;
  hqspi.Init.ClockPrescaler = 1;                     // QSPI CLK = 200MHz/2 = 100MHz
  hqspi.Init.FifoThreshold = 4;                      // FIFO阈值
  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;  // 半周期采样
  hqspi.Init.FlashSize = POSITION_VAL(0x2000000) - 1; // 32MB闪存
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_4_CYCLE;  // CS高时间
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;          // 时钟模式0
  hqspi.Init.FlashID = QSPI_FLASH_ID_1;              // 闪存ID
  hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;    // 禁用双闪存
  HAL_QSPI_Init(&hqspi);
  ```

- **`QSPI_InitTypeDef` 结构体成员说明**：
  
  | **成员**               | **说明**  | **关键选项**               | **H750特殊说明**                      |
  | -------------------- | ------- | ---------------------- | --------------------------------- |
  | `ClockPrescaler`     | 时钟预分频   | 1-255                  | `QSPI_CLK = 200MHz/(Prescaler+1)` |
  | `FifoThreshold`      | FIFO阈值  | 1-32                   | 影响DMA效率                           |
  | `SampleShifting`     | 采样偏移    | `HALFCYCLE`, `NOCYCLE` | 抗信号延迟                             |
  | `FlashSize`          | 闪存大小    | 0-31 (2^(size+1)字节)    | `POSITION_VAL(size)-1`            |
  | `ChipSelectHighTime` | CS高电平时间 | `1-8周期`                | 保证CS建立时间                          |
  | `ClockMode`          | 时钟模式    | `MODE0`, `MODE3`       | 匹配闪存要求                            |
  | `DualFlash`          | 双闪存     | `ENABLE`, `DISABLE`    | 双通道并行访问                           |

- **QSPI时钟配置**（核心！）：
  
  - **时钟源**：`QSPICLK = 200MHz`（来自PLL2）
  
  - **实际频率**：`QSPICLK / (ClockPrescaler + 1)`
  
  - **典型配置**（100MHz）：
    
    ```c
    hqspi.Init.ClockPrescaler = 1;  // 200MHz → 100MHz
    ```
  
  - **最大频率**：
    
    - 单线模式：≤ 133MHz
    - 四线模式：≤ 66MHz（建议≤ 100MHz确保稳定）

- **闪存参数配置**：
  
  ```c
  // 配置闪存（MX25L51245G为例）
  QSPI_CommandTypeDef sCommand = {0};
  sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
  ```

### 3.2 QSPI操作核心函数

- **基础命令操作**：
  
  | **函数**                   | **原型**                    | **特点** | **应用场景** |
  | ------------------------ | ------------------------- | ------ | -------- |
  | `HAL_QSPI_Command()`     | `(hqspi, *cmd, Timeout)`  | 发送指令   | 读写寄存器    |
  | `HAL_QSPI_Command_IT()`  | `(hqspi, *cmd)`           | 中断指令   | 非阻塞      |
  | `HAL_QSPI_Transmit()`    | `(hqspi, *data, Timeout)` | 发送数据   | 写操作      |
  | `HAL_QSPI_Transmit_IT()` | `(hqspi, *data)`          | 中断发送   |          |
  | `HAL_QSPI_Receive()`     | `(hqspi, *data, Timeout)` | 接收数据   | 读操作      |
  | `HAL_QSPI_Receive_IT()`  | `(hqspi, *data)`          | 中断接收   |          |

- **存储器映射模式**（关键高级功能）：
  
  ```c
  // 配置为存储器映射模式
  QSPI_MemoryMappedTypeDef sMemMappedCfg = {0};
  sMemMappedCfg.TimeOutPeriod = 1;
  sMemMappedCfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
  HAL_QSPI_MemoryMapped(&hqspi, &sMemMappedCfg);
  
  // 之后可像普通内存一样访问
  uint8_t data = *(uint8_t*)(QSPI_BASE_ADDR + 0x1000);
  memcpy(buffer, (void*)QSPI_BASE_ADDR, 1024);
  ```

- **直接写模式**（Direct Write）：
  
  ```c
  // 配置直接写
  HAL_QSPI_ConfigFlash(&hqspi, &sCommand, QSPI_FUNCTIONAL_MODE_DIRECT_WRITE);
  
  // 直接写入闪存
  HAL_QSPI_Transmit(&hqspi, write_buffer, 100);
  ```

- **状态检查与标志**：
  
  ```c
  // 检查忙状态
  uint8_t status;
  do {
      Read_Status_Register(&status);
  } while(status & 0x01);  // 忙位
  
  // 获取FIFO空/满状态
  if (__HAL_QSPI_GET_FLAG(&hqspi, QSPI_FLAG_FIFO_EMPTY)) {
      // FIFO为空
  }
  ```

### 3.3 高级功能与特性

- **DDR模式**（双倍数据速率）：
  
  ```c
  // 启用DDR模式（提高带宽）
  sCommand.DdrMode = QSPI_DDR_MODE_ENABLE;
  sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_HALF_CLK_DELAY;
  
  // 时钟模式必须为3
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_3;
  ```
  
  **性能对比**：

| **模式**    | **理论带宽** | **实际可用**    |
| --------- | -------- | ----------- |
| SDR 80MHz | 80Mbps   | 70-75Mbps   |
| DDR 80MHz | 160Mbps  | 140-150Mbps |

- **双闪存模式**（Dual Flash）：
  
  ```c
  // 启用双闪存模式
  hqspi.Init.DualFlash = QSPI_DUALFLASH_ENABLE;
  HAL_QSPI_Init(&hqspi);
  
  // 双闪存命令
  QSPI_CommandTypeDef sCommand = {0};
  sCommand.Instruction = 0x72;  // 双闪存页程序
  sCommand.AddressMode = QSPI_ADDRESS_4_LINES;
  sCommand.DataMode = QSPI_DATA_4_LINES;
  HAL_QSPI_Command(&hqspi, &sCommand, 5000);
  
  // 传输数据（两个闪存同时工作）
  HAL_QSPI_Transmit(&hqspi, dual_data_buffer, 256);
  ```

- **DQS信号使用**（数据选通信号）：
  
  ```c
  // 启用DQS（用于DDR模式）
  hqspi.Instance->DCR |= QUADSPI_DCR_DQS_EN;
  
  // 配置DQS延迟
  hqspi.Instance->LPTR = 0x10;  // 延迟值
  ```
  
  **DQS优势**：

- 提供精确的数据采样时钟

- 克服信号飞行时间差异

- 提高高速传输可靠性

- **Cache优化**（H7C0关键）：
  
  ```c
  // 启用QSPI Cache
  __HAL_RCC_QSPIM_RAM_CLK_ENABLE();
  hqspi.Instance->CR |= QUADSPI_CR_EN;  // 使能预取
  
  // 软件预取
  __IO uint32_t *ptr = (uint32_t*)QSPI_BASE_ADDR;
  uint32_t temp = *ptr;  // 触发预取
  ```

### 3.4 使用示例（完整流程）

#### 3.4.1 示例1：QSPI闪存读写操作

```c
QSPI_HandleTypeDef hqspi = {0};

// 1. 读取闪存ID
HAL_StatusTypeDef Read_Flash_ID(uint8_t *id)
{
    QSPI_CommandTypeDef sCommand = {0};

    // 配置读ID命令
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction = 0x9F;  // 读ID指令
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = QSPI_DATA_1_LINE;
    sCommand.NbData = 3;  // 3字节ID
    sCommand.DummyCycles = 0;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    return HAL_QSPI_Command(&hqspi, &sCommand, 1000);
}

// 2. 读取状态寄存器
HAL_StatusTypeDef Read_Status_Register(uint8_t *status)
{
    QSPI_CommandTypeDef sCommand = {0};

    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction = 0x05;  // 读状态寄存器
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.DataMode = QSPI_DATA_1_LINE;
    sCommand.NbData = 1;

    return HAL_QSPI_Command(&hqspi, &sCommand, 1000);
}

// 3. 页编程（写一页）
HAL_StatusTypeDef Page_Program(uint32_t address, uint8_t *data, uint32_t size)
{
    QSPI_CommandTypeDef sCommand = {0};

    // 使能写操作
    Write_Enable();

    // 配置页程序命令
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction = 0x02;  // 页程序
    sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
    sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
    sCommand.Address = address;
    sCommand.DataMode = QSPI_DATA_1_LINE;
    sCommand.NbData = size;
    sCommand.DummyCycles = 0;

    HAL_StatusTypeDef status = HAL_QSPI_Command(&hqspi, &sCommand, 1000);
    if (status != HAL_OK) return status;

    return HAL_QSPI_Transmit(&hqspi, data, 1000);
}

// 4. 读取数据
HAL_StatusTypeDef Read_Data(uint32_t address, uint8_t *data, uint32_t size)
{
    QSPI_CommandTypeDef sCommand = {0};

    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction = 0x0B;  // 快速读取（带4字节dummy）
    sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
    sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
    sCommand.Address = address;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = QSPI_DATA_1_LINE;
    sCommand.NbData = size;
    sCommand.DummyCycles = 8;  // 8个dummy周期
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    HAL_StatusTypeDef status = HAL_QSPI_Command(&hqspi, &sCommand, 1000);
    if (status != HAL_OK) return status;

    return HAL_QSPI_Receive(&hqspi, data, 1000);
}
```

#### 3.4.2 示例2：存储器映射模式使用

```c
// 1. 初始化QSPI（同上）
QSPI_Init();

// 2. 配置存储器映射模式
void QSPI_Memory_Mapped_Mode(void)
{
    QSPI_CommandTypeDef sCommand = {0};

    // 配置读命令
    sCommand.InstructionMode = QSPI_INSTRUCTION_4_LINES;
    sCommand.Instruction = 0xEB;  // 四线快速读取
    sCommand.AddressMode = QSPI_ADDRESS_4_LINES;
    sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_4_LINES;
    sCommand.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
    sCommand.AlternateBytes = 0xFF;  // 8个dummy周期
    sCommand.DataMode = QSPI_DATA_4_LINES;
    sCommand.DummyCycles = 6;  // 6个dummy周期
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.SIOOMode = QSPI_SIOO_INST_ONLY_FIRST_CMD;

    // 配置存储器映射
    QSPI_MemoryMappedTypeDef sMemMappedCfg = {0};
    sMemMappedCfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;

    HAL_QSPI_MemoryMapped(&hqspi, &sCommand, &sMemMappedCfg);
}

// 3. 使用示例（像普通内存一样访问）
void Memory_Mapped_Usage(void)
{
    QSPI_Memory_Mapped_Mode();

    // 从QSPI闪存执行代码
    // 跳转到QSPI映射地址的函数
    void (*app_entry)(void) = (void (*)(void))(QSPI_BASE_ADDR + 0x1000);
    app_entry();

    // 读取配置数据
    uint32_t config_value = *(uint32_t*)(QSPI_BASE_ADDR + 0x8000);

    // 复制固件到RAM
    memcpy((void*)0x20000000, (void*)QSPI_BASE_ADDR, 64*1024);
}
```

## 4. 关键注意事项

1. **时钟相位匹配**：
   
   - **QSPI Clock Mode 0**：
     
     - CPOL=0（空闲低）
     - CPHA=0（上升沿采样）
   
   - **QSPI Clock Mode 3**：
     
     - CPOL=1（空闲高）
     - CPHA=1（下降沿采样）
   
   - **必须匹配闪存要求**

2. **信号完整性要求**：
   
   | **信号**  | **要求** | **H750设计建议** |
   | ------- | ------ | ------------ |
   | CLK     | 长度匹配   | ±500mil      |
   | IO0-IO3 | 长度匹配   | ±300mil      |
   | CS      | 最短     | 优先布线         |
   | DQS     | 与IO等长  | DDR模式必需      |

3. **写保护与块保护**：
   
   ```c
   // 解锁写操作
   void Write_Enable(void)
   {
       QSPI_CommandTypeDef sCommand = {0};
       sCommand.Instruction = 0x06;
       sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
       HAL_QSPI_Command(&hqspi, &sCommand, 1000);
   }
   
   // 块保护配置
   uint8_t status_reg[2];
   status_reg[0] = 0x00;  // 清除块保护
   Page_Program(0x000000, status_reg, 1);
   ```

4. **Cache一致性**：
   
   - **写QSPI后**：
     
     ```c
     SCB_InvalidateDCache_by_Addr((uint32_t*)QSPI_BASE_ADDR, size);
     ```
   
   - **使用存储器映射**：
     
     ```c
     // 确保指令Cache更新
     SCB_InvalidateICache_by_Addr((uint32_t*)QSPI_BASE_ADDR, size);
     ```

5. **功耗优化**：
   
   - **未使用时关闭QSPI时钟**：
     
     ```c
     __HAL_RCC_QSPI_CLK_DISABLE();
     ```
   
   - **GPIO配置为模拟模式**：
     
     ```c
     GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
     HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
     HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
     HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
     ```

---

### 4.1 H750特有优化技巧

| **场景**    | **解决方案** | **性能提升** | **实现要点**            |
| --------- | -------- | -------- | ------------------- |
| **XIP执行** | 存储器映射    | 代码直接执行   | `SIOO_MODE`配置       |
| **高速下载**  | DDR+四线   | 速率↑2倍    | `DDR_MODE_ENABLE`   |
| **大容量存储** | 双闪存      | 容量×2     | `DUAL_FLASH_ENABLE` |
| **实时性保证** | Cache预取  | 延迟↓50%   | 软件触发预取              |

> **避坑指南**：
> 
> 1. **H750 QSPI时钟树**：
>    
>    - QSPI时钟必须来自**PLL2_Q**（200MHz）
>    
>    - 配置：
>      
>      ```c
>      GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
>      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
>      HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
>      HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
>      ```
> 
> 2. **FIFO使用陷阱**：
>    
>    - FIFO阈值影响传输效率
>    - **推荐值**：4-8
>    - 太小：频繁中断
>    - 太大：延迟增加
> 
> 3. **中断优先级**：
>    
>    - QSPI中断优先级应高于DMA中断
>    - 否则可能导致FIFO溢出
> 
> 4. **闪存兼容性**：
>    
>    - 不同厂商闪存指令集可能有差异
>    - 仔细核对**数据手册**
>    - 测试读写保护功能

---

### 4.2 QSPI性能参数对比

| **模式**    | **理论带宽** | **典型应用** | **CPU负载** |
| --------- | -------- | -------- | --------- |
| SDR 80MHz | 80Mbps   | 代码存储     | 低         |
| DDR 80MHz | 160Mbps  | 高速数据     | 极低        |
| 存储器映射     | 80MB/s   | XIP执行    | 0%        |
| DMA传输     | 70MB/s   | 大数据搬运    | 低         |

> **重要提示**：
> 
> - QSPI是**大容量非易失存储**的最佳选择
> - 存储器映射模式可实现**代码直接执行**（XIP）
> - DDR模式显著提高数据传输速率
> - 仔细设计PCB布线是高速稳定工作的**关键**

---


