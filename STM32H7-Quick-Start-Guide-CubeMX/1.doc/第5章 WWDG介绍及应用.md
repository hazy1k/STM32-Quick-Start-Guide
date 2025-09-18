# 第五章 WWDG介绍及应用

## 1. WWDG简介

WWDG（Window Watchdog，窗口看门狗）是 STM32H750VBT6 的**高精度系统保护机制**，通过**严格的时间窗口约束**实现比 IWDG 更高级的安全监控。与 IWDG 不同，WWDG 要求程序必须在**精确的时间窗口内**执行喂狗操作，否则触发复位。这种设计有效防止了程序跑飞后仍能**错误喂狗**（如死循环中持续喂狗却无法执行关键任务）的致命隐患，是汽车电子、医疗设备等**功能安全（ISO 26262/IEC 62304）** 认证的核心组件。

> 🔍 **关键区别**：
> 
> - **WWDG** = 窗口看门狗（**必须在特定时间窗口内喂狗**，精度±1%）
> - **IWDG** = 独立看门狗（任意时间喂狗，精度±10%）  
>   *WWDG 适用于需严格监控任务执行时序的场景，IWDG 适用于基础看护*

---

### 1.1 WWDG 核心特性（STM32H750VBT6）

| **特性**   | **参数**                                                   | **说明**                               | **安全价值**        |
| -------- | -------------------------------------------------------- | ------------------------------------ | --------------- |
| **时钟源**  | **PCLK1 (100 MHz)**                                      | 由 APB1 总线时钟驱动（需预分频）                  | 与主系统时钟同步，精度高    |
| **计数器**  | **7-bit 递减计数器**                                          | 范围 `0x40–0x7F`（64–127），**不可低于 0x40** | 防止计数器溢出导致误复位    |
| **窗口机制** | **W[6:0] 寄存器**                                           | 喂狗必须在 `T > W` 且 `T < 0x7F` 时执行       | 消除程序跑飞后错误喂狗风险   |
| **复位时间** | **T<sub>out</sub> = (4096 × (T[5:0]+1) × 分频系数) / PCLK1** | 典型范围 **5–1000 ms**（PCLK1=100MHz）     | 精确匹配任务周期        |
| **提前唤醒** | **EWI 中断 (Early Wakeup Interrupt)**                      | 计数器减至 `0x40` 时触发                     | 提供故障处理机会，避免立即复位 |
| **写保护**  | **CR 配置锁**                                               | 修改配置需先写 `WWDG_CR = 0x7F`             | 防止运行时误配置        |

📌 **STM32H750VBT6 安全增强设计**：

- **双阈值保护**：  
  1️⃣ **窗口下限 (W)**：喂狗过早 → 提前复位（防死循环）  
  2️⃣ **窗口上限 (0x7F)**：喂狗过晚 → 溢出复位（防任务超时）
- **调试模式暂停**：通过 `DBGMCU` 配置，调试时暂停计数器（避免干扰）
- **中断可屏蔽**：EWI 中断可被 NVIC 禁用（但复位功能不可禁用）

---

### 1.2 工作原理详解

#### 1.2.1 窗口机制与计数逻辑

- **关键寄存器关系**：
  - **T**（计数器值）：初始值 `0x7F`，递减至 `0x40` 时触发 EWI
  - **W**（窗口值）：必须满足 `W < 0x7F`（典型 `0x50–0x7E`）
  - **喂狗条件**：`W < T < 0x7F`（如 `W=0x55`，则 `T` 必须在 `0x56–0x7E`）

#### 1.2.2 超时时间计算

**公式**：  
**T<sub>out</sub> = (4096 × (T[5:0] + 1) × 分频系数) / PCLK1**

*示例：PCLK1=100MHz, 分频=8, T=0x7B (123)*

- **T[5:0]** = 0x3B (59) → 有效计数值 = 59+1 = 60
- **T<sub>out</sub>** = (4096 × 60 × 8) / 100,000,000 = **19.66 ms**

⚠️ **精度保障**：

- PCLK1 频率稳定 → 误差 **< ±1%**（IWDG 为 ±10%）
- **窗口宽度** = `(T<sub>max</sub> - T<sub>min</sub>) × 100%`  
  *例：T=0x7F, W=0x50 → 窗口宽度 = (127-80)/127 ≈ 37%*  
  **推荐窗口宽度 ≤ 25%**（确保严格监控时序）

---

### 1.3 关键寄存器操作

#### 1.3.1 核心寄存器与配置流程

| **寄存器**          | **关键位/值**    | **功能**         | **操作要点**                    |
| ---------------- | ------------ | -------------- | --------------------------- |
| **CR** (Control) | `T[6:0]`     | 计数器初值 (64–127) | **必须 ≥ 0x40**               |
|                  | `WDGA=1`     | 启动 WWDG        | **最后一步**                    |
| **CFR** (Config) | `W[6:0]`     | 窗口下限值          | **必须 < T**                  |
|                  | `EWI=1`      | 使能提前唤醒中断       | 需配置 NVIC                    |
|                  | `WDGTB[2:0]` | 分频系数 (1–128)   | `000=1, 001=2,..., 111=128` |
| **SR** (Status)  | `EWIF=1`     | EWI 中断挂起标志     | **必须在 ISR 中清除**             |

#### 1.3.2 配置步骤（寄存器级）

```c
// 1. 配置预分频器和窗口值 (CFR)
WWDG->CFR = (3 << 11)        // WDGTB=3 → 分频系数=8
           | (0x50 << 0)     // W=0x50 (窗口下限)
           | (1 << 9);       // 使能 EWI 中断

// 2. 设置计数器初值并启动 (CR)
WWDG->CR = 0x7F              // T=0x7F (计数器初值)
         | (1 << 7);         // WDGA=1 启动 WWDG

// 3. 喂狗操作 (必须在窗口内执行!)
WWDG->CR = (T_value & 0x7F)  // T_value 必须在 W+1 ~ 0x7E 之间
         | (1 << 7);         // 保持 WDGA 置位
```

#### 1.3.3 HAL 库简化操作

```c
WWDG_HandleTypeDef hwwdg;
hwwdg.Instance = WWDG;
hwwdg.Init.Prescaler = WWDG_PRESCALER_8;  // 分频=8
hwwdg.Init.Window    = 0x50;              // 窗口下限
hwwdg.Init.Counter   = 0x7F;              // 计数器初值
hwwdg.Init.EWIMode   = WWDG_EWI_ENABLE;   // 使能提前中断
HAL_WWDG_Init(&hwwdg);                    // 自动启动

// 喂狗 (需确保当前 T 在窗口内)
HAL_WWDG_Refresh(&hwwdg, 0x7B);           // T=0x7B 必须 > W
```

### 1.4 WWDG vs IWDG 深度对比

| **特性**    | **WWDG**          | **IWDG**        | **安全等级**    |
| --------- | ----------------- | --------------- | ----------- |
| **时钟源**   | PCLK1 (100MHz)    | LSI (32kHz)     | WWDG：依赖主时钟  |
| **计数器范围** | 0x40–0x7F (7-bit) | 0–4095 (12-bit) | WWDG：防止低值溢出 |
| **喂狗约束**  | **严格时间窗口**        | 任意时间            | WWDG：★★★★★  |
| **复位精度**  | ±1%               | ±10%            | WWDG：★★★★☆  |
| **提前预警**  | EWI 中断 (可恢复)      | 无               | WWDG：★★★★★  |
| **适用场景**  | ASIL-B/C (汽车)     | SIL-1 (工业)      | WWDG：高安全需求  |
| **典型超时**  | 5–1000 ms         | 100ms–30s       | WWDG：短周期任务  |

> 💡 **STM32H750VBT6 选型建议**：
> 
> - **必用 WWDG 场景**：  
>   ✅ 汽车电子（动力系统、制动控制）  
>   ✅ 医疗设备（生命支持系统）  
>   ✅ 实时控制系统（任务周期波动 < 30%）
> - **禁用 WWDG 场景**：  
>   ❌ 超低功耗设备（Stop 模式下 PCLK1 停止）  
>   ❌ 任务周期波动 > 50% 的系统  
>   ❌ 仅需基础看护的简单应用

## 2. WWDG使用示例-STM32IDE

### 2.1 STM32Cube配置

#### 2.1.1 RCC配置

只在第一章中展示，因为后续内容一样

#### 2.1.2 WWDG配置

![屏幕截图 2025-08-24 165416.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/24-16-54-40-屏幕截图%202025-08-24%20165416.png)

### 2.2 用户代码

#### 2.2.1 WWDG初始胡

```c
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    wwdg.c
  * @brief   This file provides code for the configuration
  *          of the WWDG instances.
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
#include "wwdg.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

WWDG_HandleTypeDef hwwdg1;

/* WWDG1 init function */
/**
* @brief 初始化窗口看门狗
* @param tr: T[6:0],计数器值
* @param tw: W[6:0],窗口值
* @param fprer:分频系数（WDGTB） ,范围:WWDG_PRESCALER_1~WWDG_PRESCALER_128,
* Fwwdg=PCLK3/(4096*2^fprer). 一般 PCLK3=120Mhz
* 溢出时间=(4096*2^fprer)*(tr-0X3F)/PCLK3
* 假设 fprer=4,tr=7f,PCLK3=120Mhz
* 则溢出时间=4096*16*64/120Mhz=34.95ms
* @retval 无
*/
void MX_WWDG1_Init(uint8_t tr, uint8_t wr, uint32_t fprer)
{

  /* USER CODE BEGIN WWDG1_Init 0 */

  /* USER CODE END WWDG1_Init 0 */

  /* USER CODE BEGIN WWDG1_Init 1 */

  /* USER CODE END WWDG1_Init 1 */
  hwwdg1.Instance = WWDG1;
  hwwdg1.Init.Prescaler = fprer;
  hwwdg1.Init.Window = wr;
  hwwdg1.Init.Counter = tr;
  hwwdg1.Init.EWIMode = WWDG_EWI_ENABLE;
  if (HAL_WWDG_Init(&hwwdg1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN WWDG1_Init 2 */

  /* USER CODE END WWDG1_Init 2 */

}

void HAL_WWDG_MspInit(WWDG_HandleTypeDef* wwdgHandle)
{

  if(wwdgHandle->Instance==WWDG1)
  {
  /* USER CODE BEGIN WWDG1_MspInit 0 */

  /* USER CODE END WWDG1_MspInit 0 */
    /* WWDG1 clock enable */
    HAL_RCCEx_WWDGxSysResetConfig(RCC_WWDG1);
    __HAL_RCC_WWDG1_CLK_ENABLE();
    /* WWDG1 interrupt Init */
    HAL_NVIC_SetPriority(WWDG_IRQn, 2, 3);
    HAL_NVIC_EnableIRQ(WWDG_IRQn);
  /* USER CODE BEGIN WWDG1_MspInit 1 */

  /* USER CODE END WWDG1_MspInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

```

#### 2.2.2 中断函数

```c
/**
  * @brief This function handles Window watchdog interrupt.
  */
void WWDG_IRQHandler(void)
{
  /* USER CODE BEGIN WWDG_IRQn 0 */

  /* USER CODE END WWDG_IRQn 0 */
  HAL_WWDG_IRQHandler(&hwwdg1);
  /* USER CODE BEGIN WWDG_IRQn 1 */
  /* USER CODE END WWDG_IRQn 1 */
}

void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg)
{
  // 在此处添加喂狗代码
  if (hwwdg->Instance == WWDG1)
  {
	// 喂狗
	HAL_WWDG_Refresh(hwwdg);
	// 点亮 LED0，指示喂狗操作
	HAL_GPIO_TogglePin(LED_RED_Port, LED_RED_Pin);
	printf("WWDG Early Wakeup Interrupt: Dog fed!\n");
  }
}
```

#### 2.2.3 主函数测试

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
#include "wwdg.h"

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
  /* USER CODE BEGIN 2 */
  bsp_init();
  // 点亮 LED0 延时 300ms，指示系统被复位
  HAL_GPIO_WritePin(LED_BLUE_Port, LED_BLUE_Pin, RESET);
  HAL_Delay(300);
  MX_WWDG1_Init(0x7F, 0x5F, WWDG_PRESCALER_16); // 溢出时间约34.95ms
  /* USER CODE END 2 */
  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  HAL_GPIO_WritePin(LED_BLUE_Port, LED_BLUE_Pin, SET); // 灭
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
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

## 3. IWDG相关函数总结（HAL库）

### 3.1 初始化与配置

- `HAL_WWDG_Init(WWDG_HandleTypeDef *hwwdg)`  
  初始化窗口看门狗（WWDG），**关键前提**：
  
  - **必须使能PCLK1时钟**（WWDG挂载在APB1总线）
  - **启动后不可停止**（仅能通过系统复位重置）

- **`WWDG_InitTypeDef` 结构体成员说明**：
  
  | **成员**      | **说明**      | **有效范围**                                | **H750推荐值**        |
  | ----------- | ----------- | --------------------------------------- | ------------------ |
  | `Prescaler` | APB1时钟预分频系数 | `WWDG_PRESCALER_1` 到 `WWDG_PRESCALER_8` | `WWDG_PRESCALER_8` |
  | `Window`    | 窗口下限值       | 0x40-0x7F (64-127)                      | 0x50(80)           |
  | `Counter`   | 计数器初始值      | Window ≤ Counter ≤ 0x7F                 | 0x7F(127)          |
  | `EWIMode`   | 提前唤醒中断模式    | `WWDG_EWI_DISABLE`, `WWDG_EWI_ENABLE`   | `WWDG_EWI_ENABLE`  |

- **超时时间计算公式**（核心！）：
  
  ```c
  T[超时] = 12288 × (Counter+1) / (PCLK1 × Prescaler)
  T[窗口] = 12288 × (Window+1) / (PCLK1 × Prescaler)
  ```
  
  **H750典型配置**（PCLK1=100MHz）：
  
  ```c
  hwwdg.Instance = WWDG;
  hwwdg.Init.Prescaler = WWDG_PRESCALER_8;  // 8分频
  hwwdg.Init.Window = 80;                   // 0x50 → 窗口开启时间
  hwwdg.Init.Counter = 127;                 // 0x7F → 初始计数值
  hwwdg.Init.EWIMode = WWDG_EWI_ENABLE;     // 启用提前唤醒中断
  HAL_WWDG_Init(&hwwdg);                    // 超时≈1.5ms, 窗口≈0.98ms
  ```

- **预分频系数对照表**：
  
  | **宏定义**            | **分频值** | **最小超时** (PCLK1=100MHz) | **适用场景** |
  | ------------------ | ------- | ----------------------- | -------- |
  | `WWDG_PRESCALER_1` | 4       | 0.49ms                  | 超实时系统    |
  | `WWDG_PRESCALER_2` | 8       | 0.98ms                  | **推荐默认** |
  | `WWDG_PRESCALER_4` | 16      | 1.96ms                  |          |
  | `WWDG_PRESCALER_8` | 32      | 3.92ms                  | 长周期任务    |

### 3.2 看门狗操作核心函数

- `HAL_WWDG_Refresh(WWDG_HandleTypeDef *hwwdg, uint32_t Counter)`  
  **喂狗操作**（必须在窗口期内执行）：
  
  ```c
  HAL_WWDG_Refresh(&hwwdg, 0x7F);  // 重置计数器为127
  ```
  
  > ✅ **窗口机制**：
  > 
  > - 仅当计数器值 **< Window** 且 **> 0x40** 时喂狗有效
  > - 早于窗口期 → 系统复位
  > - 晚于窗口期 → 系统复位

- **提前唤醒中断（EWI）**：
  
  ```c
  // 1. 启动带中断的WWDG
  HAL_WWDG_Start_IT(&hwwdg);
  
  // 2. 中断服务函数 (stm32h7xx_it.c)
  void WWDG_IRQHandler(void) {
      HAL_WWDG_IRQHandler(&hwwdg);
  }
  
  // 3. 用户回调处理
  void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg) {
      // 在复位前进行最后处理（<500us）
      save_critical_data();
      log_error("WWDG即将复位! Last state: 0x%02X", current_state);
  }
  ```

- **状态与错误检测**：
  
  ```c
  HAL_WWDG_GetState(&hwwdg);  // 返回：HAL_WWDG_STATE_BUSY等
  HAL_WWDG_GetError(&hwwdg);  // 获取错误代码（如HAL_WWDG_ERROR_TIMEOUT）
  ```

### **3.3 高级功能与特性**

- **窗口值动态调整**：
  
  ```c
  // 任务关键期缩小窗口（提高监控精度）
  __HAL_WWDG_SET_WINDOW(&hwwdg, 0x60);  // 窗口下限=96
  
  // 任务非关键期扩大窗口（避免误触发）
  __HAL_WWDG_SET_WINDOW(&hwwdg, 0x45);  // 窗口下限=69
  ```

- **低功耗模式行为**：
  
  | **模式**      | **WWDG状态** | **H750特殊处理** |
  | ----------- | ---------- | ------------ |
  | RUN         | 正常计数       |              |
  | SLEEP       | 继续计数       |              |
  | STOP0/STOP1 | **暂停计数**   | 唤醒后需重载计数器    |
  | STOP2       | **完全关闭**   | 无法用于STOP2唤醒  |
  | STANDBY     | **关闭**     | 复位后需重新配置     |

- **复位源检测**：
  
  ```c
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST)) {
      // WWDG导致的复位
      __HAL_RCC_CLEAR_RESET_FLAGS();
      // 分析故障原因（通过备份寄存器）
      last_ewi_counter = READ_REG(RTC->BKP0R);
  }
  ```

- **调试模式冻结**：
  
  ```c
  __HAL_DBGMCU_FREEZE_WWDG();   // 调试时暂停WWDG
  __HAL_DBGMCU_UNFREEZE_WWDG(); // 恢复计数
  ```
  
  > ✅ **开发建议**：调试阶段启用冻结，避免频繁复位

### 3.4 使用示例（完整流程）

```c
WWDG_HandleTypeDef hwwdg = {0};

// 1. 使能时钟（关键！WWDG挂载在APB1）
__HAL_RCC_WWDG_CLK_ENABLE();

// 2. 配置WWDG参数（超时≈1.5ms, 窗口≈0.98ms）
hwwdg.Instance = WWDG;
hwwdg.Init.Prescaler = WWDG_PRESCALER_8;  // 8分频
hwwdg.Init.Window = 80;                   // 窗口下限=80
hwwdg.Init.Counter = 127;                 // 初始=127
hwwdg.Init.EWIMode = WWDG_EWI_ENABLE;     // 启用提前唤醒中断

// 3. 初始化并启动WWDG
if (HAL_WWDG_Init(&hwwdg) != HAL_OK) {
    Error_Handler();
}

// 4. 配置NVIC中断
HAL_NVIC_SetPriority(WWDG_IRQn, 0, 0);    // 最高优先级
HAL_NVIC_EnableIRQ(WWDG_IRQn);

// 5. 主循环中精准喂狗（示例：每1ms喂一次）
while (1) {
    // ... 关键任务执行 ...
    
    // 检查是否进入窗口期（计数器<窗口值）
    if (__HAL_WWDG_GET_COUNTER(&hwwdg) < hwwdg.Init.Window) {
        HAL_WWDG_Refresh(&hwwdg, 127);  // 窗口期内喂狗
    }
    
    osDelay(1);  // 间隔必须严格控制
}

// 6. 提前唤醒中断处理
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg)
{
    // 保存关键数据（必须在500us内完成！）
    WRITE_REG(RTC->BKP1R, system_state);
    WRITE_REG(RTC->BKP2R, fault_code);
}
```

## 4. 关键注意事项

1. **窗口机制陷阱**：
   
   - **必须**在计数器值 **< Window** 且 **> 0x40** 时喂狗
   
   - **错误示例**：
     
     ```c
     // 错误：计数器从127递减，Window=80
     // 127→81期间喂狗 → 触发复位（早于窗口期）
     // 63以下喂狗 → 触发复位（晚于窗口期）
     ```

2. **超时时间计算误区**：
   
   - **H750时钟树特殊性**：
     
     - WWDG挂载在**D2域APB1**（最高100MHz）
     - 若APB1预分频为2 → 实际PCLK1=50MHz → 超时翻倍
   
   - **验证方法**：
     
     ```c
     uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
     float timeout = 12288.0f * (127+1) / (pclk1 * 8);
     ```

3. **提前唤醒中断限制**：
   
   - 中断触发时计数器值 = **0x40**（固定）
   
   - **剩余时间** = (0x40 / (PCLK1 × Prescaler)) × 12288
     
     - PCLK1=100MHz, Prescaler=8 → 仅剩 **491.52μs**
   
   - **必须**在中断中：  
     ✅ 保存关键数据  
     ✅ 记录故障状态  
     ❌ 禁止调用复杂函数（如printf）

4. **多任务系统喂狗策略**：
   
   ```c
   // 任务监控结构体
   typedef struct {
       uint32_t last_feed;
       uint32_t timeout;
   } TaskMonitor;
   
   TaskMonitor tasks[3] = {
       {0, 500},  // 任务1：500ms超时
       {0, 1000}, // 任务2：1000ms超时
       {0, 200}   // 任务3：200ms超时
   };
   
   // 定时器回调中更新任务状态
   void Task_Heartbeat(uint8_t task_id) {
       tasks[task_id].last_feed = HAL_GetTick();
   }
   
   // 主循环喂狗条件
   if ((HAL_GetTick() - tasks[0].last_feed) < tasks[0].timeout &&
       (HAL_GetTick() - tasks[1].last_feed) < tasks[1].timeout &&
       (HAL_GetTick() - tasks[2].last_feed) < tasks[2].timeout) {
       HAL_WWDG_Refresh(&hwwdg, 127);
   }
   ```

5. **与IWDG的协同工作**：
   
   ```c
   ┌─────────────┐     ┌─────────────┐     ┌─────────────┐
   │  WWDG       │     │  IWDG       │     │  系统状态   │
   ├─────────────┤     ├─────────────┤     ├─────────────┤
   │ 严格时间窗口│───→ │ 长周期监控  │───→ │ 安全降级    │
   │ (1-10ms级)  │     │ (1-10秒级)  │     │ 执行        │
   └─────────────┘     └─────────────┘     └─────────────┘
   ```
   
   > ✅ **分层监控策略**：
   > 
   > - WWDG监控实时任务（如电机控制）
   > - IWDG监控整体系统（如通信协议栈）
   > - WWDG复位后IWDG可捕获故障数据

---

### 4.1 H750特有优化技巧

| **场景**       | **解决方案**      | **优势**  | **实现代码**                                                       |
| ------------ | ------------- | ------- | -------------------------------------------------------------- |
| **精确窗口计算**   | 动态计算Prescaler | 匹配任务周期  | `hwwdg.Init.Prescaler = calc_prescaler(desired_timeout);`      |
| **故障诊断增强**   | EWI中断保存上下文    | 复位后分析原因 | `SAVE_REG(RTC->BKP0R, __get_MSP()); SAVE_REG(RTC->BKP1R, LR);` |
| **STOP模式安全** | 唤醒后立即喂狗       | 避免虚假复位  | `HAL_PWREx_EnableInternalWakeUpLine(); HAL_WWDG_Refresh();`    |
| **多核协同**     | 通过HSEM同步喂狗    | 多核系统安全  | `if(HAL_HSEM_1StepLock(12) == HAL_OK) { HAL_WWDG_Refresh(); }` |

> **避坑指南**：
> 
> 1. **窗口值必须小于计数器初始值**：
>    
>    - `Window < Counter`（否则初始化失败）
>    - 最小窗口：`Counter - Window ≥ 1`
> 
> 2. **中断优先级设置**：
>    
>    - WWDG中断优先级**必须高于**所有关键任务
>    - 推荐：抢占优先级=0（最高）
> 
> 3. **H750复位电路特殊性**：
>    
>    - WWDG复位会清除D1/D2域（但备份域保留）
>    - 通过`RTC->BKP0R`在EWI中断保存故障状态
> 
> 4. **生产环境配置**：
>    
>    ```c
>    // 防止调试模式意外禁用WWDG
>    #ifdef NDEBUG
>        HAL_WWDG_Init(&hwwdg);  // 正式版启用WWDG
>    #else
>        __HAL_DBGMCU_FREEZE_WWDG(); // 调试版冻结
>    #endif
>    ```

### 4.2 WWDG窗口机制图解

```c
计数器值
  127 ┌───────────┐
      │           │
      │  禁止喂狗  │ ← 早于窗口期喂狗 → 复位
   80 ┼───┐       │
      │窗 │       │
      │口 │       │
   64 ┼───┼───┐   │
      │   │禁 │   │
      │   │止 │   │
      │   │喂 │   │
      │   │狗 │   │ ← 晚于窗口期喂狗 → 复位
      │   │   │   │
    0 └───┴───┴───┘
      T_window   T_timeout
```

---


