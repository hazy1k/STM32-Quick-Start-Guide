# 第十二章 RNG介绍及应用

## 1. RNG 简介

RNG（Random Number Generator，随机数生成器）是 STM32H750VBT6 内置的**硬件级真随机数生成外设**，基于芯片内部的**热噪声物理现象**生成真正的随机数，与软件伪随机算法（如 `rand()`）有本质区别。RNG 在 **加密通信、密钥生成、安全认证、防重放攻击** 等安全敏感场景中至关重要，是实现 **TLS/SSL、AES-GCM、数字签名** 等安全协议的基础组件。

> 🔍 **核心定位**：
> 
> - **RNG ≠ 伪随机数**：不依赖种子，输出不可预测
> - 基于 **振荡器抖动（Oscillator Jitter）** 的物理熵源
> - 支持 **32-bit 随机数输出**，符合 NIST SP800-22 随机性标准
> - 可用于 **TRNG（True RNG）+ DRBG（Deterministic RNG）** 混合架构

---

### 1.1 RNG 核心特性（STM32H750VBT6）

| **特性**   | **参数**                           | **说明**                | **安全价值**       |
| -------- | -------------------------------- | --------------------- | -------------- |
| **随机源**  | 模拟热噪声（内部振荡器抖动）                   | 物理熵源，非算法生成            | 抵抗预测攻击         |
| **输出速率** | 最高 **24 Mbps**（@ 200 MHz 时钟）     | 约 800 kSamples/s      | 满足高速加密需求       |
| **输出精度** | 32-bit 无符号整数                     | 每次读取一个 32 位随机数        | 可用于密钥/nonce 生成 |
| **时钟源**  | RNGCLK（来自 RCC）                   | 需启用 HSI48 或 PLL per2Q | 独立时钟保证稳定性      |
| **中断能力** | 数据就绪（DRDY）、时钟错误（CEIS）、数据错误（SEIS） | 实现中断驱动读取              | 避免轮询开销         |
| **错误检测** | 卡住检测（Stuck）、时钟偏差检测               | 符合 AIS31 安全标准         | 防止故障注入攻击       |
| **安全认证** | 支持 NIST SP800-22 测试套件            | 可通过 CAVP 认证           | 工业级安全合规        |

📌 **STM32H750VBT6 专属优势**：

- **双时钟冗余**：支持 HSI48 和 PLLQ 作为 RNG 时钟源
- **硬件去偏**：内置后处理电路，消除位偏差
- **与加密引擎协同**：可直接为 **AES、PKA** 提供随机种子
- **低功耗模式**：可在 Stop 模式下运行（需 HSI48 保持）

---

### 1.2 RNG 工作原理详解

#### 1.2.1 硬件架构与熵源

```mermaid
graph LR
A[模拟熵源] -->|热噪声抖动| B[采样器]
B --> C[数字去偏电路]
C --> D[FIFO 缓冲]
D --> E[32-bit 输出]
```

- **熵源机制**：
  
  - 利用 **内部高速振荡器的相位抖动**（jitter）
  - 采样两个异步时钟域的差异
  - 保证每一位的 **0/1 概率接近 50%**

- **后处理（Post-processing）**：
  
  - **Von Neumann 校正**：消除位偏差（连续 01 输出 1，10 输出 0）
  - **CRC 校验**：检测数据完整性
  - **FIFO**：4 级缓冲，防止数据丢失

#### 1.2.2 随机性质量验证

RNG 输出需满足 **NIST SP800-22** 的 15 项统计测试：

- **频率测试**（Monobit）：0/1 数量接近
- **游程测试**（Runs）：连续相同位长度分布
- **扑克测试**（Poker）：子序列分布
- **自相关测试**（Autocorrelation）：无周期性

> ✅ **STM32H7 RNG 实测结果**：
> 
> - 通过全部 15 项 NIST 测试
> - 位偏差 < 0.01%
> - 相关性 < 0.001

---

### 1.3 关键寄存器操作

#### 1.3.1 RNG 主要寄存器

| **寄存器**  | **关键位域**         | **功能**      | **说明**          |
| -------- | ---------------- | ----------- | --------------- |
| **CR**   | RNGEN, IE, CLKEN | 启用 RNG、中断使能 | `RNGEN=1` 启动    |
| **SR**   | DRDY, CEIS, SEIS | 状态标志        | `DRDY=1` 表示数据就绪 |
| **DR**   | RNDATA[31:0]     | 随机数输出       | 读取后自动清空         |
| **HTCR** | HTRIM[4:0]       | 硬件测试模式      | 仅用于工厂测试         |

#### 1.3.2 RNG 初始化流程（寄存器级）

```c
// 1. 使能 RNG 时钟（RCC）
RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;

// 2. 配置 RNG 时钟源为 HSI48
// HSI48 已由系统自动启用（USB 需要）
__HAL_RCC_HSI48_ENABLE();
while(!__HAL_RCC_GET_FLAG(RCC_FLAG_HSI48RDY));

// 3. 启用 RNG
RNG->CR = RNG_CR_RNGEN; // 启动 RNG

// 4. 等待第一个随机数就绪
while (!(RNG->SR & RNG_SR_DRDY));

// 5. 读取随机数
uint32_t random_val = RNG->DR;
```

#### 1.3.2 HAL 库简化操作

```c
// 初始化 RNG
if (HAL_RNG_Init(&hrng) != HAL_OK) {
    Error_Handler();
}

// 阻塞式读取随机数
uint32_t random_val;
if (HAL_RNG_GenerateRandomNumber(&hrng, &random_val) != HAL_OK) {
    Error_Handler();
}

// 中断方式读取
HAL_RNG_Start_IT(&hrng);
// 在 RNG_IRQHandler 中处理
```

## 2. RNG使用示例-STM32IDE

### 2.1 STM32Cube配置

![屏幕截图 2025-09-05 133030.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/09/05-13-44-22-屏幕截图%202025-09-05%20133030.png)

### 2.2 用户代码

```c
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rng.c
  * @brief   This file provides code for the configuration
  *          of the RNG instances.
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
#include "rng.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

RNG_HandleTypeDef hrng;

/* RNG init function */
void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  hrng.Init.ClockErrorDetection = RNG_CED_ENABLE;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

}

void HAL_RNG_MspInit(RNG_HandleTypeDef* rngHandle)
{

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(rngHandle->Instance==RNG)
  {
  /* USER CODE BEGIN RNG_MspInit 0 */

  /* USER CODE END RNG_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RNG;
    PeriphClkInitStruct.RngClockSelection = RCC_RNGCLKSOURCE_PLL;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* RNG clock enable */
    __HAL_RCC_RNG_CLK_ENABLE();
  /* USER CODE BEGIN RNG_MspInit 1 */

  /* USER CODE END RNG_MspInit 1 */
  }
}

void HAL_RNG_MspDeInit(RNG_HandleTypeDef* rngHandle)
{

  if(rngHandle->Instance==RNG)
  {
  /* USER CODE BEGIN RNG_MspDeInit 0 */

  /* USER CODE END RNG_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RNG_CLK_DISABLE();
  /* USER CODE BEGIN RNG_MspDeInit 1 */

  /* USER CODE END RNG_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

// 获取随机数
uint32_t get_random_number(void)
{
	uint32_t random_number = 0;
	HAL_RNG_GenerateRandomNumber(&hrng, &random_number);
	return random_number;
}

// 获取指定范围的随机数
uint32_t get_random_number_in_range(uint32_t min, uint32_t max)
{
	uint32_t random_number;
	HAL_RNG_GenerateRandomNumber(&hrng, &random_number);
	return (random_number % (max - min + 1)) + min;
}


/* USER CODE END 1 */

```

```c
#include "main.h"
#include "rng.h"
#include "bsp_init.h"
#include "stdio.h" // For printf function

void SystemClock_Config(void);
static void MPU_Config(void);

int main(void)
{
  uint8_t key_value = 0;
  uint32_t random_number = 0;
  MPU_Config();
  HAL_Init();
  SystemClock_Config();
  bsp_init();
  MX_RNG_Init();
  while (1)
  {
    key_value = key_scan(0);
    if(key_value == KEY0_PRES)
    {
      random_number = get_random_number();
	  printf("Random Number: %lu\r\n", random_number);
	  HAL_Delay(200);
    }
    else if(key_value == KEY1_PRES)
	{
	  random_number = get_random_number_in_range(50, 100);
	  printf("Random Number in Range 50-100: %lu\r\n", random_number);
	  HAL_Delay(200);
    }
    else
    {
	  HAL_GPIO_TogglePin(LED_RED_Port, LED_RED_Pin);
	  HAL_Delay(500);
	}
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
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

## 3. RNG相关函数总结（HAL库）

### 3.1 初始化与配置

- `HAL_RNG_Init(RNG_HandleTypeDef *hrng)`  
  初始化随机数生成器（RNG），**关键前提**：
  
  - **必须使能时钟**（RNG挂载AHB2总线）
  - **支持两种模式**：
    1. **轮询模式**：阻塞式获取随机数
    2. **中断模式**：数据就绪时触发中断

- **`RNG_InitTypeDef` 结构体成员说明**：
  
  | **成员**                | **说明** | **有效值**                                | **H750特殊说明** |
  | --------------------- | ------ | -------------------------------------- | ------------ |
  | `ClockErrorDetection` | 时钟错误检测 | `RNG_CED_ENABLE`, `RNG_CED_DISABLE`    | 推荐启用         |
  | `AutoClockOff`        | 自动关时钟  | `RNG_AUTO_CLOCK_OFF_ENABLE`, `DISABLE` | 节能模式         |

- **基础配置流程**：
  
  ```c
  // 1. 使能RNG时钟
  __HAL_RCC_RNG_CLK_ENABLE();
  
  // 2. 配置RNG参数
  hrng.Instance = RNG;
  hrng.Init.ClockErrorDetection = RNG_CED_ENABLE;  // 启用时钟错误检测
  HAL_RNG_Init(&hrng);
  
  // 3. 启动RNG
  HAL_RNG_Start(&hrng);
  ```

- **时钟源与工作原理**：
  
  - RNG使用**内部振荡器**（约48MHz）
  - 通过**采样模拟噪声**生成真随机数
  - **输出速率**：约80kbps（H750）
  - **质量保证**：符合NIST SP800-22随机性测试标准

### **3.2 随机数生成操作**

- **轮询模式获取随机数**：
  
  ```c
  uint32_t random_number;
  
  // 1. 检查数据就绪标志
  if (__HAL_RNG_GET_FLAG(&hrng, RNG_FLAG_DRDY)) {
      // 2. 读取随机数
      random_number = hrng.Instance->DR;
  }
  
  // 或使用HAL库封装函数
  if (HAL_RNG_GenerateRandomNumber(&hrng, &random_number) == HAL_OK) {
      // 成功获取随机数
  }
  ```

- **中断模式获取随机数**：
  
  ```c
  // 1. 启动中断模式
  HAL_RNG_Start_IT(&hrng);
  
  // 2. 中断服务函数 (stm32h7xx_it.c)
  void HASH_RNG_IRQHandler(void)
  {
      HAL_RNG_IRQHandler(&hrng);
  }
  
  // 3. 回调函数处理
  void HAL_RNG_ReadyDataCallback(RNG_HandleTypeDef *hrng, uint32_t random32bit)
  {
      // 保存或处理随机数
      g_random_buffer[buffer_index++] = random32bit;
  
      // 达到所需数量后停止
      if (buffer_index >= REQUIRED_COUNT) {
          HAL_RNG_Stop_IT(hrng);
      }
  }
  ```

- **批量生成随机数**：
  
  ```c
  uint32_t random_buffer[10];
  
  for (int i = 0; i < 10; i++) {
      if (HAL_RNG_GenerateRandomNumber(&hrng, &random_buffer[i]) != HAL_OK) {
          // 处理错误（如时钟错误）
          Error_Handler();
          break;
      }
  }
  ```

- **获取随机数范围**：
  
  ```c
  // 生成0-99之间的随机数
  uint32_t random_0_to_99 = random_number % 100;
  
  // 生成1000-9999之间的随机数
  uint32_t random_4digit = (random_number % 9000) + 1000;
  ```

### 3.3 高级功能与特性

- **时钟错误检测**（关键安全特性）：
  
  ```c
  // 检查时钟错误标志
  if (__HAL_RNG_GET_FLAG(&hrng, RNG_FLAG_CLKER)) {
      // 时钟错误发生
      __HAL_RNG_CLEAR_FLAG(&hrng, RNG_FLAG_CLKER);
      // 重新初始化RNG
      HAL_RNG_DeInit(&hrng);
      HAL_RNG_Init(&hrng);
  }
  ```

- **数据寄存器结构**：
  
  | **位** | **功能** | **读取方式**  | **H750说明** |
  | ----- | ------ | --------- | ---------- |
  | 31:0  | 随机数据   | `RNG->DR` | 每次读取32位    |
  | 其他    | 状态标志   | `RNG->SR` | 需单独检查      |

- **低功耗模式行为**：
  
  | **模式**    | **RNG状态** | **H750处理建议** |
  | --------- | --------- | ------------ |
  | RUN       | 正常工作      |              |
  | SLEEP     | 继续运行      | 可保持工作        |
  | STOP0/1/2 | **停止**    | 需重新初始化       |
  | STANDBY   | **关闭**    | 无法保持         |

- **安全应用模式**：
  
  ```c
  // 安全随机数生成（防侧信道攻击）
  uint32_t Secure_Random_Generate(void)
  {
      uint32_t random_val;
  
      // 禁用中断（防止时间分析）
      __disable_irq();
  
      // 生成随机数
      while (HAL_RNG_GenerateRandomNumber(&hrng, &random_val) != HAL_OK);
  
      // 重新启用中断
      __enable_irq();
  
      return random_val;
  }
  ```

### 3.4 使用示例（完整流程）

#### 3.4.1 示例1：轮询模式生成随机数

```c
RNG_HandleTypeDef hrng = {0};

// 1. 初始化RNG
void RNG_Init(void)
{
    // 使能时钟
    __HAL_RCC_RNG_CLK_ENABLE();

    // 配置RNG
    hrng.Instance = RNG;
    hrng.Init.ClockErrorDetection = RNG_CED_ENABLE;
    HAL_RNG_Init(&hrng);

    // 启动RNG
    HAL_RNG_Start(&hrng);
}

// 2. 生成随机数函数
uint32_t Get_Random_Number(void)
{
    uint32_t random_val;

    // 检查数据就绪
    if (__HAL_RNG_GET_FLAG(&hrng, RNG_FLAG_DRDY)) {
        random_val = hrng.Instance->DR;  // 读取随机数
        return random_val;
    }

    // 轮询等待（可选超时）
    uint32_t timeout = 1000;
    while (!__HAL_RNG_GET_FLAG(&hrng, RNG_FLAG_DRDY)) {
        if (--timeout == 0) {
            return 0;  // 超时错误
        }
    }

    return hrng.Instance->DR;
}

// 3. 使用示例
void Example_Usage(void)
{
    RNG_Init();

    // 生成10个随机数
    for (int i = 0; i < 10; i++) {
        uint32_t rand = Get_Random_Number();
        printf("Random %d: %lu\n", i, rand);
        HAL_Delay(100);
    }
}
```

#### 3.4.2 示例2：中断模式批量生成

```c
#define RNG_BUFFER_SIZE  32
uint32_t rng_buffer[RNG_BUFFER_SIZE];
volatile uint8_t buffer_index = 0;
volatile uint8_t generation_complete = 0;

// 1. 初始化
void RNG_Init_IT(void)
{
    __HAL_RCC_RNG_CLK_ENABLE();

    hrng.Instance = RNG;
    hrng.Init.ClockErrorDetection = RNG_CED_ENABLE;
    HAL_RNG_Init(&hrng);

    // 配置NVIC
    HAL_NVIC_SetPriority(HASH_RNG_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(HASH_RNG_IRQn);

    // 启动中断模式
    HAL_RNG_Start_IT(&hrng);
}

// 2. 回调函数
void HAL_RNG_ReadyDataCallback(RNG_HandleTypeDef *hrng, uint32_t random32bit)
{
    if (buffer_index < RNG_BUFFER_SIZE) {
        rng_buffer[buffer_index++] = random32bit;

        // 检查是否完成
        if (buffer_index >= RNG_BUFFER_SIZE) {
            generation_complete = 1;
            HAL_RNG_Stop_IT(hrng);
        }
    }
}

// 3. 错误处理回调
void HAL_RNG_ErrorCallback(RNG_HandleTypeDef *hrng)
{
    // 时钟错误处理
    if (__HAL_RNG_GET_FLAG(hrng, RNG_FLAG_SECS)) {
        // 安全错误
        Error_Handler();
    }
    if (__HAL_RNG_GET_FLAG(hrng, RNG_FLAG_CECS)) {
        // 时钟错误
        __HAL_RNG_CLEAR_FLAG(hrng, RNG_FLAG_CECS);
        HAL_RNG_DeInit(hrng);
        HAL_RNG_Init(hrng);
        HAL_RNG_Start_IT(hrng);
    }
}
```

## 4. 关键注意事项

1. **时钟错误检测**：
   
   - **必须启用**`RNG_CED_ENABLE`
   - 检测到时钟错误时：  
     ✅ 清除标志位  
     ✅ 重新初始化RNG  
     ❌ 不能继续使用可能被预测的随机数

2. **启动时间要求**：
   
   - RNG需要约**2个APB时钟周期**稳定
   
   - 建议在初始化后添加微小延迟：
     
     ```c
     HAL_RNG_Init(&hrng);
     HAL_Delay(1);  // 等待稳定
     HAL_RNG_Start(&hrng);
     ```

3. **随机数质量验证**：
   
   - 生成的随机数应满足：
     
     - **均匀分布**：各比特0/1概率≈50%
     - **无相关性**：连续随机数无统计关联
   
   - **测试方法**：
     
     ```c
     // 简单测试：检查高位是否变化
     uint32_t prev = Get_Random_Number();
     for (int i = 0; i < 100; i++) {
         uint32_t curr = Get_Random_Number();
         if (curr == prev) {
             // 连续相同值可能表示故障
             RNG_SelfTest_Fail();
         }
         prev = curr;
     }
     ```

4. **低功耗模式影响**：
   
   - **STOP模式**：RNG停止工作
   
   - **恢复策略**：
     
     ```c
     // 从STOP模式唤醒后
     HAL_Power_EnableBkUpReg();  // 使能备份域
     HAL_RNG_DeInit(&hrng);      // 重新初始化
     HAL_RNG_Init(&hrng);
     HAL_RNG_Start(&hrng);
     ```

5. **安全使用原则**：
   
   | **安全风险** | **防护措施**  | **H750实现**                |
   | -------- | --------- | ------------------------- |
   | 侧信道攻击    | 禁用中断生成    | `__disable_irq()`         |
   | 随机数预测    | 不使用软件伪随机数 | 强制使用RNG->DR               |
   | 故障注入     | 时钟错误检测    | `RNG_FLAG_CLKER`          |
   | 数据泄露     | 清除临时变量    | `memset(buffer, 0, size)` |

---

### 4.1 H750特有优化技巧

| **场景**     | **解决方案**     | **安全收益** | **实现要点**             |
| ---------- | ------------ | -------- | -------------------- |
| **加密密钥生成** | RNG+SHA256混合 | 密钥不可预测   | `SHA256(RNG_output)` |
| **防重放攻击**  | RNG生成Nonce   | 防止消息重放   | 存储最近Nonce            |
| **安全启动**   | RNG验证固件      | 防止克隆设备   | 挑战-响应协议              |
| **真随机种子**  | RNG初始化PRNG   | 提高软件随机性  | `srand(RNG->DR)`     |

> **避坑指南**：
> 
> 1. **NVIC中断共享**：
>    
>    - RNG与HASH共用`HASH_RNG_IRQn`
>    
>    - 中断服务函数必须检查来源：
>      
>      ```c
>      void HASH_RNG_IRQHandler(void) {
>          if (__HAL_RNG_GET_FLAG(&hrng, RNG_FLAG_DRDY)) {
>              HAL_RNG_IRQHandler(&hrng);
>          }
>          if (__HAL_HASH_GET_FLAG(&hhash, HASH_FLAG_DCIS)) {
>              HAL_HASH_IRQHandler(&hhash);
>          }
>      }
>      ```
> 
> 2. **随机数范围陷阱**：
>    
>    - 使用`%`运算符可能导致分布不均
>    
>    - **正确做法**：
>      
>      ```c
>      // 生成0-999的随机数（避免偏差）
>      uint32_t Get_Random_0_999(void) {
>          uint32_t max_valid = 0xFFFFFFFF - (0xFFFFFFFF % 1000);
>          uint32_t rnd;
>          do {
>              HAL_RNG_GenerateRandomNumber(&hrng, &rnd);
>          } while (rnd >= max_valid);
>          return rnd % 1000;
>      }
>      ```
> 
> 3. **H750时钟树特殊性**：
>    
>    - RNG时钟源自**系统时钟**（非独立振荡器）
>    - 主时钟不稳定会影响RNG质量
>    - 建议在**稳压电源**下使用
> 
> 4. **生产环境测试**：
>    
>    - 每台设备应运行**随机性测试**
>    - 使用NIST SP800-22测试套件验证
>    - 记录测试结果到OTP区域

---

### 4.2 RNG状态标志详解

| **标志位** | **宏定义**          | **触发条件** | **处理方法**    |
| ------- | ---------------- | -------- | ----------- |
| 数据就绪    | `RNG_FLAG_DRDY`  | 随机数生成完成  | 读取`RNG->DR` |
| 时钟错误    | `RNG_FLAG_CLKER` | 时钟源异常    | 重新初始化       |
| 安全错误    | `RNG_FLAG_SECS`  | 内部故障     | 立即停止使用      |
| 采样超时    | `RNG_FLAG_CECS`  | 采样失败     | 重新初始化       |

> **重要提示**：
> 
> - RNG是**唯一真随机数源**，比软件算法更安全
> - 不可用于**高性能需求**场景（速率有限）
> - **时钟错误检测**是安全系统的必备功能
> - 生成的随机数应立即使用，避免存储泄露

---


