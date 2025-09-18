# 第二章 EXTI介绍及应用

## 1. NVIC 和 EXTI 简介

NVIC（Nested Vectored Interrupt Controller）和 EXTI（External Interrupt/Event Controller）是 STM32H750VBT6 中断系统的核心组件。**NVIC 是 ARM Cortex-M7 内核的中断管理中枢**，负责仲裁、优先级调度和中断响应；**EXTI 是外设级中断控制器**，专门处理外部信号（如 GPIO 变化）触发的中断或事件。两者协作实现高效、低延迟的中断处理，是实时控制（如电机驱动、通信协议）的关键基础。

---

### 1.1 NVIC 简介

NVIC 是 Cortex-M7 内核的固有组件（非 STM32 独有），管理所有中断源（内核异常 + 外设中断）。STM32H750VBT6 支持 **114 个可屏蔽中断请求（IRQs）**（参考 RM0433 Table 56），包括 EXTI、USART、TIM 等。

#### 1.1.1 优先级系统

NVIC 使用 **8 位优先级寄存器**（实际有效位由分组决定），通过 `SCB->AIRCR.PRIGROUP` 设置抢占优先级（Preemption Priority）和子优先级（Subpriority）的分配：

| **分组值 (PRIGROUP)** | **抢占优先级位数** | **子优先级位数** | **优先级范围示例 (4-bit)** | **关键特性**             |
| ------------------ | ----------- | ---------- | ------------------- | -------------------- |
| `0` (0b1000)       | 4           | 0          | 0–15 (仅抢占)          | 无子优先级，高抢占级必打断低抢占级    |
| `1` (0b1001)       | 3           | 1          | 抢占 0–7, 子优先级 0–1    | 抢占级相同，子优先级高者先执行      |
| `2` (0b1010)       | 2           | 2          | 抢占 0–3, 子优先级 0–3    | **推荐默认值**（平衡灵活性与复杂度） |
| `3` (0b1011)       | 1           | 3          | 抢占 0–1, 子优先级 0–7    | 子优先级主导，抢占级仅两级        |
| `4` (0b1100)       | 0           | 4          | 0–15 (仅子优先级)        | 无嵌套中断，纯排队执行          |

🔹 **核心规则**：

- **数值越小，优先级越高**（0 = 最高，15 = 最低）。
- **抢占优先级**：高抢占级中断可打断低抢占级中断（实现嵌套）。
- **子优先级**：抢占级相同时，子优先级高者先执行；若两者均相同，则按 IRQ 编号顺序执行。

⚠️ **关键注意事项**：

- 优先级分组需在 **系统初始化时一次性设置**（`HAL_NVIC_SetPriorityGrouping()`），后续修改可能导致系统崩溃。
- STM32H750VBT6 优先级分组范围为 **0–4**（对应 `PRIGROUP[4:0] = 0x300`–`0x700` in `AIRCR`）。
- 避免将所有中断设为高优先级，否则低优先级中断可能“饥饿”（无法执行）。

#### 1.1.2 关键寄存器操作

| **寄存器**                           | **功能** | **配置方式**                                            | **典型用途**                       |
| --------------------------------- | ------ | --------------------------------------------------- | ------------------------------ |
| **ISER** (Interrupt Set-Enable)   | 使能中断   | `NVIC->ISER[0] = 1 << (IRQn % 32)`                  | 使能 EXTI0_IRQ（IRQn=6 → `ISER[0] |
| **ICER** (Interrupt Clear-Enable) | 禁用中断   | `NVIC->ICER[0] = 1 << (IRQn % 32)`                  | 临时禁用高负载中断                      |
| **IPR** (Interrupt Priority)      | 设置优先级  | `NVIC->IP[IRQn] = (preemption << 4) \| subpriority` | EXTI0 设抢占级 2、子优先级 1 → `0x21`   |
| **IABR** (Interrupt Active Bit)   | 查询活跃中断 | 只读寄存器                                               | 调试时检查中断状态                      |

📌 **配置步骤（HAL 库流程）**：

1. 设置优先级分组：`HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);`
2. 使能中断：`HAL_NVIC_EnableIRQ(EXTI0_IRQn);`
3. 设置优先级：`HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 1);`

✅ **优势**：

- **低延迟响应**：硬件自动保存上下文，中断入口延迟仅 **12 个时钟周期**（Cortex-M7 特性）。
- **动态优先级调整**：运行时可修改优先级（需谨慎）。

---

### 1.2 EXTI 简介

EXTI 是 STM32 专属外设，**将外部信号（如 GPIO 电平变化）转化为中断或事件请求**。STM32H750VBT6 支持 **24 条 EXTI 线**（EXTI0–EXTI23）：

- **EXTI0–EXTI15**：可映射到任意 GPIO 引脚（通过 SYSCFG 寄存器）。
- **EXTI16–EXTI23**：固定连接内部信号（如 PVD、RTC、USB OTG FS）。

#### 1.2.1 EXTI 的核心特性

🔹 **中断 vs 事件模式**：

- **中断模式**：触发 NVIC 中断，CPU 执行 ISR（有延迟，适合需软件处理场景）。
- **事件模式**：直接触发 DMA/外设（无 CPU 开销，延迟 **< 50 ns**），用于高速数据流。

🔹 **触发方式**（通过 `RTSR`/`FTSR` 配置）：

| **触发类型** | **RTSR** | **FTSR** | **特点**    | **应用场景**           |
| -------- | -------- | -------- | --------- | ------------------ |
| 上升沿触发    | `1`      | `0`      | 低→高变化时触发  | 按键释放检测（上拉输入）       |
| 下降沿触发    | `0`      | `1`      | 高→低变化时触发  | **最常用**：按键按下检测（接地） |
| 双边沿触发    | `1`      | `1`      | 上升/下降沿均触发 | 脉冲计数、编码器信号         |

⚠️ **硬件限制**：

- 同一 EXTI 线（如 EXTI0）只能连接 **1 个 GPIO 引脚**（PA0/PB0/... 互斥，需通过 `SYSCFG_EXTICR1` 选择）。
- 事件模式 **无法触发 NVIC 中断**，但可唤醒低功耗模式（如 Stop Mode）。

#### 1.2.2 EXTI 关键寄存器配置

| **功能**      | **寄存器**                      | **配置值**     | **说明**                                            |
| ----------- | ---------------------------- | ----------- | ------------------------------------------------- |
| **中断/事件选择** | `IMR` (Interrupt Mask)       | `1` = 使能中断  | 与 `EMR` 互斥（通常中断模式 `IMR=1, EMR=0`）                 |
|             | `EMR` (Event Mask)           | `1` = 使能事件  | 事件模式 `IMR=0, EMR=1`                               |
| **触发边沿选择**  | `RTSR` (Rising Trigger)      | `1` = 使能上升沿 |                                                   |
|             | `FTSR` (Falling Trigger)     | `1` = 使能下降沿 |                                                   |
| **挂起标志管理**  | `PR` (Pending Register)      | 读为 `1` 表示挂起 | **必须在 ISR 中清除**（写 `1`）：`EXTI->PR = EXTI_PR_PIF0;` |
|             | `SWIER` (Software Interrupt) | 写 `1` 强制触发  | 用于软件测试中断                                          |

📌 **EXTI 与 GPIO 关联步骤**：

1. 配置 GPIO 为输入模式（如按键用 **上拉输入**：`MODER=00, PUPDR=01`）。
2. 通过 `SYSCFG->EXTICR` 选择 GPIO 引脚（如 PA0 → EXTI0：`SYSCFG->EXTICR[0] &= ~0x000F;`）。
3. 设置 `RTSR`/`FTSR` 选择触发边沿。
4. 使能中断/事件（`EXTI->IMR |= EXTI_IMR_IM0;`）。
5. 配置 NVIC 优先级并使能中断。

⚠️ **经典错误**：

- **未清除挂起位**：导致中断重复触发（ISR 中必须写 `PR` 寄存器）。
- **错误映射 GPIO**：如 PB0 映射到 EXTI0 但未配置 `SYSCFG_EXTICR`，中断失效。

## 2. EXTI使用示例-STM32IDE

### 2.1 STM32Cube配置

#### 2.1.1 RCC配置

只在第一章中展示，因为后续内容一样

#### 2.1.2 EXTI配置

![屏幕截图 2025-08-24 153815.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/24-15-38-34-屏幕截图%202025-08-24%20153815.png)

#### 2.1.3 NVIC配置

![屏幕截图 2025-08-24 153908.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/24-15-39-23-屏幕截图%202025-08-24%20153908.png)

### 2.2 用户代码

#### 2.2.1 EXTI相关宏定义

```c
#ifndef __EXTI_H
#define __EXTI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32h7xx_hal.h"

#define EXTI_KEY1_Pin GPIO_PIN_1
#define EXTI_KEY1_GPIO_Port GPIOA
#define EXTI_KEY1_EXTI_IRQn EXTI1_IRQn

void MX_EXTI_GPIO_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __EXTI_H */

```

#### 2.2.2 EXTI初始化配置

```c
#include "exti.h"

void MX_EXTI_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /*Configure GPIO pin : EXTI_KEY1_Pin */
  GPIO_InitStruct.Pin = EXTI_KEY1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(EXTI_KEY1_GPIO_Port, &GPIO_InitStruct);
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}

```

#### 2.2.3 中断函数

```c
/**
  * @brief This function handles EXTI line1 interrupt.
  */
void EXTI1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI1_IRQn 0 */

  /* USER CODE END EXTI1_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(EXTI_KEY1_Pin);
  /* USER CODE BEGIN EXTI1_IRQn 1 */
  if(HAL_GPIO_ReadPin(EXTI_KEY1_GPIO_Port, EXTI_KEY1_Pin) == GPIO_PIN_RESET)
  {
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
  }
  /* USER CODE END EXTI1_IRQn 1 */
}
```

#### 2.2.4 主函数测试

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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp_init.h"
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
  bsp_init();
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
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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

## 3. EXTI相关函数总结（HAL库）

### 3.1 初始化与配置

- **核心配置流程**（需三步联动）：
  
  1. **GPIO初始化**（配置中断触发模式）
  2. **EXTI线映射**（指定GPIO端口）
  3. **NVIC配置**（设置优先级和使能）

- `HAL_GPIO_Init(GPIO_TypeDef *GPIOx, const GPIO_InitTypeDef *GPIO_Init)`  
  **关键配置**：
  
  ```c
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;  // 下降沿触发中断
  GPIO_InitStruct.Pull = GPIO_PULLUP;           // 内部上拉（按键接地场景）
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  ```
  
  **触发模式选择**：
  
  | **模式宏**                       | **触发条件** | **应用场景**   |
  | ----------------------------- | -------- | ---------- |
  | `GPIO_MODE_IT_RISING`         | 上升沿      | 低电平有效信号    |
  | `GPIO_MODE_IT_FALLING`        | 下降沿      | 高电平有效信号    |
  | `GPIO_MODE_IT_RISING_FALLING` | 双边沿      | 脉冲计数/编码器   |
  | `GPIO_MODE_EVT_xxx`           | 同触发条件    | 事件模式（不进中断） |

- **EXTI线映射规则**：
  
  - 每个GPIO引脚需映射到特定EXTI线（0-15）
  
  - **关键函数**：
    
    ```c
    __HAL_RCC_SYSCFG_CLK_ENABLE();  // 必须先使能SYSCFG时钟！
    HAL_SYSCFG_EXTILineConfig(EXTI_SYSCFG_GPIOA, EXTI_PIN_0); // PA0→EXTI0
    ```
  
  - **端口选择宏**：  
    `EXTI_SYSCFG_GPIOA` / `EXTI_SYSCFG_GPIOB` / ... / `EXTI_SYSCFG_GPIOI`

### 3.2 中断处理核心函数

- `HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin)`  
  **中断向量表中必须调用**：
  
  ```c
  void EXTI0_IRQHandler(void)  // EXTI0专用中断服务函数
  {
      HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0); // 自动清除中断标志
  }
  ```

- `HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)`  
  **用户业务逻辑入口**（弱函数，自动调用）：
  
  ```c
  void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
  {
      if(GPIO_Pin == GPIO_PIN_0) 
      {
          // 安全处理：立即退出避免长时间占用中断
          flag_key_pressed = 1; 
      }
  }
  ```

- **标志位操作宏**（底层调试用）：
  
  ```c
  __HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5);    // 检查EXTI5是否触发
  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5);  // 手动清除标志位
  __HAL_GPIO_EXTI_GENERATE_SWIT(GPIO_PIN_5); // 软件触发测试
  ```

### 3.3 高级功能

- **事件模式（Event Mode）**：
  
  - 配置`GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING`
  - **特点**：不触发NVIC中断，但可：  
    ✅ 触发DMA传输  
    ✅ 唤醒低功耗模式（STOP/STANDBY）  
    ✅ 触发定时器捕获

- **组合中断逻辑**（H750特有）：
  
  ```c
  // 配置EXTI5和EXTI6为AND逻辑（同时触发才中断）
  EXTI->EMR1 |= (EXTI_EMR1_EM5 | EXTI_EMR1_EM6); // 使能事件
  EXTI->RTSR2 |= EXTI_RTSR2_RT80;                // 设置组合条件
  ```
  
  > **应用场景**：双按键同时按下才触发安全动作

- **低功耗唤醒配置**：
  
  ```c
  HAL_PWREx_EnableGPIOPullUp(GPIOC, GPIO_PIN_13); // 唤醒引脚上拉
  HAL_PWREx_EnablePullUpPullDownConfig();         // 使能唤醒配置
  HAL_PWREx_EnableWakeUpPin(EXTI_WAKEUP_PIN1);    // 选择WKUP引脚
  ```

### 3.4 使用示例（完整流程）

```c
// 1. 使能时钟（关键！）
__HAL_RCC_GPIOA_CLK_ENABLE();
__HAL_RCC_SYSCFG_CLK_ENABLE();  // EXTI映射必需SYSCFG时钟

// 2. 配置PA0为下降沿中断
GPIO_InitTypeDef GPIO_InitStruct = {0};
GPIO_InitStruct.Pin = GPIO_PIN_0;
GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
GPIO_InitStruct.Pull = GPIO_PULLUP;  // 外部按键接地
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

// 3. 配置EXTI线映射（PA0→EXTI0）
HAL_SYSCFG_EXTILineConfig(EXTI_SYSCFG_GPIOA, EXTI_PIN_0);

// 4. 配置NVIC优先级
HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0); // 抢占优先级1
HAL_NVIC_EnableIRQ(EXTI0_IRQn);        // 使能中断

// 5. 中断服务函数（stm32h7xx_it.c）
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0); // 标准调用
}

// 6. 用户回调处理
volatile uint8_t key_flag = 0;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_0) 
    {
        key_flag = 1;  // 标志位通知主循环
    }
}

// 7. 主循环处理标志位
while(1) {
    if(key_flag) {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        key_flag = 0;
    }
}
```

## 4. 关键注意事项

1. **时钟使能陷阱**：
   
   - **必须**先使能`__HAL_RCC_SYSCFG_CLK_ENABLE()`才能配置EXTI映射
   - 未使能SYSCFG时钟将导致EXTI配置失效（常见低级错误）

2. **中断线冲突**：
   
   - 一根EXTI线（如EXTI0）只能连接**一个GPIO端口**（PA0/PB0/PC0...任选其一）
   - **错误示例**：同时配置PA0和PB0为EXTI0中断 → 导致不可预测行为

3. **低功耗唤醒限制**：
   
   | **唤醒源**   | **支持模式**    | **H750特殊要求**        |
   | --------- | ----------- | ------------------- |
   | EXTI0-15  | STOP0/STOP1 | 引脚需配置为`GPIO_NOPULL` |
   | PVD       | STOP2       | 需使能`PWR_CR1_ALS`    |
   | WKUP Pins | STANDBY     | 仅支持PA0/PD5          |

4. **中断抖动处理**：
   
   - **硬件方案**：按键引脚加100nF电容 + 10kΩ上拉
   
   - **软件方案**：在回调中记录时间戳，5ms内重复触发忽略
     
     ```c
     static uint32_t last_trig = 0;
     if(HAL_GetTick() - last_trig > 5) {
         // 处理有效中断
         last_trig = HAL_GetTick();
     }
     ```

5. **中断优先级设计**：
   
   - **安全关键中断**（如急停按钮）：优先级0（最高）
   - **普通按键**：优先级≥2（避免阻塞系统时钟中断）
   - **组合中断**：使用`HAL_NVIC_SetPriorityGrouping()`配置分组

---

### 4.1 H750特有优化技巧

| **功能**    | **实现方式**                          | **优势**     | **典型场景** |
| --------- | --------------------------------- | ---------- | -------- |
| **快速唤醒**  | STOP2模式 + EXTI下降沿                 | 唤醒时间仅2.9μs | 电池供电设备   |
| **事件链触发** | GPIO_MODE_EVT_RISING → 触发DMA      | 0延迟数据传输    | 高速数据采集   |
| **组合中断**  | 配置EXTI_RTSR2/FTSR2寄存器             | 多条件逻辑判断    | 安全互锁系统   |
| **软件测试**  | `__HAL_GPIO_EXTI_GENERATE_SWIT()` | 无需硬件连接     | 自动化测试    |

> **避坑指南**：
> 
> 1. 按键中断**永远不要**在回调函数中加`HAL_Delay()`（会阻塞其他中断）
> 2. STOP2模式唤醒后需**重配时钟**：`SystemClock_Config()`
> 3. 调试时开启`__HAL_DBGMCU_FREEZE_IWDG1()`避免看门狗复位

---

### 4.2 EXTI与GPIO协同工作图解

```c
[外部信号] → (GPIO引脚) → [EXTI触发条件]  
                    │  
                    ├─→ 中断模式 → NVIC → HAL_GPIO_EXTI_IRQHandler() → 用户回调  
                    │  
                    └─→ 事件模式 → DMA/定时器/唤醒电路 (不经过CPU)
```

---


