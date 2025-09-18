# 第十六章 SDIO介绍及应用

## 1. SDIO简介

SD 卡 (Secure Digital Memory Card) 在我们生活中已经非常普遍了，控制器对 SD 卡进行读写通信操作一般有两种通信接口可选，一种是 SPI 接口，另外一种就是 SDIO 接口。 SDIO 全称是安全数字输入/输出接口，多媒体卡 (MMC)、 SD 卡、 SD I/O 卡都有 SDIO 接口。 STM32F10x 系列控制器有一个 SDIO 主机接口，它可以与 MMC 卡、 SD 卡、 SD I/O 卡以及 CE-ATA 设备进行数据传输。MMC 卡可以说是 SD 卡的前身，现阶段已经用得很少。 SD I/O 卡本身不是用于存储的卡，它是指利用 SDIO 传输协议的一种外设。比如 Wi-Fi Card，它主要是提供 Wi-Fi 功能，有些 Wi-Fi 模块是使用串口或者 SPI 接口进行通信的，但 Wi-Fi SDIO Card 是使用 SDIO 接口进行通信的。并且一般设计 SD I/O 卡是可以插入到 SD 的插槽。 CE-ATA 是专为轻薄笔记本硬盘设计的硬盘高速通讯接口。

![屏幕截图 2025-07-09 165950.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/07/09-16-59-52-屏幕截图%202025-07-09%20165950.png)

## 2. SDIO初始化结构体

标准库函数对 SDIO 外设建立了三个初始化结构体，分别为 SDIO 初始化结构体 SDIO_InitTypeDef、SDIO 命令初始化结构体 SDIO_CmdInitTypeDef 和 SDIO 数据初始化结构体 SDIO_DataInitTypeDef。这些结构体成员用于设置 SDIO 工作环境参数，并由 SDIO 相应初始化配置函数或功能函数调用，这些参数将会被写入到 SDIO 相应的寄存器，达到配置 SDIO 工作环境的目的。

SDIO 初始化结构体用于配置 SDIO 基本工作环境，比如时钟分频、时钟沿、数据宽度等等。它被SDIO_Init 函数使用。

```c
typedef struct {
    uint32_t SDIO_ClockEdge; // 时钟沿
    uint32_t SDIO_ClockBypass; // 旁路时钟
    uint32_t SDIO_ClockPowerSave; // 节能模式
    uint32_t SDIO_BusWide; // 数据宽度
    uint32_t SDIO_HardwareFlowControl; // 硬件流控制
    uint8_t SDIO_ClockDiv; // 时钟分频
}SDIO_InitTypeDef;
```

### 2.1 **SDIO_ClockEdge**

- **描述**: 控制SDIO时钟有效沿的选择，即SDIO时钟信号在CLK引脚上是通过上升沿还是下降沿有效。
- **可选值**:
  - `SDIO_ClockEdge_Rising`: 选择上升沿有效。这意味着时钟信号的上升沿用于同步数据采样。
  - `SDIO_ClockEdge_Falling`: 选择下降沿有效。这意味着时钟信号的下降沿用于同步数据采样。
- **作用**: 该参数配置时钟边沿（上升或下降沿），会影响到SDIO数据传输的同步方式，通常选择上升沿 (`SDIO_ClockEdge_Rising`) 作为默认配置。
- **寄存器影响**: 设定 SDIO 时钟控制寄存器 (SDIO_CLKCR) 中的 `NEGEDGE` 位。

### 2.2 **SDIO_ClockBypass**

- **描述**: 控制是否启用时钟旁路功能。
- **可选值**:
  - `SDIO_ClockBypass_Disable`: 禁用时钟旁路功能，时钟信号按常规的方式处理。
  - `SDIO_ClockBypass_Enable`: 启用时钟旁路功能，时钟信号可以被旁路以优化时钟传输。
- **作用**: 当启用旁路时，时钟信号会直接用于数据传输，不经过标准时钟路径，通常用于优化数据传输的延迟。

### 2.3 **SDIO_ClockPowerSave**

- **描述**: 控制SDIO接口是否进入节能模式。
- **可选值**:
  - `SDIO_ClockPowerSave_Disable`: 禁用节能模式，SDIO时钟一直处于激活状态。
  - `SDIO_ClockPowerSave_Enable`: 启用节能模式，在空闲时降低功耗。
- **作用**: 在SDIO不活跃时，启用节能模式可以减少功耗，通常用于低功耗设备。

### 2.4 **SDIO_BusWide**

- **描述**: 配置SDIO数据总线的宽度，即一次数据传输时并行传输的数据位数。
- **可选值**:
  - `SDIO_BusWide_1b`: 1位数据总线，标准模式下使用的总线宽度。
  - `SDIO_BusWide_4b`: 4位数据总线，较高的数据传输速率。
  - `SDIO_BusWide_8b`: 8位数据总线，最高的数据传输速率。
- **作用**: 选择数据总线宽度会直接影响SDIO接口的数据传输速率。更宽的总线（如8位）可以更快速地传输更多的数据。

### 2.5 **SDIO_HardwareFlowControl**

- **描述**: 控制是否启用硬件流控制。
- **可选值**:
  - `SDIO_HardwareFlowControl_Disable`: 禁用硬件流控制，数据传输不进行流控制。
  - `SDIO_HardwareFlowControl_Enable`: 启用硬件流控制，SDIO自动控制数据流量，以避免数据丢失。
- **作用**: 启用硬件流控制可以确保数据传输的平稳进行，避免因为数据速率过快导致数据丢失或溢出。

### 2.6 **SDIO_ClockDiv**

- **描述**: 设置SDIO时钟的分频系数，用于调整SDIO时钟的频率。
- **可选值**: `0` 到 `255` 之间的任何值。
  - 时钟分频值决定了SDIO时钟的频率。通常，较小的分频值会导致更高的时钟频率，适用于高数据速率的应用。
  - 较大的分频值可以降低时钟频率，适用于降低功耗或应对某些外部设备对时钟频率的要求。
- **作用**: 控制SDIO时钟的分频比，进而影响SDIO数据传输的速率。通过调整此值，可以根据需要选择合适的时钟频率。

## 3. SDIO命令初始化结构体

SDIO 命令初始化结构体用于设置命令相关内容，比如命令号、命令参数、响应类型等等。它被SDIO_SendCommand 函数使用。

```c
typedef struct {
    uint32_t SDIO_Argument; // 命令参数
    uint32_t SDIO_CmdIndex; // 命令号
    uint32_t SDIO_Response; // 响应类型
    uint32_t SDIO_Wait; // 等待使能
    uint32_t SDIO_CPSM; // 命令路径状态机
}SDIO_CmdInitTypeDef;
```

### 3.1 **SDIO_Argument**：

- **类型**: `uint32_t`
- **作用**: 该成员用于设置SDIO命令的参数。在发送命令时，某些命令需要额外的参数传递给SDIO，`SDIO_Argument` 用于传递这些参数。具体的参数内容会根据命令的类型而不同。

### 3.2 **SDIO_CmdIndex**：

- **类型**: `uint32_t`
- **作用**: 该成员用于设置SDIO命令的命令号。每个命令都由一个唯一的索引号标识，例如读取数据、写入数据、初始化等操作，每个操作会对应一个不同的命令号。

### 3.3 **SDIO_Response**：

- **类型**: `uint32_t`
- **作用**: 用于设置命令的响应类型。SDIO支持不同类型的响应：
  - **无响应**: 某些命令不需要响应。
  - **短响应**: 简单的响应（通常是一个32位的值）。
  - **长响应**: 较长的响应（例如，带有多个数据字的响应）。
- **可选值**:
  - `SDIO_Response_No`: 无响应。
  - `SDIO_Response_Short`: 短响应（32位）。
  - `SDIO_Response_Long`: 长响应（136位）。

### 3.4 **SDIO_Wait**：

- **类型**: `uint32_t`
- **作用**: 控制在执行命令时是否启用等待，使能延时等待响应。
- **可选值**:
  - `SDIO_Wait_No`: 不等待响应。
  - `SDIO_Wait_IT`: 等待响应并在响应到来时触发中断。
  - `SDIO_Wait_Pend`: 等待响应，但不会触发中断。

### 3.5 **SDIO_CPSM**：

- **类型**: `uint32_t`
- **作用**: 设置命令路径状态机的配置。它控制命令发送后，状态机的工作模式，包括命令的发送、等待响应和数据传输等过程。
- **可选值**:
  - `SDIO_CPSM_Enable`: 启用命令路径状态机。
  - `SDIO_CPSM_Disable`: 禁用命令路径状态机，通常用于直接手动控制命令和数据流。

## 4. SDIO数据初始化结构体

SDIO 数据初始化结构体用于配置数据发送和接收参数，比如传输超时、数据长度、传输模式等等。它被 SDIO_DataConfig 函数使用。

```c
typedef struct {
    uint32_t SDIO_DataTimeOut; // 数据传输超时
    uint32_t SDIO_DataLength; // 数据长度
    uint32_t SDIO_DataBlockSize; // 数据块大小
    uint32_t SDIO_TransferDir; // 数据传输方向
    uint32_t SDIO_TransferMode; // 数据传输模式
    uint32_t SDIO_DPSM; // 数据路径状态机
}SDIO_DataInitTypeDef;
```

### 4.1 **SDIO_DataTimeOut**：

- **类型**: `uint32_t`
- **作用**: 设置数据传输的超时值。若在指定的时间内没有完成数据传输，超时机制会触发错误或中断处理。

### 4.2 **SDIO_DataLength**：

- **类型**: `uint32_t`
- **作用**: 设置要传输的数据长度。此值表示一次数据传输的字节数。

### 4.3 **SDIO_DataBlockSize**：

- **类型**: `uint32_t`
- **作用**: 设置数据块的大小。它定义了每次数据传输时每块数据的字节数。数据可以按此块大小进行传输，通常是 512 字节、1024 字节等。

### 4.4 **SDIO_TransferDir**：

- **类型**: `uint32_t`
- **作用**: 设置数据传输的方向。SDIO支持两种数据传输方向：
  - **从设备到主机（读取）**。
  - **从主机到设备（写入）**。
- **可选值**:
  - `SDIO_TransferDir_ToSD`: 从主机到SD卡写入。
  - `SDIO_TransferDir_FromSD`: 从SD卡到主机读取。

### 4.5 **SDIO_TransferMode**：

- **类型**: `uint32_t`
- **作用**: 设置数据传输的模式。常见的模式有：
  - **单次传输模式**：每次传输一块数据。
  - **连续传输模式**：可以不断连续传输数据块，直到传输结束。
- **可选值**:
  - `SDIO_TransferMode_Block`: 使用块传输模式（一次传输一个块）。
  - `SDIO_TransferMode_Stream`: 使用流传输模式（持续不断地传输数据）。

### 4.6 **SDIO_DPSM**：

- **类型**: `uint32_t`
- **作用**: 设置数据路径状态机的配置。控制数据的传输路径和状态，包括如何启动、等待数据、传输过程等。数据路径状态机的启用和禁用决定了是否自动进行数据传输。
- **可选值**:
  - `SDIO_DPSM_Enable`: 启用数据路径状态机，自动完成数据传输。
  - `SDIO_DPSM_Disable`: 禁用数据路径状态机，通常用于手动控制数据传输。

## 5. SD卡读写测试

```c
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_sdio.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include <stdio.h>
#include <string.h>

/* 硬件配置 */
#define SD_DETECT_PIN     GPIO_Pin_8
#define SD_DETECT_PORT    GPIOC
#define SD_DETECT_CLK     RCC_APB2Periph_GPIOC

/* 调试串口配置 */
#define DEBUG_USART       USART1
#define DEBUG_USART_CLK   RCC_APB2Periph_USART1
#define DEBUG_USART_TX    GPIO_Pin_9
#define DEBUG_USART_RX    GPIO_Pin_10
#define DEBUG_USART_PORT  GPIOA

/* SD卡状态定义 */
typedef enum {
    SD_OK = 0,
    SD_ERROR,
    SD_TIMEOUT,
    SD_ERROR_CMD_CRC,
    SD_ERROR_DATA_CRC,
    SD_ERROR_RESP_TIMEOUT,
    SD_ERROR_VOLTAGE,
    SD_ERROR_CMD_RSP,
    SD_CARD_LOCKED,
    SD_INVALID_VOLTRANGE
} SDStatus;

/* SD卡信息结构 */
typedef struct {
    uint32_t CardType;     // 卡类型
    uint32_t RCA;          // 相对卡地址
    uint32_t Capacity;     // 容量(块数)
    uint32_t BlockSize;    // 块大小
} SD_CardInfo;

/* 全局变量 */
static SD_CardInfo SDCardInfo;
static volatile uint32_t SysTickCount = 0;

/* 函数声明 */
void SystemClock_Config(void);
void SysTick_Init(void);
void USART_Config(void);
void GPIO_Config(void);
void SDIO_GPIO_Init(void);
void SDIO_Config(void);
SDStatus SD_SendCommand(uint8_t cmd, uint32_t arg, uint8_t response);
SDStatus SD_GetResponse(uint32_t *response);
SDStatus SD_Init(void);
SDStatus SD_ReadBlock(uint8_t *buffer, uint32_t block, uint16_t count);
SDStatus SD_WriteBlock(uint8_t *buffer, uint32_t block, uint16_t count);
uint8_t SD_IsDetected(void);
void SD_DMA_Init(void);
void SDIO_Cmd(FunctionalState NewState);
void DMA2_Channel4_5_IRQHandler(void);
void Delay_ms(uint32_t ms);
void SD_Test(void);

/***************************** 串口重定向 *****************************/
// 确保printf正常工作
#ifdef __GNUC__
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE {
    USART_SendData(DEBUG_USART, (uint8_t)ch);
    while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);
    return ch;
}

/***************************** 系统初始化 *****************************/
void SystemClock_Config(void) {
    // 设置系统时钟为72MHz
    SystemInit();  // 使用标准库提供的初始化
    
    // 启用外部晶振
    RCC_HSEConfig(RCC_HSE_ON);
    while (!RCC_WaitForHSEStartUp());
    
    // 配置PLL
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    RCC_PLLCmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    
    // 切换系统时钟到PLL
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while (RCC_GetSYSCLKSource() != 0x08);
    
    // 设置AHB、APB1、APB2时钟
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_PCLK2Config(RCC_HCLK_Div1);
}

void SysTick_Init(void) {
    // 配置SysTick为1ms中断
    if (SysTick_Config(SystemCoreClock / 1000)) {
        while (1);
    }
    NVIC_SetPriority(SysTick_IRQn, 0);
}

void USART_Config(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    
    // 配置TX引脚
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEBUG_USART_PORT, &GPIO_InitStructure);
    
    // 配置RX引脚
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(DEBUG_USART_PORT, &GPIO_InitStructure);
    
    // 配置USART
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(DEBUG_USART, &USART_InitStructure);
    
    USART_Cmd(DEBUG_USART, ENABLE);
    
    // 确保串口已准备好
    USART_ClearFlag(DEBUG_USART, USART_FLAG_TC);
}

void GPIO_Config(void) {
    // 使能GPIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | 
                          RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
    
    // 配置SD卡检测引脚
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SD_DETECT_PORT, &GPIO_InitStructure);
    
    // 关闭JTAG以释放引脚（如果需要）
    // GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}

/***************************** SDIO驱动 *****************************/
void SDIO_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 配置SDIO CMD引脚(PD2)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    // 配置SDIO CLK引脚(PC12)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // 配置SDIO数据引脚(PC8-11)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // 可选：配置上拉电阻（如果硬件上没有）
    GPIO_WriteBit(GPIOC, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11, Bit_SET);
    GPIO_WriteBit(GPIOD, GPIO_Pin_2, Bit_SET);
}

// 实现SDIO_Cmd函数
void SDIO_Cmd(FunctionalState NewState) {
    if (NewState != DISABLE) {
        SDIO->CLKCR |= SDIO_CLKCR_CLKEN;  // 使能SDIO时钟
    } else {
        SDIO->CLKCR &= ~SDIO_CLKCR_CLKEN; // 禁用SDIO时钟
    }
}

void SDIO_Config(void) {
    SDIO_InitTypeDef SDIO_InitStructure;
    
    // 使能SDIO时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SDIO, ENABLE);
    
    // 初始配置为低速(400kHz)
    SDIO_InitStructure.SDIO_ClockDiv = 0xB4;   // 72MHz/(180+2) ≈ 400kHz
    SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
    SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
    SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
    SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
    SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
    SDIO_Init(&SDIO_InitStructure);
    
    // 使能SDIO
    SDIO_Cmd(ENABLE);
}

SDStatus SD_SendCommand(uint8_t cmd, uint32_t arg, uint8_t response) {
    SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
    uint32_t timeout = 0xFFFF;
    
    // 清除所有标志
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT | SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND);
    
    // 配置命令
    SDIO_CmdInitStructure.SDIO_Argument = arg;
    SDIO_CmdInitStructure.SDIO_CmdIndex = cmd;
    SDIO_CmdInitStructure.SDIO_Response = response;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);
    
    // 等待响应或超时
    while (!(SDIO->STA & (SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT | SDIO_FLAG_CCRCFAIL))) {
        if (timeout-- == 0) return SD_TIMEOUT;
    }
    
    // 检查错误标志
    if (SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT)) {
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return SD_TIMEOUT;
    }
    if (SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL)) {
        SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
        return SD_ERROR_CMD_CRC;
    }
    
    return SD_OK;
}

SDStatus SD_GetResponse(uint32_t *response) {
    if (response == NULL) return SD_ERROR;
    *response = SDIO_GetResponse(SDIO_RESP1);
    return SD_OK;
}

SDStatus SD_Init(void) {
    SDStatus status;
    uint32_t response = 0;
    uint8_t validvoltage = 0;
    uint32_t startTime = SysTickCount;
    
    // 初始化GPIO
    SDIO_GPIO_Init();
    
    // 配置SDIO外设
    SDIO_Config();
    
    // 1. 发送CMD0使SD卡进入空闲状态
    status = SD_SendCommand(0, 0, SDIO_Response_No); // CMD0
    if (status != SD_OK) {
        printf("CMD0 failed: %d\r\n", status);
        return status;
    }
    
    // 2. 发送CMD8检查电压范围
    status = SD_SendCommand(8, 0x1AA, SDIO_Response_Short); // CMD8
    if (status == SD_OK) {
        SD_GetResponse(&response);
        if ((response & 0xFFF) != 0x1AA) {
            printf("Voltage mismatch: 0x%lX\r\n", response);
            return SD_ERROR_VOLTAGE;
        }
        validvoltage = 1;
        printf("CMD8 response: 0x%lX\r\n", response);
    } else {
        printf("CMD8 failed: %d\r\n", status);
    }
    
    // 3. 初始化SD卡
    do {
        if (SysTickCount - startTime > 1000) {
            printf("ACMD41 timeout\r\n");
            return SD_TIMEOUT; // 1秒超时
        }
        
        // 发送CMD55
        status = SD_SendCommand(55, 0, SDIO_Response_Short); // CMD55
        if (status != SD_OK) {
            printf("CMD55 failed: %d\r\n", status);
            continue;
        }
        
        // 发送ACMD41
        status = SD_SendCommand(41, validvoltage ? 0x40300000 : 0x00, 
                               SDIO_Response_Short); // ACMD41
        if (status != SD_OK) {
            printf("ACMD41 failed: %d\r\n", status);
            continue;
        }
        
        SD_GetResponse(&response);
        printf("ACMD41 response: 0x%lX\r\n", response);
        
    } while ((response & 0x80000000) == 0); // 等待初始化完成
    
    // 检查卡类型
    SDCardInfo.CardType = (response & 0x40000000) ? 2 : 1; // 2: SDHC, 1: SDSC
    printf("Card type: %s\r\n", SDCardInfo.CardType == 2 ? "SDHC/SDXC" : "SDSC");
    
    // 4. 获取CID (CMD2)
    status = SD_SendCommand(2, 0, SDIO_Response_Long); // CMD2
    if (status != SD_OK) {
        printf("CMD2 failed: %d\r\n", status);
        return status;
    }
    
    // 5. 获取相对地址 (CMD3)
    status = SD_SendCommand(3, 0, SDIO_Response_Short); // CMD3
    if (status != SD_OK) {
        printf("CMD3 failed: %d\r\n", status);
        return status;
    }
    
    SD_GetResponse(&response);
    SDCardInfo.RCA = response >> 16;
    printf("RCA: 0x%lX\r\n", SDCardInfo.RCA);
    
    // 6. 选择卡 (CMD7)
    status = SD_SendCommand(7, SDCardInfo.RCA << 16, SDIO_Response_Short); // CMD7
    if (status != SD_OK) {
        printf("CMD7 failed: %d\r\n", status);
        return status;
    }
    
    // 7. 设置总线宽度为4位
    // 发送CMD55
    status = SD_SendCommand(55, SDCardInfo.RCA << 16, SDIO_Response_Short); // CMD55
    if (status != SD_OK) {
        printf("CMD55 (bus width) failed: %d\r\n", status);
        return status;
    }
    
    // 发送ACMD6
    status = SD_SendCommand(6, 0x2, SDIO_Response_Short); // ACMD6
    if (status != SD_OK) {
        printf("ACMD6 failed: %d\r\n", status);
        return status;
    }
    
    // 设置高速模式(24MHz)
    SDIO_InitTypeDef SDIO_InitStruct;
    SDIO_StructInit(&SDIO_InitStruct);
    SDIO_InitStruct.SDIO_ClockDiv = 2; // 72MHz / (2+2) = 18MHz
    SDIO_InitStruct.SDIO_BusWide = SDIO_BusWide_4b;
    SDIO_Init(&SDIO_InitStruct);
    
    // 初始化DMA
    SD_DMA_Init();
    
    // 设置块大小为512字节
    status = SD_SendCommand(16, 512, SDIO_Response_Short); // CMD16
    if (status != SD_OK) {
        printf("CMD16 failed: %d\r\n", status);
        return status;
    }
    
    printf("SD Card initialized successfully!\r\n");
    return SD_OK;
}

/***************************** DMA驱动 *****************************/
static volatile uint8_t dmaTransferComplete = 0;

void SD_DMA_Init(void) {
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 使能DMA2时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
    
    // 配置DMA通道4 (SDIO RX)
    DMA_DeInit(DMA2_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SDIO->FIFO;
    DMA_InitStructure.DMA_MemoryBaseAddr = 0;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel4, &DMA_InitStructure);
    
    // 配置DMA中断
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel4_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    DMA_ITConfig(DMA2_Channel4, DMA_IT_TC, ENABLE);
    
    // 配置DMA通道5 (SDIO TX)
    DMA_DeInit(DMA2_Channel5);
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_Init(DMA2_Channel5, &DMA_InitStructure);
    DMA_ITConfig(DMA2_Channel5, DMA_IT_TC, ENABLE);
}

SDStatus SD_ReadBlock(uint8_t *buffer, uint32_t block, uint16_t count) {
    SDIO_DataInitTypeDef SDIO_DataInitStructure;
    uint32_t timeout;
    
    // 对于标准容量卡，块地址需要乘以块大小
    if (SDCardInfo.CardType == 1) { // SDSC
        block *= 512;
    }
    
    // 发送读命令
    SDStatus status = SD_SendCommand(count > 1 ? 18 : 17, block, SDIO_Response_Short);
    if (status != SD_OK) {
        printf("Read cmd failed: %d\r\n", status);
        return status;
    }
    
    // 配置数据路径
    SDIO_DataInitStructure.SDIO_DataTimeOut = 0xFFFFFF;
    SDIO_DataInitStructure.SDIO_DataLength = count * 512;
    SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_512b;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataConfig(&SDIO_DataInitStructure);
    
    // 配置DMA
    DMA_Cmd(DMA2_Channel4, DISABLE);
    DMA2_Channel4->CMAR = (uint32_t)buffer;
    DMA2_Channel4->CNDTR = (count * 512) / 4;
    DMA_Cmd(DMA2_Channel4, ENABLE);
    
    // 使能DMA传输
    SDIO_DMACmd(ENABLE);
    
    // 等待传输完成
    dmaTransferComplete = 0;
    timeout = 0xFFFFF;
    while (!dmaTransferComplete && timeout--) {
        if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_RXOVERR)) {
            SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_RXOVERR);
            return SD_ERROR_DATA_CRC;
        }
    }
    
    if (!timeout) {
        printf("Read timeout\r\n");
        return SD_TIMEOUT;
    }
    
    return SD_OK;
}

SDStatus SD_WriteBlock(uint8_t *buffer, uint32_t block, uint16_t count) {
    SDIO_DataInitTypeDef SDIO_DataInitStructure;
    uint32_t timeout;
    
    // 对于标准容量卡，块地址需要乘以块大小
    if (SDCardInfo.CardType == 1) { // SDSC
        block *= 512;
    }
    
    // 发送写命令
    SDStatus status = SD_SendCommand(count > 1 ? 25 : 24, block, SDIO_Response_Short);
    if (status != SD_OK) {
        printf("Write cmd failed: %d\r\n", status);
        return status;
    }
    
    // 配置数据路径
    SDIO_DataInitStructure.SDIO_DataTimeOut = 0xFFFFFF;
    SDIO_DataInitStructure.SDIO_DataLength = count * 512;
    SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_512b;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataConfig(&SDIO_DataInitStructure);
    
    // 配置DMA
    DMA_Cmd(DMA2_Channel5, DISABLE);
    DMA2_Channel5->CMAR = (uint32_t)buffer;
    DMA2_Channel5->CNDTR = (count * 512) / 4;
    DMA_Cmd(DMA2_Channel5, ENABLE);
    
    // 使能DMA传输
    SDIO_DMACmd(ENABLE);
    
    // 等待传输完成
    dmaTransferComplete = 0;
    timeout = 0xFFFFF;
    while (!dmaTransferComplete && timeout--) {
        if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_TXUNDERR)) {
            SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_TXUNDERR);
            return SD_ERROR_DATA_CRC;
        }
    }
    
    if (!timeout) {
        printf("Write timeout\r\n");
        return SD_TIMEOUT;
    }
    
    // 等待写入完成
    timeout = 0xFFFF;
    while (SDIO_GetFlagStatus(SDIO_FLAG_TXACT) && timeout--);
    
    if (!timeout) {
        printf("Write complete timeout\r\n");
        return SD_TIMEOUT;
    }
    
    return SD_OK;
}

/***************************** 中断处理 *****************************/
void SysTick_Handler(void) {
    SysTickCount++;
}

void DMA2_Channel4_5_IRQHandler(void) {
    // 通道4传输完成 (读)
    if (DMA_GetITStatus(DMA2_IT_TC4)) {
        DMA_ClearITPendingBit(DMA2_IT_TC4);
        DMA_Cmd(DMA2_Channel4, DISABLE);
        dmaTransferComplete = 1;
    }
    
    // 通道5传输完成 (写)
    if (DMA_GetITStatus(DMA2_IT_TC5)) {
        DMA_ClearITPendingBit(DMA2_IT_TC5);
        DMA_Cmd(DMA2_Channel5, DISABLE);
        dmaTransferComplete = 1;
    }
}

/***************************** 辅助函数 *****************************/
void Delay_ms(uint32_t ms) {
    uint32_t start = SysTickCount;
    while ((SysTickCount - start) < ms);
}

uint8_t SD_IsDetected(void) {
    return (GPIO_ReadInputDataBit(SD_DETECT_PORT, SD_DETECT_PIN) == Bit_RESET);
}

/***************************** SD卡测试 *****************************/
void SD_Test(void) {
    SDStatus status;
    uint8_t writeBuffer[512];
    uint8_t readBuffer[512];
    uint32_t i;
    
    printf("\r\nStarting SD Card Test...\r\n");
    
    // 1. 检测SD卡是否存在
    if (!SD_IsDetected()) {
        printf("Error: No SD card detected!\r\n");
        return;
    }
    printf("SD card detected.\r\n");
    
    // 2. 初始化SD卡
    printf("Initializing SD card...\r\n");
    status = SD_Init();
    
    if (status != SD_OK) {
        printf("Initialization failed: %d\r\n", status);
        return;
    }
    
    // 3. 准备测试数据
    for (i = 0; i < 512; i++) {
        writeBuffer[i] = i % 256;
    }
    
    // 4. 写入测试数据 (块地址0x1000)
    printf("Writing test data to block 0x1000...\r\n");
    status = SD_WriteBlock(writeBuffer, 0x1000, 1);
    if (status != SD_OK) {
        printf("Write failed: %d\r\n", status);
        return;
    }
    printf("Write completed.\r\n");
    
    // 5. 读取数据
    printf("Reading data from block 0x1000...\r\n");
    status = SD_ReadBlock(readBuffer, 0x1000, 1);
    if (status != SD_OK) {
        printf("Read failed: %d\r\n", status);
        return;
    }
    printf("Read completed.\r\n");
    
    // 6. 验证数据
    printf("Verifying data...\r\n");
    for (i = 0; i < 512; i++) {
        if (writeBuffer[i] != readBuffer[i]) {
            printf("Data mismatch at byte %lu: Wrote 0x%02X, Read 0x%02X\r\n", 
                   i, writeBuffer[i], readBuffer[i]);
            printf("Verification failed!\r\n");
            return;
        }
    }
    printf("Verification successful! All data matches.\r\n");
    printf("SD card test completed successfully!\r\n");
}

/***************************** 主函数 *****************************/
int main(void) {
    // 系统初始化
    SystemClock_Config();
    SysTick_Init();
    GPIO_Config();
    USART_Config();
    
    // 确保串口正常工作
    printf("\r\nSystem Initialized\r\n");
    printf("Clock Frequency: %lu Hz\r\n", SystemCoreClock);
    printf("This message confirms UART is working!\r\n");
    
    // 运行SD卡测试
    SD_Test();
    
    while (1) {
        // 主循环
        Delay_ms(1000);
        printf("System running...\r\n");
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
    printf("Assert failed in %s, line %lu\r\n", file, line);
    while(1);
}
#endif

```

## 6. SDIO常见函数（STD库）

### 6.1 SDIO 初始化与配置

#### 6.1.1 **时钟与GPIO初始化**

```c
RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);

// 配置SDIO引脚（4位模式）
GPIO_InitTypeDef GPIO_InitStruct;
// CLK(PC12), CMD(PD2)
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;  // CLK
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOC, &GPIO_InitStruct);

// D0-D3(PC8-PC11)
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
GPIO_Init(GPIOC, &GPIO_InitStruct);
```

#### 6.1.2 **SDIO 参数配置**

```c
SDIO_InitTypeDef SDIO_InitStruct;
SDIO_InitStruct.SDIO_ClockDiv = SDIO_TRANSFER_CLK_DIV;  // 分频系数（例：0x02为18MHz）
SDIO_InitStruct.SDIO_ClockEdge = SDIO_ClockEdge_Rising; // 上升沿采样
SDIO_InitStruct.SDIO_ClockBypass = SDIO_ClockBypass_Disable; // 禁用旁路
SDIO_InitStruct.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable; // 关闭省电模式
SDIO_InitStruct.SDIO_BusWide = SDIO_BusWide_4b;         // 4位数据线（或SDIO_BusWide_8b）
SDIO_InitStruct.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable; // 禁用硬件流控:cite[3]:cite[5]

SDIO_Init(&SDIO_InitStruct);
SDIO_Cmd(ENABLE); // 使能SDIO
```

### 6.2 **数据传输函数**

#### 6.2.1 **单块读写**

```c
// 读取单个块（512字节）
SD_Error SD_ReadBlock(uint8_t *readbuff, uint32_t ReadAddr, uint16_t BlockSize) {
    // 必须发送CMD16设置块大小
    SDIO_CmdInitStructure.SDIO_Argument = BlockSize;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
    SDIO_SendCommand(&SDIO_CmdInitStructure);
    if (CmdResp1Error(SD_CMD_SET_BLOCKLEN) != SD_OK) return ERROR;

    // 发送CMD17读取数据
    SDIO_CmdInitStructure.SDIO_Argument = ReadAddr;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_READ_SINGLE_BLOCK;
    SDIO_SendCommand(&SDIO_CmdInitStructure);
    // ... 数据接收逻辑:cite[8]
}
```

#### 6.2.2 **多块读写**

```c
// 写入多块数据（需DMA支持）
SD_Error SD_WriteMultiBlocks(uint8_t *writebuff, uint32_t WriteAddr, uint16_t BlockSize, uint32_t NumberOfBlocks) {
    // 发送CMD25启动多块写入
    SDIO_CmdInitStructure.SDIO_Argument = WriteAddr;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_WRITE_MULT_BLOCK;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    // 启用DMA传输
    SDIO_DMACmd(ENABLE);
    DMA_Config(); // 配置DMA通道（DMA2_Channel4/5）

    // 传输完成后发送CMD12停止命令
    SDIO_CmdInitStructure.SDIO_Argument = 0;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    // 必须等待SD卡退出忙状态（检测D0线电平）:cite[4]:cite[7]
    while(SDIO_GetFlagStatus(SDIO_FLAG_RXACT) != RESET);
}
```

### 6.3 **DMA 配置**

#### 6.3.1 **使能SDIO DMA请求**

```c
SDIO_DMACmd(ENABLE);
```

#### 6.3.2 **初始化DMA通道**

```c
DMA_InitTypeDef DMA_InitStruct;
DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&SDIO->FIFO; // SDIO FIFO地址
DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)buffer;         // 内存缓冲区
DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;               // 读方向（外设→内存）
DMA_InitStruct.DMA_BufferSize = blockSize * numBlocks / 4;    // 传输字数（32位）
DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word; // 32位
DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;                    // 非循环模式
DMA_InitStruct.DMA_Priority = DMA_Priority_High;
DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;

DMA_Init(DMA2_Channel4, &DMA_InitStruct);  // SDIO TX用Channel4
DMA_Cmd(DMA2_Channel4, ENABLE);
```

### 6.4 **中断处理**

#### 6.4.1 **使能中断**

```c
// 使能SDIO事件中断（传输完成、错误等）
SDIO_ITConfig(SDIO_IT_DATAEND | SDIO_IT_RXOVERR | SDIO_IT_DCRCFAIL, ENABLE);

// 配置NVIC
NVIC_InitTypeDef NVIC_InitStruct;
NVIC_InitStruct.NVIC_IRQChannel = SDIO_IRQn;
NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0; // 高优先级
NVIC_Init(&NVIC_InitStruct);
```

#### 6.4.2 **中断服务函数**

```c
void SDIO_IRQHandler(void) {
    if (SDIO_GetITStatus(SDIO_IT_DATAEND) != RESET) {
        // 数据传输完成处理
        SDIO_ClearITPendingBit(SDIO_IT_DATAEND);
    }
    if (SDIO_GetITStatus(SDIO_IT_DCRCFAIL) != RESET) {
        // CRC错误处理
        SDIO_ClearITPendingBit(SDIO_IT_DCRCFAIL);
    }
}
```

### 6.5 **错误处理与状态检测**

#### 6.5.1 **错误标志检查**

```c
SD_Error status = SD_OK;
if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET) {
    status = SD_CRC_ERROR;
    SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
}
if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET) {
    status = SD_DATA_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
}
```

#### 6.5.2 **卡状态查询**

```c
// 发送CMD13查询卡状态
SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)RCA << 16; // RCA地址
SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
SDIO_SendCommand(&SDIO_CmdInitStructure);
if (CmdResp1Error(SD_CMD_SEND_STATUS) == SD_OK) {
    uint32_t card_status = SDIO_GetResponse(SDIO_RESP1);
    if (card_status & 0x02000000) { // 检查忙状态位
        // 卡忙，需等待
    }
}
```

错误代码释义

| **错误标志**             | **含义**    | **处理方式**      |
| -------------------- | --------- | ------------- |
| `SDIO_FLAG_DCRCFAIL` | 数据CRC校验失败 | 重传数据或降低时钟频率   |
| `SDIO_FLAG_DTIMEOUT` | 数据传输超时    | 检查物理连接或增加超时值  |
| `SDIO_FLAG_RXOVERR`  | 接收溢出      | 优化DMA速率或减小块大小 |
| `SDIO_FLAG_STBITERR` | 起始位错误     | 重新初始化SD卡      |
