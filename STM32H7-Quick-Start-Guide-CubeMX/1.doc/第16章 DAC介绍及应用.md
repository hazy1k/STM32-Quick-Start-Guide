# 第十六章 DAC介绍及应用

## 1. DAC 简介

DAC（Digital-to-Analog Converter，数模转换器）是 STM32H750VBT6 中用于将**数字值转换为模拟电压输出**的外设，是控制系统、音频播放、波形生成等应用的“执行器官”。STM32H750VBT6 配备 **2 个独立 DAC 通道**（DAC1_CH1、DAC1_CH2），支持 **12-bit 分辨率**、**最高 1 MSPS 输出速率**，可配置为 **电压输出** 或 **电流输出**，并能与 **定时器、DMA** 协同实现**无 CPU 干预的波形生成**。

> 🔍 **核心定位**：
> 
> - **DAC ≠ 普通 PWM 滤波**，而是**高精度、低噪声的模拟信号源**
> - 支持 **单次、连续、触发模式** 输出
> - 可与 **TIM6/TIM7、DMA、ADC** 同步，实现闭环控制
> - 输出电压范围：**0 至 VREF+**（典型 3.3V）

---

### 1.1 DAC 核心特性（STM32H750VBT6）

| **特性**     | **参数**                        | **说明**       | **应用场景** |
| ---------- | ----------------------------- | ------------ | -------- |
| **分辨率**    | 12-bit                        | 输出 4096 级电压  | 精密控制     |
| **输出速率**   | 最高 **1 MSPS**                 | 更新周期 ≥ 1 μs  | 音频/波形生成  |
| **输出模式**   | 电压输出（缓冲）、电流输出（外部运放）           | 缓冲模式驱动能力强    | 传感器激励    |
| **触发源**    | 软件、定时器（TIM6/7/2/3/4/5/8）、EXTI | 精确时序控制       | 同步信号发生   |
| **DMA 支持** | ✅                             | 零 CPU 开销波形输出 | 任意波形发生器  |
| **双通道输出**  | DAC1_CH1 & DAC1_CH2           | 可同步更新        | 差分信号、立体声 |
| **噪声波形生成** | ✅                             | 硬件生成三角波/噪声   | 测试与校准    |
| **模拟看门狗**  | ✅                             | 检测输出异常       | 安全保护     |

📌 **STM32H750VBT6 专属优势**：

- **低噪声设计**：输出噪声 < 1 mV<sub>rms</sub>（缓冲模式）
- **高驱动能力**：可直接驱动 5 kΩ || 50 pF 负载
- **与 TIM6 硬件同步**：`TIM6_TRGO` 可触发 DAC 转换，实现**纳秒级同步精度**
- **支持低功耗模式**：Stop 模式下仍可运行（需 TIM6 配合）

---

### 1.2 DAC 工作原理详解

#### 1.2.1 DAC 架构与输出流程

![屏幕截图 2025-09-08 192748.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/09/08-19-28-01-屏幕截图%202025-09-08%20192748.png)

- **输出电压公式**：  
  **V<sub>OUT</sub> = (DOR / 4095) × VREF+**  
  *示例：DOR=2048, VREF+=3.3V → V<sub>OUT</sub> ≈ 1.65 V*

- **两种写入模式**：
  
  - **普通模式**：写 `DHRx` → 立即更新 `DOR`
  - **保持模式（Wave Mode）**：写 `DHRx` → 触发后更新，实现同步输出

#### 1.2.2 工作模式详解

| **模式**     | **配置方式**           | **触发机制**         | **应用场景** |
| ---------- | ------------------ | ---------------- | -------- |
| **软件触发**   | `SWTRIG=1`         | CPU 写 `SWTRIG` 位 | 单次设定电压   |
| **定时器触发**  | `TEN=1`, `TSEL=x`  | TIM6/TIM7 等更新事件  | 周期性波形生成  |
| **外部触发**   | `TSEL=111`         | EXTI 线           | 事件驱动输出   |
| **噪声/三角波** | `WAVE1=1`, `MAMPx` | 内部 LFSR 或计数器     | 测试与校准    |
| **DMA 模式** | `DMAEN=1`          | DMA 传输数据到 `DHRx` | 任意波形输出   |

- **双通道同步**：
  - 使用 `DHR12RD`（双 12-bit 寄存器）可实现 **CH1 和 CH2 同时更新**
  - 适用于差分信号或立体声音频

---

### 1.3 关键寄存器操作

#### 1.3.1 DAC 主要寄存器

| **寄存器**     | **关键位域**                                         | **功能**         | **说明**    |
| ----------- | ------------------------------------------------ | -------------- | --------- |
| **CR**      | EN1, EN2, TEN1, TEN2, TSEL1, TSEL2, WAVE1, MAMP1 | 通道使能、触发选择、波形模式 | 核心控制      |
| **DHRx**    | DACC1DHR[11:0]                                   | 通道 1 数据（左/右对齐） | 写入数字值     |
| **DORx**    | DACC1DOR[11:0]                                   | 通道 1 输出寄存器     | 只读，反映当前输出 |
| **SWTRIGR** | SWTRIG1, SWTRIG2                                 | 软件触发转换         | 写 1 触发    |
| **SR**      | DMAUDR1, DMAUDR2                                 | DMA 下溢标志       | DMA 传输失败  |
| **CCR**     | C1EN, C2EN                                       | 同步通道使能         | 用于双通道同步   |

#### 1.3.2 配置步骤（DAC1_CH1 定时器触发输出）

```c
// 1. 使能 DAC 时钟
RCC->AHB1ENR |= RCC_AHB1ENR_DAC1EN;

// 2. 配置 PA4 为模拟输出
GPIOA->MODER |= GPIO_MODER_MODER4; // [1:0]=11 → 模拟
GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD4; // 无上下拉

// 3. 配置 DAC 通道 1
DAC->CR &= ~DAC_CR_EN1; // 确保关闭

// 4. 选择触发源为 TIM6
DAC->CR |= DAC_CR_TSEL1_2 | DAC_CR_TSEL1_1; // 110 = TIM6_TRGO
DAC->CR |= DAC_CR_TEN1; // 使能触发

// 5. 使能 DAC1
DAC->CR |= DAC_CR_EN1;

// 6. 启动 TIM6（在外部配置）
// TIM6 将自动触发 DAC 转换
```

#### 1.3.3 HAL 库简化操作

```c
DAC_HandleTypeDef hdac;
DAC_ChannelConfTypeDef sConfig = {0};

hdac.Instance = DAC1;
HAL_DAC_Init(&hdac);

sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO; // TIM6 触发
sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;

HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1);

// 启动 DAC
HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

// 可选：启动 DMA 输出波形
HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)wave_table, TABLE_SIZE, DAC_ALIGN_12B_R);
```

## 2. DAC使用示例-STM32IDE

### 2.1 STM32Cube配置

![屏幕截图 2025-09-09 124609.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/09/09-13-15-25-屏幕截图%202025-09-09%20124609.png)

### 2.2 用户代码

```c
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dac.c
  * @brief   This file provides code for the configuration
  *          of the DAC instances.
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
#include "dac.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

DAC_HandleTypeDef hdac1;

/**
 * @brief       DAC初始化函数
 *   @note      本函数支持DAC1_OUT1/2通道初始化
 *              DAC的输入时钟来自APB1, 时钟频率=120Mhz=8.3ns
 *              DAC在输出buffer关闭的时候, 输出建立时间: tSETTLING = 2us (H750数据手册有写)
 *              因此DAC输出的最高速度约为:500Khz, 以10个点为一个周期, 最大能输出50Khz左右的波形
 *
 * @param       outx: 要初始化的通道. 1,通道1; 2,通道2
 * @retval      无
 */
void MX_DAC1_Init(uint8_t ch)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  DAC_ChannelConfTypeDef sConfig = {0};
  __HAL_RCC_DAC12_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStruct.Pin = (ch == 1) ?GPIO_PIN_4 : GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  hdac1.Instance = DAC1;
  if (HAL_DAC_Init(&hdac1) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT1 config
  */
  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE; // 不使用外部触发
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE; // 不使用输出缓冲
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
  sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_DAC_Start(&hdac1, DAC1_CHANNEL_1);
}

void HAL_DAC_MspInit(DAC_HandleTypeDef* dacHandle)
{
}

void HAL_DAC_MspDeInit(DAC_HandleTypeDef* dacHandle)
{

  if(dacHandle->Instance==DAC1)
  {
  /* USER CODE BEGIN DAC1_MspDeInit 0 */

  /* USER CODE END DAC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_DAC12_CLK_DISABLE();

    /**DAC1 GPIO Configuration
    PA4     ------> DAC1_OUT1
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);

  /* USER CODE BEGIN DAC1_MspDeInit 1 */

  /* USER CODE END DAC1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/**
 * @brief       设置通道1/2输出电压
 * @param       outx: 1,通道1; 2,通道2
 * @param       vol : 0~3300,代表0~3.3V
 * @retval      无
 */
void dac_set(uint8_t ch, uint16_t vol)
{
    double temp = vol;
    temp /= 1000;
    temp = temp * 4096 / 3.3;

    if (temp >= 4096)temp = 4095;   /* 如果值大于等于4096, 则取4095 */

    if (ch == 1)  /* 通道1 */
    {
        HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, temp); /* 12位右对齐数据格式设置DAC值 */
    }
    else            /* 通道2 */
    {
        HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, temp); /* 12位右对齐数据格式设置DAC值 */
    }
}
/* USER CODE END 1 */

```

```c
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "bsp_init.h"
#include "stdio.h" // For printf function

void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MPU_Config(void);

int main(void)
{
  uint32_t adc_value;
  uint16_t dac_value = 1000; // 初始DAC值（12位），约0.8V
  uint8_t key_value;
  MPU_Config();
  HAL_Init();
  SystemClock_Config();
  PeriphCommonClock_Config();
  bsp_init();
  MX_ADC1_Init();
  MX_DAC1_Init(1); // 初始化DAC1通道1
  HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value); // 设置初始DAC值
  printf("ADC&DAC Test\r\n");
  while (1)
  {
    key_value = key_scan(0);
    if (key_value == KEY0_PRES) // 按键0增加电压
    {
      dac_value += 50;
      if (dac_value > 4095) dac_value = 4095; // 限制最大值
      HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
      printf("DAC value increased to: %d\r\n", dac_value);
    }
    if (key_value == KEY1_PRES) // 按键1减小电压
    {
      if (dac_value >= 50)
        dac_value -= 50;
      else
        dac_value = 0; // 限制最小值
      HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
      printf("DAC value decreased to: %d\r\n", dac_value);
    }
    adc_value = adc_get_result_average(ADC_CHANNEL_19, 10); // 读取ADC通道19
    printf("ADC value: %lu\r\n", adc_value);
    float voltage = (float)adc_value * (3.3f / 65536.0f); // 计算电压值（16位ADC）
    printf("Voltage: %.3f V\r\n", voltage);
    HAL_Delay(200); // 延时200ms，避免打印过快
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

## 3. DAC相关函数总结（HAL库）

### 3.1 初始化与配置

- **核心配置流程**（四步关键操作）：
  
  1. **使能时钟**（DAC + GPIO）
  2. **配置GPIO为模拟输出**
  3. **初始化DAC参数**
  4. **启动DAC通道**

- `HAL_DAC_Init(DAC_HandleTypeDef *hdac)`  
  **基础配置示例**（DAC1通道1输出电压）：
  
  ```c
  // 1. 使能DAC和GPIO时钟
  __HAL_RCC_DAC12_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  
  // 2. 配置GPIO（PA4为DAC1_OUT1）
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  // 3. 配置DAC参数
  hdac.Instance = DAC1;
  HAL_DAC_Init(&hdac);
  
  // 4. 配置DAC通道
  DAC_ChannelConfTypeDef sConfig = {0};
  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;  // 禁用采样保持
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;                 // 无触发（软件）
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;     // 启用输出缓冲
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;  // 禁用内部连接
  sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;        // 使用出厂校准
  HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1);
  ```

- **`DAC_InitTypeDef` 结构体成员说明**：
  
  | **成员**            | **说明** | **关键选项**                | **H750特殊说明** |
  | ----------------- | ------ | ----------------------- | ------------ |
  | `DACBufferSwitch` | 缓冲开关   | `ENABLE`, `DISABLE`     | 双通道独立控制      |
  | `DAC_Trig1/2/3/4` | 外部触发源  | `EXTI_LINE_x`, `TIMx_x` | 灵活触发选择       |

- **通道配置**（`DAC_ChannelConfTypeDef`）：
  
  | **成员**                        | **说明** | **有效值**                       | **应用场景** |
  | ----------------------------- | ------ | ----------------------------- | -------- |
  | `DAC_SampleAndHold`           | 采样保持   | `ENABLE`, `DISABLE`           | 波形生成时启用  |
  | `DAC_Trigger`                 | 触发源    | `NONE`, `TIM6_TRGO`, `EXTI_9` | 定时输出     |
  | `DAC_OutputBuffer`            | 输出缓冲   | `ENABLE`, `DISABLE`           | 驱动能力增强   |
  | `DAC_ConnectOnChipPeripheral` | 内部连接   | `ENABLE`, `DISABLE`           | 连接模拟外设   |
  | `DAC_UserTrimming`            | 修调模式   | `FACTORY`, `USER`             | 精度校准     |

- **DAC时钟配置**：
  
  - **H750 DAC时钟源**：
    
    ```c
    // 通过RCC配置
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_DAC12;
    PeriphClkInitStruct.Dac12ClockSelection = RCC_DAC12CLKSOURCE_PLL2;  // 或LSE
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
    ```

### 3.2 DAC操作核心函数

- **基础输出控制**：
  
  | **函数**                | **原型**                                       | **特点** | **应用场景** |
  | --------------------- | -------------------------------------------- | ------ | -------- |
  | `HAL_DAC_Start()`     | `(hdac, Channel)`                            | 启动通道   | 准备输出     |
  | `HAL_DAC_Start_IT()`  | `(hdac, Channel)`                            | 启动+中断  | 波形生成     |
  | `HAL_DAC_Start_DMA()` | `(hdac, Channel, *pData, Length, Alignment)` | 启动+DMA | 高速波形     |
  | `HAL_DAC_Stop()`      | `(hdac, Channel)`                            | 停止输出   |          |
  | `HAL_DAC_SetValue()`  | `(hdac, Channel, Alignment, Data)`           | 设置输出值  | 电压设置     |

- **输出值设置**：
  
  ```c
  // 设置12位右对齐值（0-4095）
  HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 2048);  // 2.5V (假设VREF=5V)
  
  // 设置8位左对齐值（0-255）
  HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_8B_L, 128);
  ```

- **DMA波形生成**：
  
  ```c
  // 正弦波数据表（256点）
  uint16_t sine_wave[256];
  for(int i = 0; i < 256; i++) {
      sine_wave[i] = (uint16_t)(2048 + 2047 * sin(2*M_PI*i/256));
  }
  
  // 启动DMA波形输出
  HAL_DAC_Start_DMA(&hdac, 
                    DAC_CHANNEL_1, 
                    (uint32_t*)sine_wave, 
                    256, 
                    DAC_ALIGN_12B_R);
  ```

- **通道操作函数**：
  
  ```c
  // 启动双通道
  HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
  HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
  
  // 分别设置输出
  HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 1000);
  HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, 3000);
  ```

### 3.3 高级功能与特性

- **采样保持模式**（Sample and Hold）：
  
  ```c
  // 配置采样保持
  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_ENABLE;
  sConfig.DAC_SampleTime = 100;        // 采样时间（DACCLK周期）
  sConfig.DAC_HoldTime = 500;          // 保持时间
  sConfig.DAC_RefreshTime = 1000;      // 刷新时间
  HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1);
  ```

- **外部触发模式**：
  
  | **触发源**   | **宏定义**               | **应用场景** | **H750优势** |
  | --------- | --------------------- | -------- | ---------- |
  | TIM6_TRGO | `DAC_TRIGGER_T6_TRGO` | 定时输出     | 高精度时序      |
  | TIM3_CH4  | `DAC_TRIGGER_T3_CH4`  | PWM同步    | 电机控制       |
  | EXTI_9    | `DAC_TRIGGER_EXTI9`   | 外部事件     | 异步触发       |
  | Software  | `DAC_TRIGGER_NONE`    | 软件控制     | 简单应用       |

- **内部连接功能**：
  
  ```c
  // 连接DAC输出到内部外设（如COMP）
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_ENABLE;
  sConfig.DAC_UserTrimming = DAC_TRIMMING_USER;
  sConfig.DAC_TrimmingValue = 15;  // 用户修调值
  HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1);
  ```

- **双DAC同步**：
  
  ```c
  // 同时更新两个DAC通道
  __HAL_DAC_ENABLE(&hdac, DAC_CHANNEL_1);
  __HAL_DAC_ENABLE(&hdac, DAC_CHANNEL_2);
  __HAL_DAC_CLEAR_FLAG(&hdac, DAC_FLAG_DMAUDR1 | DAC_FLAG_DMAUDR2);
  
  // 同步设置值
  hdac.Instance->DHR12RD = (value1 << 16) | value2;
  ```

### 3.4 使用示例（完整流程）

#### 3.4.1 示例1：DAC输出可调电压

```c
DAC_HandleTypeDef hdac = {0};

// 1. 初始化DAC
void DAC_Voltage_Init(void)
{
    // 使能时钟
    __HAL_RCC_DAC12_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // 配置GPIO (PA4 = DAC1_OUT1)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 初始化DAC
    hdac.Instance = DAC1;
    HAL_DAC_Init(&hdac);

    // 配置通道
    DAC_ChannelConfTypeDef sConfig = {0};
    sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;  // 软件触发
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
    sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
    HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1);

    // 启动DAC通道
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
}

// 2. 设置输出电压函数
void Set_DAC_Voltage(float voltage, float vref)
{
    // 计算12位数字值
    uint32_t dac_value = (uint32_t)((voltage / vref) * 4095);
    if(dac_value > 4095) dac_value = 4095;

    // 设置DAC输出
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
}

// 3. 使用示例
void Example_Usage(void)
{
    DAC_Voltage_Init();

    // 输出1.5V (假设VREF=3.3V)
    Set_DAC_Voltage(1.5f, 3.3f);

    // 输出2.0V
    Set_DAC_Voltage(2.0f, 3.3f);
}
```

#### 3.4.2 示例2：DMA生成正弦波

```c
#define WAVE_TABLE_SIZE 256
uint16_t sine_wave[WAVE_TABLE_SIZE];
DAC_HandleTypeDef hdac = {0};
DMA_HandleTypeDef hdma_dac1 = {0};

// 1. 生成正弦波数据表
void Generate_Sine_Wave(void)
{
    for(int i = 0; i < WAVE_TABLE_SIZE; i++) {
        float angle = 2.0f * M_PI * i / WAVE_TABLE_SIZE;
        sine_wave[i] = (uint16_t)(2048 + 2047 * sinf(angle));
    }
}

// 2. DAC+DMA初始化
void DAC_Sine_Init(void)
{
    // 时钟和GPIO配置（同上）
    __HAL_RCC_DAC12_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // DAC初始化
    hdac.Instance = DAC1;
    HAL_DAC_Init(&hdac);

    // 配置通道（使用定时器触发）
    DAC_ChannelConfTypeDef sConfig = {0};
    sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
    sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;  // TIM6_TRGO触发
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1);

    // 配置DMA
    __HAL_RCC_DMA1_CLK_ENABLE();
    hdma_dac1.Instance = DMA1_Stream6;
    hdma_dac1.Init.Request = DMA_REQUEST_DAC1;
    hdma_dac1.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_dac1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dac1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_dac1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_dac1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_dac1.Init.Mode = DMA_CIRCULAR;  // 循环模式
    hdma_dac1.Init.Priority = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&hdma_dac1);

    // 链接DMA
    __HAL_LINKDMA(&hdac, DMA_Handle1, hdma_dac1);

    // 配置TIM6作为触发源
    MX_TIM6_Init();  // 产生TRGO信号
}

// 3. 启动波形输出
void Start_Sine_Wave(void)
{
    Generate_Sine_Wave();

    // 启动DMA传输
    HAL_DAC_Start_DMA(&hdac, 
                      DAC_CHANNEL_1, 
                      (uint32_t*)sine_wave, 
                      WAVE_TABLE_SIZE, 
                      DAC_ALIGN_12B_R);

    // 启动DAC通道
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

    // 启动TIM6（触发DAC）
    HAL_TIM_Base_Start(&htim6);
}
```

## 4. 关键注意事项

1. **输出缓冲器**：
   
   - **启用缓冲**（`ENABLE`）：  
     ✅ 输出阻抗低  
     ✅ 驱动能力增强  
     ❌ 建立时间稍长
   - **禁用缓冲**（`DISABLE`）：  
     ✅ 响应更快  
     ❌ 输出阻抗高，驱动能力弱

2. **电压参考源**：
   
   | **参考源** | **精度**       | **推荐用途** | **H750配置** |
   | ------- | ------------ | -------- | ---------- |
   | VREF+   | 可变（1.8-3.6V） | 通用       | 外部稳压       |
   | VDDA    | 3.3V         | 简单应用     | 无需外部       |
   | 外部精密    | ±0.1%        | 高精度      | 使用外部基准     |

3. **建立时间**：
   
   - **典型值**：1μs（启用缓冲）
   
   - **影响因素**：
     
     - 负载电容
     - 输出缓冲状态
     - 温度
   
   - **高速应用**：
     
     ```c
     // 确保建立时间足够
     HAL_DAC_SetValue(...);
     __HAL_DELAY(2);  // 延迟2μs
     ```

4. **DMA传输陷阱**：
   
   - **必须链接DMA句柄**：
     
     ```c
     __HAL_LINKDMA(&hdac, DMA_Handle1, hdma_dac1);
     ```
   
   - **数据对齐**：
     
     ```c
     // 12位右对齐时，高16位必须为0
     uint16_t data = 0x0FFF;  // 正确
     uint16_t bad_data = 0xFFFF;  // 错误！
     ```
   
   - **Cache一致性**：
     
     ```c
     // 波形表应在非缓存区域
     __attribute__((section(".dma_buffer"))) uint16_t sine_wave[256];
     ```

5. **低功耗模式影响**：
   
   | **模式**  | **DAC行为** | **H750处理** |
   | ------- | --------- | ---------- |
   | RUN     | 正常工作      |            |
   | SLEEP   | 继续运行      |            |
   | STOP0   | **停止**    | DAC输出保持    |
   | STOP1/2 | **关闭**    | 输出可能不稳定    |
   | STANDBY | **关闭**    |            |

---

### 4.1 H750特有优化技巧

| **场景**    | **解决方案**  | **性能提升** | **实现要点**                   |
| --------- | --------- | -------- | -------------------------- |
| **高速波形**  | DMA+TIM触发 | 频率↑10倍   | `DMA_CIRCULAR`模式           |
| **双通道同步** | 同时更新      | 相位误差=0   | `DHR12RD`寄存器               |
| **低噪声输出** | 采样保持      | 纹波↓90%   | `DAC_SAMPLEANDHOLD_ENABLE` |
| **精密校准**  | 用户修调      | 精度↑      | `DAC_TRIMMING_USER`        |

> **避坑指南**：
> 
> 1. **H750 DAC架构**：
>    
>    - DAC1和DAC2共享部分资源
>    - 双DAC模式支持同步操作
> 
> 2. **GPIO配置陷阱**：
>    
>    - 必须配置为`GPIO_MODE_ANALOG`
>    - **错误示例**：配置为推挽输出 → DAC失效
> 
> 3. **DMA缓冲区对齐**：
>    
>    - 半字传输要求2字节对齐
>    - 使用`__ALIGN_BEGIN/__ALIGN_END`宏
> 
> 4. **模拟干扰防护**：
>    
>    - DAC输出走线远离数字信号
>    - 加入RC低通滤波（如1kΩ+100nF）
>    - 使用独立的模拟电源

---

### 4.2 DAC性能参数对比

| **参数** | **典型值** | **H750特性** | **测试条件**       |
| ------ | ------- | ---------- | -------------- |
| 分辨率    | 12位     | 12-bit     | -              |
| DNL    | ±1 LSB  | 保证无失码      | -              |
| INL    | ±2 LSB  | 积分非线性      | -              |
| 建立时间   | 1μs     | 启用缓冲       | 负载=10kΩ//100pF |
| 输出范围   | 0-VREF  | -          | -              |
| 更新率    | 1MHz    | DMA模式      | -              |

> **重要提示**：
> 
> - DAC是**模拟信号生成的核心**外设
> - DMA模式可实现**连续波形输出**而无需CPU干预
> - 采样保持模式适用于**需要稳定电压**的场景
> - 双通道DAC支持**同步更新**，用于差分信号生成

---
