# 第十三章 PWR介绍及应用

## 1. PWR 简介

PWR（Power Control，电源控制）是 STM32H750VBT6 中用于**管理芯片功耗模式、电源域和唤醒机制**的核心外设。它通过控制 **电压调节器、时钟域、备份域访问** 和 **低功耗模式切换**（Sleep/Stop/Standby），实现从 **μA 级待机** 到 **480 MHz 高性能运行** 的灵活切换，是电池供电设备、工业传感器、IoT 终端等对功耗敏感系统的“电源总管”。

> 🔍 **核心定位**：
> 
> - **PWR ≠ 供电电路**，而是**功耗模式的“调度中心”**
> - 负责协调 **CPU、外设、时钟、电压域** 的节能状态
> - 支持 **多种低功耗模式**，满足不同响应时间与功耗需求
> - 与 **RTC、WWDG、EXTI** 协同实现**事件驱动唤醒**

---

### 1.1 电源架构与电压域（STM32H750VBT6）

STM32H750VBT6 采用 **多电源域架构**，PWR 模块负责控制这些域的开关与电压水平：

#### 1.1.1 电压域划分

| **域**               | **名称**          | **功能**              | **典型电压**  | **低功耗状态** |
| ------------------- | --------------- | ------------------- | --------- | --------- |
| **D1域**             | 主域（CPU、DMA1/2）  | 运行 Cortex-M7 和主 DMA | 1.0 V     | 运行/关断     |
| **D2域**             | 外设域（AHB1/2/3）   | 控制 GPIO、定时器、ADC 等   | 1.0 V     | 运行/关断     |
| **D3域**             | 低功耗域（RTC、LPTIM） | 包含 RTC、WWDG、基本定时器   | 1.0 V     | 可独立运行     |
| **V<sub>BAT</sub>** | 备份域             | 保持 RTC、备份寄存器        | 1.8–3.6 V | 永久供电      |

- **关键特性**：
  - **D3 域可独立运行**：即使 D1/D2 域关闭，RTC 仍可工作
  - **备份域**：由 `V<sub>BAT</sub>` 或 `V<sub>DD</sub>` 供电，断电后仍保留数据

#### 1.1.2 电压调节器模式

| **模式**    | **REGULATOR** | **功耗** | **恢复时间** | **适用场景**     |
| --------- | ------------- | ------ | -------- | ------------ |
| **高性能模式** | `REGULATOR0`  | 高      | 快        | 全速运行         |
| **低功耗模式** | `REGULATOR1`  | 低      | 慢        | Stop/Standby |

- **动态电压调节**（DVS）：
  - 运行时可在两种模式间切换（通过 `VOS` 位）
  - **必须在 Flash 等待状态配合下进行**

---

### 1.2 低功耗模式详解

STM32H750VBT6 支持 **3 种低功耗模式**，PWR 控制其进入与退出：

#### 1.2.1 Sleep 模式

- **CPU 停止**，但 **内核外设**（NVIC、SysTick）仍运行
- **时钟**：HCLK 停止，但 Systick 时钟（HSI 或 HSE）仍工作
- **唤醒源**：任意中断/事件
- **功耗**：~100 μA @ 400 MHz
- **恢复时间**：< 2 个 HCLK 周期

```c
// 进入 Sleep 模式（WFI）
__WFI(); // Wait For Interrupt
```

#### 1.2.2 Stop 模式（Stop 0 / Stop 1 / Stop 2）

| **模式**     | **电压调节器**        | **时钟域**        | **功耗** | **唤醒时间** | **典型应用** |
| ---------- | ---------------- | -------------- | ------ | -------- | -------- |
| **Stop 0** | REGULATOR0 (高性能) | D1/D2 停止，D3 运行 | ~20 μA | ~5 μs    | 快速响应传感器  |
| **Stop 1** | REGULATOR1 (低功耗) | D1/D2 停止       | ~10 μA | ~20 μs   | 电池设备     |
| **Stop 2** | REGULATOR1       | D1 停止，D2 运行    | ~15 μA | ~20 μs   | 外设持续工作   |

- **唤醒源**：
  
  - RTC 闹钟 / 周期性唤醒
  - EXTI 中断（PA0、WKUP）
  - LPTIM、I2C、USART 唤醒

- **进入 Stop 模式流程**：

```c
// 1. 使能备份域访问（若使用 RTC）
HAL_PWR_EnableBkUpAccess();

// 2. 配置电压调节器（Stop 1）
__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_SVOS_SCALE3);

// 3. 使能 Stop 模式
SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

// 4. 进入 Stop 1
HAL_PWREx_EnterSTOP1Mode(PWR_STOPENTRY_WFI);
```

#### **1.2.3 Standby 模式**

- **所有域关闭**（D1/D2/D3 除备份域）

- **仅 RTC 和备份寄存器保持供电**

- **功耗**：**~1.3 μA**（典型值）

- **唤醒源**：
  
  - WKUP 引脚上升沿
  - RTC 闹钟 / 唁警
  - TAMP 入侵检测
  - LSE 溢出

- **恢复**：**系统复位**（非中断返回）

```c
// 进入 Standby 模式
HAL_PWREx_EnableWakeUpPin(PWR_WAKEUP_PIN1); // PA0 = WKUP
HAL_PWREx_EnterSTANDBYMode();
```

---

### 1.3 关键寄存器与配置流程

#### 1.3.1 PWR 主要寄存器

| **寄存器**  | **关键位域**        | **功能**               | **说明**              |
| -------- | --------------- | -------------------- | ------------------- |
| **CR1**  | LPDS, PVDE, DBP | 低功耗模式选择、PVD 使能、备份域访问 | `DBP=1` 才能访问 RTC    |
| **CR2**  | BREN, MONEN     | BOR/BP关断、监控使能        | 电源监控                |
| **CR3**  | EWUP1–EWUP6     | WKUP 引脚使能            | `EWUP1=1` 启用 PA0 唤醒 |
| **CSR1** | WUF1–6, SBF     | 唤醒标志、待机标志            | 启动时检查复位源            |
| **SCR**  | CCS, WUF, SBF   | 清除标志                 | 写 1 清除 `SBF`        |

#### 1.3.2 PVD（可编程电压检测）配置

- **功能**：当 `V<sub>DD</sub>` 低于阈值时触发中断或复位
- **阈值**：由 `PVDRT` 和 `PVDFT` 配置（`2.2V–3.5V`）

```c
// 使能 PVD，阈值 2.9V，中断模式
PWR->CR1 |= PWR_CR1_PVDE;
PWR->CR1 |= (5 << 4); // PLS=101 → 2.9V
NVIC_EnableIRQ(PVD_AVD_IRQn);
```

#### 1.3.3 备份域访问配置

```c
// 解锁备份域（用于配置 RTC、TAMP）
PWR->CR1 |= PWR_CR1_DBP; // 使能备份域访问
while (!(PWR->CR1 & PWR_CR1_DBP)); // 等待就绪

// 配置 RTC、备份寄存器...
RTC->WPR = 0xCA; // 解锁 RTC 写保护

//（可选）重新锁定
// PWR->CR1 &= ~PWR_CR1_DBP;
```

## 2. PWR使用实例-STM32IDE

### 2.1 STM32Cube配置

![屏幕截图 2025-09-06 101454.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/09/06-10-48-22-屏幕截图%202025-09-06%20101454.png)

### 2.2 用户代码

```c
#include "pwr.h"
#include "key.h"

// 低功耗模式下的按键初始化(用于唤醒睡眠模式/停止模式)
void pwr_key_init(void)
{
	GPIO_InitTypeDef GPIO_Init;
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_Init.Pin = WK_UP_Pin;
	GPIO_Init.Mode = GPIO_MODE_IT_RISING; // 上升沿中断
	GPIO_Init.Pull = GPIO_PULLDOWN;
	GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(WK_UP_GPIO_Port, &GPIO_Init);
	HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 2);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

// 进入睡眠模式
void pwr_sleep(void)
{
	HAL_SuspendTick(); // 暂停滴答时钟
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI); // 进入睡眠模式
}

void EXTI0_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(WK_UP_Pin);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == WK_UP_Pin)
	{

	}
}

```

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
#include "bsp_init.h"
#include "pwr.h"
#include <stdio.h>

void SystemClock_Config(void);
static void MPU_Config(void);

int main(void)
{
  uint8_t key_value = 0;
  MPU_Config();
  HAL_Init();
  SystemClock_Config();
  bsp_init();
  pwr_key_init();
  while (1)
  {
    key_value = key_scan(0);
    if(key_value == KEY0_PRES)
    {
      HAL_GPIO_WritePin(LED_RED_Port, LED_RED_Pin, RESET);
      printf("Enter Sleep\r\n");
      pwr_sleep(); // 进入睡眠模式
      printf("Exit Sleep\r\n");
      HAL_GPIO_WritePin(LED_RED_Port, LED_RED_Pin, SET);
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

## **3. 电源管理（PWR）相关函数总结（HAL库）**

### 3.1 初始化与配置

- **核心配置流程**（三步关键操作）：
  
  1. **使能PWR时钟**（必须第一步执行）
  2. **配置电压调节器**（VOS等级）
  3. **配置备份域访问**（若使用RTC/BKP）

- `HAL_PWR_EnterSLEEPMode(uint32_t Regulator, uint8_t SLEEPEntry)`  
  进入SLEEP模式（CPU停止，外设运行）  
  **基础配置**：
  
  ```c
  // 1. 使能PWR时钟（关键前提）
  __HAL_RCC_PWR_CLK_ENABLE();
  
  // 2. 配置电压调节器（VOS1最高性能）
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  // 3. 进入SLEEP模式
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
  ```

- **`PWR_RegulatorTypeDef` 电压调节配置**：
  
  | **宏定义**                    | **说明** | **H750适用场景** | **功耗** |
  | -------------------------- | ------ | ------------ | ------ |
  | `PWR_MAINREGULATOR_ON`     | 主调节器开启 | RUN/SLEEP模式  | 正常     |
  | `PWR_LOWPOWERREGULATOR_ON` | 低功耗调节器 | STOP/STANDBY | 低功耗    |
  | `PWR_SVOS_SCALE5`          | 超低功耗模式 | 极低功耗应用       | 最低     |

- **电压缩放配置**（`VOS`等级）：
  
  | **VOS等级**   | **最大频率** | **配置函数**                                                        | **适用模式** |
  | ----------- | -------- | --------------------------------------------------------------- | -------- |
  | **Scale 0** | 480MHz   | `HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0)` | 超频模式     |
  | **Scale 1** | 400MHz   | `PWR_REGULATOR_VOLTAGE_SCALE1`                                  | 高性能      |
  | **Scale 2** | 300MHz   | `PWR_REGULATOR_VOLTAGE_SCALE2`                                  | 平衡模式     |
  | **Scale 3** | 150MHz   | `PWR_REGULATOR_VOLTAGE_SCALE3`                                  | 低功耗      |

- **备份域访问配置**：
  
  ```c
  // 使能备份域写访问（必须在操作RTC/BKP前调用）
  HAL_PWR_EnableBkUpAccess();
  
  // 禁用备份域写访问（提高安全性）
  HAL_PWR_DisableBkUpAccess();
  ```

### 3.2 低功耗模式控制

- **STOP模式进入**：
  
  | **函数**                       | **原型**                   | **特点** | **唤醒源**          |
  | ---------------------------- | ------------------------ | ------ | ---------------- |
  | `HAL_PWR_EnterSTOPMode()`    | `(Regulator, STOPEntry)` | 停止所有时钟 | EXTI, RTC, LPTIM |
  | `HAL_PWREx_EnterSTOP1Mode()` | `(Regulator, STOPEntry)` | 更低功耗   | 同STOP0           |
  | `HAL_PWREx_EnterSTOP2Mode()` | `(Regulator, STOPEntry)` | 最低功耗   | 仅内部唤醒            |

- **STOP模式配置示例**：
  
  ```c
  // 进入STOP1模式
  HAL_PWREx_EnterSTOP1Mode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
  
  // 唤醒后恢复时钟
  SystemClock_Config();  // 必须重新配置时钟
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnableBkUpReg();  // 使能备份域
  ```

- **STANDBY模式**（最低功耗）：
  
  ```c
  // 进入STANDBY模式（所有寄存器丢失）
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
  HAL_PWR_EnterSTANDBYMode();
  
  // 系统复位后继续执行
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_WU)) {
      // 由唤醒引脚触发
      __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
  }
  ```

- **低功耗模式特性对比**：
  
  | **模式**      | **功耗**  | **唤醒时间** | **时钟保持** | **RAM保持** |
  | ----------- | ------- | -------- | -------- | --------- |
  | **SLEEP**   | 10-50mA | < 1μs    | 全部保持     | 全部保持      |
  | **STOP0**   | 1-5mA   | 5μs      | 无        | D1/D2域RAM |
  | **STOP1**   | 0.5-2mA | 10μs     | 无        | D1/D2域RAM |
  | **STOP2**   | 0.1-1mA | 15μs     | 仅LSE/LSI | D1域RAM    |
  | **STANDBY** | < 10μA  | 100μs    | 无        | 仅备份RAM    |

### 3.3 电源监控与事件

- **可编程电压检测**（PVD）：
  
  ```c
  // 配置PVD阈值（PVD Level 5 = 2.9V）
  __HAL_PWR_PVD_CONFIG(PWR_PVDLEVEL_5);
  
  // 使能PVD中断
  HAL_PWR_EnablePVD();
  HAL_PWR_PVD_IRQHandler();  // 在中断中调用
  ```

- **PVD中断处理**：
  
  ```c
  // 1. 配置PVD回调
  void HAL_PWR_PVDCallback(void)
  {
      if (__HAL_PWR_GET_FLAG(PWR_FLAG_PVDO)) {
          // 电压低于阈值
          enter_low_power_mode();
          save_critical_data();
      }
  }
  
  // 2. 中断服务函数
  void PVD_AVD_IRQHandler(void)
  {
      HAL_PWR_PVD_IRQHandler();
  }
  ```

- **电源标志位操作**：
  
  | **标志位** | **宏定义**           | **说明** | **清除方式**                            |
  | ------- | ----------------- | ------ | ----------------------------------- |
  | 唤醒标志    | `PWR_FLAG_WU`     | 从低功耗唤醒 | `__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU)` |
  | PVD输出   | `PWR_FLAG_PVDO`   | 电压低于阈值 | 自动更新                                |
  | 待机标志    | `PWR_FLAG_SB`     | 处于待机模式 | `__HAL_PWR_CLEAR_FLAG()`            |
  | 冗余复位    | `RCC_FLAG_SFTRST` | 软件复位   | `__HAL_RCC_CLEAR_RESET_FLAGS()`     |

- **复位源检测**：
  
  ```c
  void Check_Reset_Source(void)
  {
      if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)) {
          // 外部引脚复位
      }
      if (__HAL_RCC_GET_FLAG(RCC_FLAG_PVDRST)) {
          // PVD导致的复位
          __HAL_RCC_CLEAR_RESET_FLAGS();
      }
      if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB)) {
          // 从STANDBY模式唤醒
      }
  }
  ```

### 3.4 高级功能与特性

- **备份SRAM供电**：
  
  ```c
  // 使能备份SRAM供电
  HAL_PWREx_EnableBkUpReg();
  
  // 写入备份SRAM（16KB）
  *(__IO uint32_t *)BKPSRAM_BASE = 0x12345678;
  
  // 启用写保护
  HAL_PWREx_DisableBkUpReg();
  ```

- **温度传感器控制**：
  
  ```c
  // 启用内部温度传感器
  HAL_PWREx_EnableTemperatureSensor();
  
  // 禁用温度传感器
  HAL_PWREx_DisableTemperatureSensor();
  ```

- **低功耗外设时钟控制**：
  
  ```c
  // 在STOP模式下保持LSE运行
  HAL_PWREx_EnableInternalWakeUpLine();
  
  // 配置RTC时钟源
  __HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSE);
  ```

- **动态电压调节**（DVFS）：
  
  ```c
  // 根据负载动态调整电压
  void Adjust_Voltage_Dynamically(uint8_t load_level)
  {
      switch(load_level) {
          case LOAD_HIGH:
              HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
              SystemClock_Config_HighPerf();
              break;
          case LOAD_LOW:
              HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE3);
              SystemClock_Config_LowPower();
              break;
      }
  }
  ```

### 3.5 使用示例（完整流程）

#### 3.5.1 示例1：STOP2模式低功耗设计

```c
void Enter_LowPower_Mode(void)
{
    // 1. 使能PWR时钟
    __HAL_RCC_PWR_CLK_ENABLE();

    // 2. 配置唤醒源（RTC闹钟）
    HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);

    // 3. 准备进入STOP2
    HAL_SuspendTick();  // 暂停SysTick
    __HAL_RCC_PWR_CLK_ENABLE();

    // 4. 进入STOP2模式
    HAL_PWREx_EnterSTOP2Mode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

    // 5. 唤醒后恢复
    SystemClock_Config();      // 重新配置系统时钟
    HAL_ResumeTick();          // 恢复SysTick
    MX_GPIO_Init();            // 重新初始化GPIO（可选）
}

// RTC闹钟唤醒处理
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    // 唤醒后立即执行
    sensor_sampling();         // 采集传感器数据
    transmit_data();           // 传输数据
    Enter_LowPower_Mode();     // 再次进入低功耗
}
```

#### 3.5.2 示例2：PVD电压监控保护

```c
void PVD_Configuration(void)
{
    // 1. 使能PWR时钟
    __HAL_RCC_PWR_CLK_ENABLE();

    // 2. 配置PVD阈值（2.9V）
    PWR_PVDTypeDef sConfigPVD = {0};
    sConfigPVD.PVDLevel = PWR_PVDLEVEL_5;  // 2.9V
    sConfigPVD.Mode = PWR_PVD_MODE_IT_RISING_FALLING;  // 上升/下降沿中断
    HAL_PWR_ConfigPVD(&sConfigPVD);

    // 3. 使能PVD
    HAL_PWR_EnablePVD();

    // 4. 配置NVIC
    HAL_NVIC_SetPriority(PVD_AVD_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(PVD_AVD_IRQn);
}

// PVD中断回调
void HAL_PWR_PVDCallback(void)
{
    uint32_t vdd = Get_VDD_Voltage();  // ADC测量

    if (vdd < 3000) {  // 低于3.0V
        // 进入安全模式
        system_shutdown = 1;
        save_critical_data();

        // 准备进入STOP模式
        HAL_SuspendTick();
        HAL_PWREx_EnterSTOP1Mode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    }
    else if (vdd > 3100) {  // 恢复到正常
        system_shutdown = 0;
        // 恢复正常操作
    }
}
```

## 4. 关键注意事项

1. **时钟恢复陷阱**：
   
   - 从STOP/STANDBY唤醒后：  
     ✅ **必须重新配置系统时钟**  
     ✅ **重新使能外设时钟**
   
   - **错误示例**：
     
     ```c
     HAL_PWREx_EnterSTOP2Mode();
     // 唤醒后直接使用UART → 失败（时钟未配置）
     ```
   
   - **正确做法**：
     
     ```c
     HAL_PWREx_EnterSTOP2Mode();
     SystemClock_Config();  // 首先恢复时钟
     MX_USART1_UART_Init(); // 再初始化外设
     ```

2. **GPIO状态保持**：
   
   | **模式**    | **GPIO状态** | **H750处理建议**      |
   | --------- | ---------- | ----------------- |
   | SLEEP     | 保持原状态      | 无需处理              |
   | STOP0/1/2 | 保持输出电平     | 配置为`PULL UP/DOWN` |
   | STANDBY   | **丢失**     | 复位后重新初始化          |

3. **备份SRAM使用**：
   
   - H750提供**16KB备份SRAM**（D3域）
   
   - 需要：
     
     ```c
     HAL_PWREx_EnableBkUpReg();  // 使能备份域供电
     ```
   
   - 推荐用途：
     
     - 存储校准参数
     - 记录运行时间
     - 故障日志

4. **STOP模式时钟选择**：
   
   - **STOP2模式**：
     
     - 仅LSE（32.768kHz）和LSI（32kHz）保持运行
     - RTC必须使用LSE/LSI作为时钟源
   
   - **配置**：
     
     ```c
     __HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSE);
     __HAL_RCC_RTC_ENABLE();
     ```

5. **功耗优化技巧**：
   
   - **未使用引脚配置**：
     
     ```c
     GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
     GPIO_InitStruct.Pull = GPIO_NOPULL;
     HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);  // 降低功耗
     ```
   
   - **外设时钟门控**：
     
     ```c
     __HAL_RCC_USART1_CLK_DISABLE();  // 不使用时关闭时钟
     ```

---

### 4.1 H750特有优化技巧

| **场景**   | **解决方案**      | **功耗降低** | **实现要点**                      |
| -------- | ------------- | -------- | ----------------------------- |
| **电池供电** | STOP2 + RTC唤醒 | < 100μA  | `HAL_PWREx_EnterSTOP2Mode()`  |
| **快速响应** | STOP0 + 高速唤醒  | 唤醒<5μs   | 保持高速时钟域                       |
| **电压监控** | PVD + 电池切换    | 防止欠压     | `PWR_PVDLEVEL_3` (2.3V)       |
| **数据保持** | 备份SRAM存储      | 掉电不丢失    | `*(__IO uint32_t*)0x38800000` |

> **避坑指南**：
> 
> 1. **H750多域电源管理**：
>    
>    - D1/D2/D3域可独立控制
>    - `HAL_PWREx_EnterSTOP2Mode()`仅关闭D1/D2域
> 
> 2. **唤醒引脚配置**：
>    
>    - 唤醒引脚（WKUP1-WKUP5）必须配置为：
>      
>      ```c
>      HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
>      ```
>    
>    - 对应GPIO配置为浮空输入
> 
> 3. **温度影响**：
>    
>    - 高温环境下STOP模式功耗可能增加30%
>    - 低温下LSE启动时间延长
> 
> 4. **生产环境测试**：
>    
>    - 必须测试所有唤醒源
>    - 测量实际功耗（使用电流计）
>    - 验证电压阈值精度

---

### 4.2 低功耗模式选择指南

```c
┌─────────────────────────────────────────────────────────────────┐
│                         功耗优化决策树                           │
├─────────────────────────────────────────────────────────────────┤
│                             系统需求                             │
│                                 │                                │
│                 ┌───────────────┴───────────────┐                │
│                 │                               │                │
│        需要快速唤醒(<10μs)             需要极低功耗(<100μA)       │
│                 │                               │                │
│           SLEEP Mode                    STOP2/STANDBY Mode       │
│                 │                               │                │
│        ┌────────┴────────┐            ┌────────┴────────┐       │
│        │                 │            │                 │       │
│  需要外设继续工作    需要最低功耗    需要RAM保持      需要完全关断 │
│        │                 │            │                 │       │
│    SLEEP/STOP0        STOP1/STOP2    STOP2           STANDBY     │
└─────────────────────────────────────────────────────────────────┘
```

**重要提示**：

- **STOP2模式**是电池供电应用的最佳选择（H750功耗可低至50μA）
- 从低功耗模式唤醒后**首要任务是恢复时钟**
- PVD是防止系统欠压运行的**关键保护机制**
- 备份SRAM和RTC的组合可实现**掉电记忆功能**

---


