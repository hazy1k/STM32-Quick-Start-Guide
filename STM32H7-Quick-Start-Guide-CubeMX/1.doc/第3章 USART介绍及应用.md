# 第三章 USART介绍及应用

## 1. USART简介

USART（Universal Synchronous Asynchronous Receiver Transmitter）是 STM32H750VBT6 的核心串行通信外设，**支持同步（Synchronous）和异步（Asynchronous）两种通信模式**，广泛应用于调试输出、传感器数据采集、设备间通信（如 GPS、蓝牙模块）等场景。STM32H750VBT6 集成了 **4 个全功能 USART**（USART1-3, USART6）和 **4 个 UART**（UART4-5, UART7-8），其中 USART 支持同步时钟信号，功能更强大。

> 🔍 **关键区别**：
> 
> - **USART** = 通用**同步**异步收发器（含同步时钟引脚 `SCLK`）
> - **UART** = 通用**异步**收发器（仅支持异步通信）  
>   *STM32H750VBT6 的 USART1/2/3/6 支持同步模式，其余为纯 UART*

---

### 1.1 USART 核心特性（基于 STM32H750VBT6）

| **特性**     | **参数**                      | **说明**                              | **应用场景**                      |
| ---------- | --------------------------- | ----------------------------------- | ----------------------------- |
| **通信模式**   | 异步/同步                       | 异步：无时钟线（TX/RX）；同步：需 `SCLK` 时钟线      | 异步：调试串口；同步：高速数据传输（如与 FPGA 通信） |
| **波特率范围**  | 1200 bps – **10.5 Mbps**    | 依赖时钟源（最高 10.5Mbps @ 200MHz APB1 时钟） | 高速传感器（如 IMU 数据流）              |
| **数据格式**   | 7/8/9 位数据 + 0.5/1/1.5/2 停止位 | 可配置校验位（偶/奇校验）                       | 兼容老式设备（如 9 位协议）               |
| **多处理器通信** | IDLE 检测 + 地址匹配              | 支持 4 位/7 位/10 位地址过滤                 | 工业总线（如 Modbus RTU）            |
| **高级功能**   | LIN 主/从模式、智能卡模式、IrDA 红外     | 内置 16x 过采样 + 噪声抑制滤波器                | 汽车电子（LIN 总线）、医疗设备             |
| **中断/DMA** | 10+ 种中断源 + 双缓冲 DMA          | 可触发发送/接收完成、帧错误、空闲中断                 | 高效数据传输（避免 CPU 轮询）             |

📌 **STM32H750VBT6 专属优势**：

- **双时钟域支持**：USART1/6 位于 APB2（最高 200MHz），USART2/3 位于 APB1（最高 100MHz）
- **低功耗模式**：支持 Stop 0/1 模式下的唤醒（通过空闲中断或地址匹配）
- **硬件流控制**：`CTS`（清发送）/`RTS`（请求发送）引脚自动控制数据流（防溢出）

---

### 1.2 工作原理详解

#### 1.2.1 异步通信模式（最常用）

🔹 **帧结构**：

```c
[起始位] | [数据位 8] | [校验位] | [停止位 1]  
   0     |  LSB→MSB  |   可选    |     1/0.5/1.5/2
```

- **起始位**：固定低电平（1 bit）
- **数据位**：7/8/9 bit（可配置 LSB/MSB 优先）
- **校验位**：偶校验/奇校验（提高可靠性）
- **停止位**：高电平（1/0.5/1.5/2 bit）

🔹 **波特率生成**：

- 公式：`Baud = f_clk / (8 * (2 - OVER8) * USARTDIV)`
  
  - `OVER8=0`：16 倍过采样（精度高）
  - `OVER8=1`：8 倍过采样（速度更快）

- **关键寄存器**：`USART_BRR`（波特率寄存器）
  
  ```c
  // 例如：115200bps @ 100MHz APB1 时钟 (OVER8=0)
  USARTDIV = 100000000 / (16 * 115200) = 54.253 → 整数部分=54, 小数部分=0.253*16≈4
  BRR = (54 << 4) | 4 = 0x364; // 写入寄存器
  ```

⚠️ **波特率误差**：

- STM32H7 要求误差 < **±2%**（否则通信失败）
- **解决方案**：
  1. 选择合适时钟源（如 HSE 25MHz 比 HSI 64MHz 更精确）
  2. 启用 `OVER8=1`（8 倍过采样）减小误差

#### 1.2.2 同步通信模式

🔹 **时钟信号**：

- 通过 `SCLK` 引脚输出同步时钟（上升沿采样）
- 时钟极性/相位可配置（类似 SPI）：
  - `CPOL=0`：空闲时钟低电平
  - `CPHA=0`：第一个时钟边沿捕获数据

🔹 **优势**：

- 无波特率误差问题（时钟同步）
- 最高支持 **10.5 Mbps**（异步模式仅 5.25 Mbps）
- 适合长距离/噪声环境通信

---

### 1.3 关键寄存器操作

#### 1.3.1 核心配置寄存器

| **寄存器** | **关键位域**                   | **功能**               | **配置示例**                                                      |
| ------- | -------------------------- | -------------------- | ------------------------------------------------------------- |
| **CR1** | UE, M, PCE, PS, TE, RE     | 使能外设、数据长度、校验、发送/接收   | `USART1->CR1 = (1<<3) \| (1<<2) \| (1<<13); // 使能 TX/RX + 外设` |
| **CR2** | STOP, CLKEN, LBDL          | 停止位、同步时钟使能、LIN 检测    | `USART1->CR2 = (0b11<<12); // 2 停止位`                          |
| **CR3** | CTSE, RTSE, DMAT, DMAR     | 硬件流控制、DMA 使能         | `USART1->CR3 = (1<<9) \| (1<<8); // 使能 RTS/CTS`               |
| **BRR** | DIV_Mantissa, DIV_Fraction | 波特率分频值               | `USART1->BRR = 0x364; // 115200bps`                           |
| **SR**  | TXE, RXNE, ORE, IDLE       | 状态标志（发送空、接收非空、溢出、空闲） | `while (!(USART1->SR & (1<<6))); // 等待发送完成`                   |
| **DR**  | DR[8:0]                    | 数据寄存器（读写）            | `USART1->DR = 'A'; // 发送字符`                                   |

#### 1.3.2 中断/DMA 关键配置

| **功能**     | **寄存器位**                     | **说明**      | **实战要点**                              |
| ---------- | ---------------------------- | ----------- | ------------------------------------- |
| **接收中断**   | `CR1.RXNEIE=1`               | 数据寄存器非空时触发  | **必须立即读 DR**（否则标志不自动清零）               |
| **发送中断**   | `CR1.TXEIE=1`                | 发送寄存器为空时触发  | 适合发送不定长数据（如字符串）                       |
| **空闲中断**   | `CR1.IDLEIE=1`               | 线路空闲时触发     | **DMA 接收不定长数据的黄金方案**（结合 `SR.IDLE`）    |
| **DMA 请求** | `CR3.DMAT=1`<br>`CR3.DMAR=1` | 使能发送/接收 DMA | 配置 DMA 通道（USART1_RX 通常在 DMA1 Stream5） |

✅ **空闲中断 + DMA 接收流程**：

1. 使能 `CR1.IDLEIE` 和 `CR3.DMAR`
2. DMA 接收数据到缓冲区
3. 空闲线检测触发中断 → 读取 `SR.IDLE`
4. 通过 `DMA_SxNDTR` 计算已接收数据长度
5. **关键操作**：先写 `SR.IDLE` 再读 `DR` 清除标志

---

### 1.4 USART vs UART vs SPI vs I2C 对比表

| **特性**    | **USART**              | **UART**  | **SPI**  | **I2C**  |
| --------- | ---------------------- | --------- | -------- | -------- |
| **时钟线**   | 有 (同步模式)               | 无         | 有 (SCLK) | 有 (SCL)  |
| **最大速率**  | 10.5 Mbps              | 5.25 Mbps | 50+ Mbps | 1-5 Mbps |
| **引脚数**   | 2-4 (TX/RX/CK/CTS/RTS) | 2 (TX/RX) | 2-4      | 2        |
| **多设备支持** | 地址匹配                   | 无         | 片选线      | 7/10 位地址 |
| **典型用途**  | 调试、传感器                 | 低成本通信     | 高速外设     | 低速外设     |

> 💡 **选择建议**：
> 
> - **调试输出** → USART（异步模式）
> - **连接 GPS/蓝牙** → UART（无同步需求）
> - **高速 ADC 传输** → USART（同步模式）
> - **避免 I2C 总线拥塞** → USART + 地址匹配（多设备通信）

## 2. USART使用示例-STM32IDE

### 2.1 STM32Cube配置

#### 2.1.1 RCC配置

只在第一章中展示，因为后续内容一样

#### 2.1.2 USART GPIO配置

![屏幕截图 2025-08-24 160053.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/24-16-01-07-屏幕截图%202025-08-24%20160053.png)

#### 2.1.3 USART工作模式配置

![屏幕截图 2025-08-24 160140.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/24-16-01-51-屏幕截图%202025-08-24%20160140.png)

#### 2.1.4 NVIC配置

![屏幕截图 2025-08-24 160207.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/24-16-02-21-屏幕截图%202025-08-24%20160207.png)

### 2.2 用户代码

#### 2.2.1 相关宏定义

```c
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */
// 环形缓冲区配置
#define BUFFER_SIZE 64
// 声明环形缓冲区结构
typedef struct {
    uint8_t data[BUFFER_SIZE];
    volatile uint16_t head;  // 写指针
    volatile uint16_t tail;  // 读指针
} RingBuffer;
// 声明全局变量
extern RingBuffer rx_buffer;
extern uint8_t received_byte;

// 是否启用接收中断
#define USE_UART_RX_IT 0
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */


```

#### 2.2.2 USART初始化配置

```c
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "stdio.h"
/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}
```

#### 2.2.3 printf重定向

```c
/* USER CODE BEGIN 1 */
/******************************************************************************************/
/* 加入以下代码, 支持printf函数, 而不需要选择use MicroLIB */
#if 1

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

/* printf重定向 */
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
#endif
/* USER CODE END 1 */
```

#### 2.2.4 接收中断函数

```c
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32h7xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32h7xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "exti.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */
// 定义全局变量
RingBuffer rx_buffer = { .head = 0, .tail = 0 };
uint8_t received_byte;
/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart1;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32H7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h7xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line1 interrupt.
  */
void EXTI1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI1_IRQn 0 */

  /* USER CODE END EXTI1_IRQn 0 */
  //HAL_GPIO_EXTI_IRQHandler(EXTI_KEY1_Pin);
  /* USER CODE BEGIN EXTI1_IRQn 1 */
  //if(HAL_GPIO_ReadPin(EXTI_KEY1_GPIO_Port, EXTI_KEY1_Pin) == GPIO_PIN_RESET)
  //{
	//	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
  //}
  /* USER CODE END EXTI1_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/* USER CODE BEGIN 1 */
#if USE_UART_RX_IT
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    // 计算下一个写入位置
    uint16_t next_head = (rx_buffer.head + 1) % BUFFER_SIZE;

    // 仅当缓冲区未满时存储数据
    if (next_head != rx_buffer.tail)
    {
      rx_buffer.data[rx_buffer.head] = received_byte;
      rx_buffer.head = next_head;
    }

    // 重新启用接收中断
    HAL_UART_Receive_IT(&huart1, &received_byte, 1);
  }
}
#endif
/* USER CODE END 1 */

```

#### 2.2.5 主函数测试

```c
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp_init.h"
#include "stdio.h" // For printf function
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern RingBuffer rx_buffer;
extern uint8_t received_byte;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  //MX_GPIO_Init();
  //MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  bsp_init();
  // 初始化缓冲区指针
  rx_buffer.head = 0;
  rx_buffer.tail = 0;
  // 启动接收中断
  HAL_UART_Receive_IT(&huart1, &received_byte, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(rx_buffer.head != rx_buffer.tail) // 检查缓冲区是否有数据
	  {
	      // 从缓冲区读取数据
	      uint8_t tx_byte = rx_buffer.data[rx_buffer.tail];
	      rx_buffer.tail = (rx_buffer.tail + 1) % BUFFER_SIZE;
	      // 发送数据
	      HAL_UART_Transmit(&huart1, &tx_byte, 1, HAL_MAX_DELAY);
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
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

## 3. USART/UART相关函数（HAL库）

### 3.1 初始化与配置

- **核心配置流程**（四步关键操作）：
  
  1. **使能时钟**（区分APB1/APB2总线）
  2. **配置GPIO复用功能**
  3. **初始化USART参数**
  4. **配置NVIC中断**

- `HAL_UART_Init(UART_HandleTypeDef *huart)`  
  **基础配置示例**：
  
  ```c
  // 1. 使能时钟 (USART1/6在APB2, 其他在APB1)
  __HAL_RCC_USART1_CLK_ENABLE();       // APB2最高160MHz
  __HAL_RCC_GPIOB_CLK_ENABLE();        // TX=PB6, RX=PB7
  
  // 2. 配置GPIO (复用推挽输出 + 浮空输入)
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;      // TX必须推挽
  GPIO_InitStruct.Pull = GPIO_NOPULL;          // RX可配置上拉
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART1; // 查手册确认AF编号
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  // 3. 初始化USART
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  HAL_UART_Init(&huart1);
  ```

- **`UART_InitTypeDef` 关键成员**：
  
  | **成员**       | **说明** | **典型值**               | **H750特殊说明**             |
  | ------------ | ------ | --------------------- | ------------------------ |
  | `BaudRate`   | 通信波特率  | 9600/115200/921600    | 最高支持10.5Mbps             |
  | `WordLength` | 数据位长度  | `UART_WORDLENGTH_8B`  | 支持7/8/9位                 |
  | `StopBits`   | 停止位数量  | `UART_STOPBITS_1`     | `UART_STOPBITS_0_5`需校验   |
  | `Parity`     | 校验方式   | `UART_PARITY_NONE`    | `UART_PARITY_EVEN/ODD`   |
  | `Mode`       | 工作模式   | `UART_MODE_TX_RX`     | `UART_MODE_RX_ONLY`      |
  | `HwFlowCtl`  | 硬件流控   | `UART_HWCONTROL_NONE` | `UART_HWCONTROL_RTS_CTS` |

- **波特率精度保障**（H750关键）：
  
  ```c
  // 计算实际波特率误差（CubeMX自动验证）
  float error = fabs(115200 - (float)APB_Clock / USARTDIV) / 115200;
  if(error > 0.02) { /* 误差>2%需调整时钟配置 */ }
  ```
  
  > ✅ **H750优化方案**：
  > 
  > - 使用16倍过采样（`OVER8=0`）提高精度
  > - 高波特率时优先选择APB2总线（160MHz）

### 3.2 数据收发操作

- **基础收发模式**：
  
  | **函数**                           | **原型**                           | **特点**   | **适用场景** |
  | -------------------------------- | -------------------------------- | -------- | -------- |
  | `HAL_UART_Transmit()`            | `(huart, *pData, Size, Timeout)` | 阻塞式发送    | 简单调试信息   |
  | `HAL_UART_Receive()`             | `(huart, *pData, Size, Timeout)` | 阻塞式接收    | 固定长度协议   |
  | `HAL_UART_Transmit_IT()`         | `(huart, *pData, Size)`          | 中断发送     | 中小数据量    |
  | `HAL_UART_Receive_IT()`          | `(huart, *pData, Size)`          | 中断接收     | 定长数据包    |
  | `HAL_UART_Transmit_DMA()`        | `(huart, *pData, Size)`          | DMA发送    | 大数据量     |
  | `HAL_UARTEx_ReceiveToIdle_DMA()` | `(huart, *pData, Size)`          | DMA+空闲中断 | 不定长数据    |

- **中断回调机制**：
  
  ```c
  // 发送完成回调
  void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
  {
      if(huart->Instance == USART1) {
          tx_complete = 1;  // 设置完成标志
      }
  }
  
  // 接收完成回调（DMA空闲中断特有）
  void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
  {
      if(huart->Instance == USART2) {
          memcpy(rx_buffer, dma_rx_buffer, Size); // 复制有效数据
          HAL_UARTEx_ReceiveToIdle_DMA(huart, dma_rx_buffer, RX_BUFFER_SIZE); // 重启接收
      }
  }
  ```

- **DMA接收关键配置**：
  
  ```c
  // 1. 启用空闲中断
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
  
  // 2. 配置DMA循环模式
  hdma_rx.Instance = DMA1_Stream0;
  hdma_rx.Init.Mode = DMA_CIRCULAR;  // 必须循环模式
  HAL_DMA_Init(&hdma_rx);
  
  // 3. 启动接收（H750特有）
  HAL_UARTEx_ReceiveToIdle_DMA(&huart2, dma_rx_buffer, RX_BUFFER_SIZE);
  ```

### 3.3 高级功能

- **LIN总线模式**：
  
  ```c
  // 启用LIN模式（USART2支持）
  huart2.Instance->CR2 |= USART_CR2_LINEN;
  
  // 发送Break帧（11位低电平）
  HAL_LIN_SendBreak(&huart2);
  
  // 配置LIN同步中断
  HAL_UART_RegisterCallback(&huart2, HAL_UART_RXEVENT_CB_ID, LIN_SyncCallback);
  ```

- **单线半双工模式**：
  
  ```c
  huart1.Instance->CR3 |= USART_CR3_HDSEL; // 启用半双工
  GPIO_InitStruct.Pin = GPIO_PIN_6;        // 共用TX/RX引脚
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;  // 开漏输出
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  ```

- **时钟同步模式**（同步通信）：
  
  ```c
  huart3.Instance->CR2 |= USART_CR2_CLKEN;  // 启用时钟输出
  huart3.Instance->CR2 |= USART_CR2_LBCL;   // 最后位时钟脉冲
  GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
  GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_12; // CK引脚
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  ```

- **错误处理机制**：
  
  | **错误类型** | **标志位**         | **清除方式**                     | **回调函数**                   |
  | -------- | --------------- | ---------------------------- | -------------------------- |
  | 溢出错误     | `UART_FLAG_ORE` | `__HAL_UART_CLEAR_OREFLAG()` | `HAL_UART_ErrorCallback()` |
  | 帧错误      | `UART_FLAG_FE`  | `__HAL_UART_CLEAR_FEFAG()`   | 同上                         |
  | 噪声错误     | `UART_FLAG_NE`  | `__HAL_UART_CLEAR_NEFLAG()`  | 同上                         |
  | 硬件流控错误   | `UART_FLAG_LBD` | `__HAL_UART_CLEAR_LBDFLAG()` | 同上                         |

### 3.4 使用示例（DMA空闲中断接收）

```c
#define RX_BUFFER_SIZE  256
uint8_t dma_rx_buffer[RX_BUFFER_SIZE];
uint8_t app_rx_buffer[128];

// 1. 初始化USART2 (APB1最高80MHz)
__HAL_RCC_USART2_CLK_ENABLE();
__HAL_RCC_GPIOA_CLK_ENABLE();

// 2. 配置PA2(TX)/PA3(RX)
GPIO_InitTypeDef GPIO_InitStruct = {0};
GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

// 3. USART2参数配置
huart2.Instance = USART2;
huart2.Init.BaudRate = 115200;
huart2.Init.WordLength = UART_WORDLENGTH_8B;
huart2.Init.StopBits = UART_STOPBITS_1;
huart2.Init.Parity = UART_PARITY_NONE;
huart2.Init.Mode = UART_MODE_TX_RX;
HAL_UART_Init(&huart2);

// 4. 配置DMA (USART2_RX = DMA1_Stream0)
__HAL_RCC_DMA1_CLK_ENABLE();
hdma_rx.Instance = DMA1_Stream0;
hdma_rx.Init.Request = DMA_REQUEST_USART2_RX;
hdma_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
hdma_rx.Init.PeriphInc = DMA_PINC_DISABLE;
hdma_rx.Init.MemInc = DMA_MINC_ENABLE;
hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
hdma_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
hdma_rx.Init.Mode = DMA_CIRCULAR;        // 关键：循环模式
hdma_rx.Init.Priority = DMA_PRIORITY_HIGH;
HAL_DMA_Init(&hdma_rx);
__HAL_LINKDMA(&huart2, hdmarx, hdma_rx);

// 5. 启动DMA空闲中断接收
__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
HAL_UARTEx_ReceiveToIdle_DMA(&huart2, dma_rx_buffer, RX_BUFFER_SIZE);

// 6. 数据处理回调
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(huart->Instance == USART2 && Size <= sizeof(app_rx_buffer))
    {
        memcpy(app_rx_buffer, dma_rx_buffer, Size); // 复制有效数据
        process_data(app_rx_buffer, Size);          // 应用层处理
    }
    // 重启DMA接收（关键步骤）
    HAL_UARTEx_ReceiveToIdle_DMA(huart, dma_rx_buffer, RX_BUFFER_SIZE);
}
```

## 4. 关键注意事项

1. **引脚复用冲突**：
   
   - USART1 TX可接PA9/PB6，**必须确认AF编号**（H750部分引脚AF编号不同）
   - **避坑**：PB7不能用于USART1_RX（仅支持USART3_RX）

2. **波特率精度陷阱**：
   
   - H750主频480MHz → APB1=80MHz → USARTDIV=80e6/(16×115200)=43.4
   - 实际值43 → 误差1.1% ✅
   - APB1=100MHz时误差达4.5% ❌ → **必须使用CubeMX验证**

3. **DMA接收死锁预防**：
   
   - 永远不要在回调中调用`HAL_UART_Receive_DMA()`
   
   - **正确做法**：
     
     ```c
     void HAL_UARTEx_RxEventCallback(...) {
         // 处理数据后立即重启接收
         HAL_UARTEx_ReceiveToIdle_DMA(huart, buffer, size);
     }
     ```

4. **低功耗设计要点**：
   
   | **模式**  | **操作**              | **H750特殊要求** |
   | ------- | ------------------- | ------------ |
   | STOP0   | `HAL_UART_DeInit()` | 需保持VOS=0     |
   | STOP2   | 启用`USART_CR1_UESM`  | 仅支持APB1外设    |
   | STANDBY | 禁用所有时钟              | 无法保持通信       |

5. **多实例冲突解决方案**：
   
   - **时分复用**：使用`HAL_UART_DeInit()`释放总线
   - **优先级管理**：高优先级通信使用APB2外设（USART1/6）
   - **总线隔离**：关键外设使用独立DMA通道

---

### 4.1 H750特有优化技巧

| **功能**     | **实现方式**                                          | **性能提升** | **典型场景**    |
| ---------- | ------------------------------------------------- | -------- | ----------- |
| **DMA双缓冲** | `HAL_UARTEx_ReceiveToIdle_DMA()` + `DMA_CIRCULAR` | 0丢包接收    | 高速数据采集      |
| **16倍过采样** | `huart->Init.OverSampling = UART_OVERSAMPLING_16` | 波特率误差↓   | 921600bps通信 |
| **时钟补偿**   | 配置`USART_CR1_M1=0` + `USART_CR1_M0=0`             | 避免9位数据错位 | Modbus RTU  |
| **快速唤醒**   | STOP2模式 + USART2唤醒                                | 唤醒时间<3μs | 电池设备待机      |

> **避坑指南**：
> 
> 1. **H750时钟树特殊性**：
>    
>    - USART1/6挂载在D1域（最高160MHz）
>    - USART2/3/4/5/7/8挂载在D2域（最高80MHz）
>    - **错误配置会导致波特率严重偏差**
> 
> 2. **空闲中断限制**：
>    
>    - 空闲中断触发需**≥10位时间**的低电平
>    - 高波特率时（如4Mbps）需确保数据间隔 > 2.5μs
> 
> 3. **DMA传输陷阱**：
>    
>    - 传输长度必须为偶数（H750 DMA对齐要求）
>    - 使用`__HAL_DMA_ENABLE_IT(&hdma, DMA_IT_HT)`实现半传输中断

---

### 4.2 USART工作模式对比图

```c
┌─────────────┬───────────────┬───────────────┬─────────────────────┐
│   模式      │  数据长度     │  中断触发点   │    H750适用场景     │
├─────────────┼───────────────┼───────────────┼─────────────────────┤
│ 标准中断接收 │ 固定长度      │ RXNE标志置位  │ Modbus ASCII        │
│             │ (如10字节)    │               │                     │
├─────────────┼───────────────┼───────────────┼─────────────────────┤
│ 空闲中断     │ 不定长        │ 空闲线检测    │ GPS/NMEA协议        │
│ (推荐)      │ (自动判断)    │               │ 蓝牙透传模块        │
├─────────────┼───────────────┼───────────────┼─────────────────────┤
│ 事件模式     │ 无            │ 无CPU介入     │ 唤醒STOP2模式       │
│             │               │               │ 触发DMA传输         │
└─────────────┴───────────────┴───────────────┴─────────────────────┘
```

---


