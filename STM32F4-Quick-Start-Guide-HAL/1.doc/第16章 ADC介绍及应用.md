# 第十六章 ADC介绍及应用

## 1. ADC简介

ADC 即模拟数字转换器，英文详称 Analog-to-digital converter，可以将外部的模拟信号转换为数字信号。

STM32F4xx 系列芯片拥有 3 个 ADC，这些 ADC 可以独立使用，其中 ADC1 和 ADC2 还可以组成双重模式（提高采样率）。 STM32 的 ADC 是 12 位逐次逼近型的模拟数字转换器。它有 19 个通道，可测量 16 个外部和 2 个内部信号源和 Vbat 通道的信号 ADC 中的各个通道的A/D 转换可以单次、连续、扫描或间断模式执行。 ADC 的结果可以以左对齐或者右对齐存储在16 位数据寄存器中。 ADC 具有模拟看门狗的特性，允许应用检测输入电压是否超过了用户自定义的阈值上限或下限。

STM32F407 的 ADC 主要特性我们可以总结为以下几条：

1. 可配置 12 位、 10 位、 8 位或 6 位分辨率；

2. 转换结束、注入转换结束和发生模拟看门狗事件时产生中断

3. 单次和连续转换模式

4. 自校准

5. 带内嵌数据一致性的数据对齐

6. 采样间隔可以按通道分别编程

7. 规则转换和注入转换均有外部触发选项

8. 间断模式

9. 双重模式（带 2 个或以上 ADC 的器件）

10. ADC 转换时间： 最大转换速率为 2.4MHz，转换时间为 0.41us

11. ADC 供电要求： 2.4V 到 3.6V

12. ADC 输入范围： VREF–≤VIN≤VREF+

13. 规则通道转换期间有 DMA 请求产生

下面来介绍 ADC 的框图：

![屏幕截图 2025-08-08 153320.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/08-15-33-29-屏幕截图%202025-08-08%20153320.png)

### 1.1 输入电压

在前面 ADC 的主要特性也对输入电压有所提及， ADC 输入范围 VREF–≤VIN≤VREF+，最终还是由 VREF–、 VREF+、 VDDA和 VSSA决定的。下面看一下这几个参数的关系

![屏幕截图 2025-08-08 153402.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/08-15-34-07-屏幕截图%202025-08-08%20153402.png)

从上图可以知道， VDDA 和 VREF+接 VCC3.3，而 VSSA和 VREF-是接地，所以 ADC 的输入范围即 0~3.3V。 R55 默认焊接， R54 默认不焊接。

### 1.2 输入通道

在确定好了 ADC 输入电压后，如何把外部输入的电压输送到 ADC 转换器中呢，在这里引入了 ADC 的输入通道，在前面也提及到了 ADC1 有 16 个外部通道和 3 个内部通道，而 ADC2和 ADC3 只有有 16 个外部通道。 ADC1 的外部通道是通道 17、通道 18 和通道 19，分别连接到内部温度传感器、内部Vrefint和Vbat。

![屏幕截图 2025-08-08 153826.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/08-15-38-31-屏幕截图%202025-08-08%20153826.png)

### 1.3 转换顺序

 当任意 ADCx 多个通道以任意顺序进行一系列转换就诞生了成组转换，这里就有两种成组转换类型：规则组和注入组。规则组就是图上的规则通道，注入组也就是图上的注入通道。为了避免大家对输入通道加上规则通道和注入通道理解有所模糊，后面规则通道以规则组来代称，注入通道以注入组来代称。

规则组允许最多 16 个输入通道进行转换，而注入组允许最多 4 个输入通道进行转换。这里讲解一下规则组和注入组。

规则组（规则通道）

规则组，按字面理解，“规则”就是按照一定的顺序，相当于正常运行的程序，平常用到最多也是规则组。

注入组（注入通道）

注入组，按字面理解，“注入”就是打破原来的状态，相当于中断。当程序执行的时候，中断是可以打断程序的执行。同这个类似，注入组转换可以打断规则组的转换。假如在规则组转换过程中，注入组启动，那么注入组被转换完成之后，规则组才得以继续转换。

![屏幕截图 2025-08-08 153921.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/08-15-39-25-屏幕截图%202025-08-08%20153921.png)

### 1.4 触发源

在配置好输入通道以及转换顺序后，就可以进行触发转换了。 ADC 的触发转换有两种方法：分别是通过软件或外部事件(也就是硬件)触发转换。

我们先来看看通过写软件触发转换的方法。方法是：通过写 ADC_CR2 寄存器的 ADON 这个位来控制，写 1 就开始转换，写 0 就停止转换，这个控制 ADC 转换的方式非常简单。

另一种就是通过外部事件触发转换的方法，有定时器和输入引脚触发等等。这里区分规则组和注入组。方法是：通过 ADC_CR2 寄存器的 EXTSET[2:0]选择规则组的触发源， JEXTSET[2:0]选择注入组的触发源。通过 ADC_CR2 的 EXTTRIG 和 JEXTTRIG 这两位去激活触发源。

### 1.5 转换时间

STM32F407 的 ADC 总转换时间的计算公式如下：

```c
TCONV = 采样时间 + 12 个周期
```

采样时间可通过 ADC_SMPR1 和 ADC_SMPR2 寄存器中的 SMP[2:0]位编程， ADC_SMPR2控制的是通道 0~9， ADC_SMPR1 控制的是通道 10~18。所有通道都可以通过编程来控制使用不同的采样时间，可选采样时间值如下：

- SMP = 000： 3 个 ADC 时钟周期

- SMP = 001： 15 个 ADC 时钟周期

- SMP = 010： 28 个 ADC 时钟周期

- SMP = 011： 56 个 ADC 时钟周期

- SMP = 100： 84 个 ADC 时钟周期

- SMP = 101： 112 个 ADC 时钟周期

- SMP = 110： 144 个 ADC 时钟周期

- SMP = 111： 480 个 ADC 时钟周期

12 个周期是 ADC 输入时钟 ADC_CLK 决定的。 ADC_CLK 是由 APB2 经过分频产生，分频系数是由 RCC_CFGR 寄存器中的 PPRE2[2:0]进行设置，有 2/4/6/8/16 分频选项。

采样时间最小是 3 个时钟周期，这个采样时间下，我们可以得到最快的采样速度。 举个例子，我们采用最高的采样速率，使用采样时间为 3 个 ADC 时钟周期，那么得到：

```c
TCONV = 3 个 ADC 时钟周期 + 12 个 ADC 时钟周期 = 15 个 ADC 时钟周期
```

一般 APB2 的时钟是 84MHz，经过 ADC 分频器的 4 分频后， ADC 时钟频率就为 21MHz。通过换算可得到：

```c
TCONV = 15 个 ADC 时钟周期 = ( 1/21000000) ∗ 15 s = 0.71us
```

### 1.6 数据寄存器

ADC 转换完成后的数据输出寄存器。根据转换组的不同，规则组的完成转换的数据输出到ADC_DR 寄存器，注入组的完成转换的数据输出到 ADC_JDRx 寄存器。假如是使用双重模式，规则组的数据也是存放在 ADC_DR 寄存器。

### 1.7 中断

规则和注入组转换结束时能产生中断，当模拟看门狗状态位被设置时也能产生中断。它们在 ADC_SR 中都有独立的中断使能位，后面讲解 ADC_SR 寄存器时再进行展开。这里讲解一下，模拟看门狗中断以及 DMA 请求。

模拟看门狗中断

模拟看门狗中断发生条件：首先通过ADC_LTR和ADC_HTR寄存器设置低阈值和高阈值，然后开启了模拟看门狗中断后，当被 ADC 转换的模拟电压低于低阈值或者高于高阈值时，就会产生中断。例如我们设置高阈值是 3.0V，那么模拟电压超过 3.0V 的时候，就会产生模拟看门狗中断，低阈值的情况类似。

DMA 请求

规则组和注入组的转换结束后，除了产生中断外，还可以产生 DMA 请求，把转换好的数据存储在内存里面，防止读取不及时数据被覆盖。

## 2. ADC使用示例

### 2.1 单通道ADC采集

#### 2.1.1 ADC初始化

```c
#include "adc.h"
#include "delay.h"

ADC_HandleTypeDef adc_handle;

void adc_init(void)
{
    adc_handle.Instance = ADC1;
    adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4; // 4分频
    adc_handle.Init.Resolution = ADC_RESOLUTION_12B; // 12位精度
    adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc_handle.Init.ScanConvMode = DISABLE;
    adc_handle.Init.ContinuousConvMode = DISABLE;
    adc_handle.Init.NbrOfConversion = 1; // 单次转换
    adc_handle.Init.DiscontinuousConvMode = DISABLE;
    adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adc_handle.Init.DMAContinuousRequests = DISABLE;
    HAL_ADC_Init(&adc_handle);
}
```

#### 2.1.2 ADC通道配置

```c
// ADC通道配置
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    if(hadc->Instance==ADC1)
    {
        GPIO_InitTypeDef GPIO_Initure;
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_ADC1_CLK_ENABLE();
        /* PA5 */
        GPIO_Initure.Pin = GPIO_PIN_5; 
        GPIO_Initure.Mode = GPIO_MODE_ANALOG; // 输入模式
        GPIO_Initure.Pull = GPIO_NOPULL; // 上拉
        HAL_GPIO_Init(GPIOA, &GPIO_Initure);
    }
}
```

#### 2.1.3 设置ADC通道采样时间

```c
/**
 * @brief       设置ADC通道采样时间
 * @param       adcx : adc句柄指针,ADC_HandleTypeDef
 * @param       ch   : 通道号, ADC_CHANNEL_0~ADC_CHANNEL_17
 * @param       stime: 采样时间  0~7, 对应关系为:
 *   @arg       ADC_SAMPLETIME_3CYCLES,  3个ADC时钟周期        ADC_SAMPLETIME_15CYCLES, 15个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_28CYCLES, 28个ADC时钟周期       ADC_SAMPLETIME_56CYCLES, 56个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_84CYCLES, 84个ADC时钟周期       ADC_SAMPLETIME_112CYCLES,112个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_144CYCLES,144个ADC时钟周期      ADC_SAMPLETIME_480CYCLES,480个ADC时钟周期
 * @param       rank: 多通道采集时需要设置的采集编号,
                假设你定义channel1的rank=1，channel2的rank=2，
                那么对应你在DMA缓存空间的变量数组AdcDMA[0] 就i是channel1的转换结果，AdcDMA[1]就是通道2的转换结果。 
                单通道DMA设置为 ADC_REGULAR_RANK_1
 *   @arg       编号1~16：ADC_REGULAR_RANK_1~ADC_REGULAR_RANK_16
 * @retval      无
 */
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch, uint32_t rank, uint32_t stime)
{
    ADC_ChannelConfTypeDef adc_channel;
    adc_channel.Channel = ch;         // 设置ADCX对通道ch
    adc_channel.Rank = rank;          // 设置采样序列
    adc_channel.SamplingTime = stime; // 设置采样时间
    HAL_ADC_ConfigChannel( adc_handle, &adc_channel);   
}
```

#### 2.1.4 获取采样值

```c
// 获取ADC通道ch的转换值 
uint32_t adc_get_result(uint32_t ch)
{
    adc_channel_set(&adc_handle, ch, 1, ADC_SAMPLETIME_480CYCLES); // 设置通道，序列和采样时间
    HAL_ADC_Start(&adc_handle); // 开启ADC
    HAL_ADC_PollForConversion(&adc_handle, 10); // 轮询转换

    return (uint16_t)HAL_ADC_GetValue(&adc_handle); // 返回最近一次ADC1规则组的转换结果
}

// 通道ch的times次转换结果平均值
uint32_t adc_get_result_average(uint32_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < times; t++)  
    {
        temp_val += adc_get_result(ch);
        delay_ms(5);
    }
    return temp_val / times;     
}
```

#### 2.1.5 主函数测试

```c
#include "bsp_init.h"
#include "stdio.h"
#include "adc.h"

int main(void)
{
  uint16_t adc_value;
  float temp;
  bsp_init();
  adc_init();
  LCD_ShowString(30,50,200,16,16,"STM32F4 ADC Test");
  LCD_ShowString(30,110,200,16,16, "ADC1_CH5_VAL:");
  LCD_ShowString(30,130,200,16,16, "ADC1_CH5_VOL:0.000V");
    while(1)
  {
    adc_value = adc_get_result_average(ADC_CHANNEL_5, 10); // 10次采样取平均值
    LCD_ShowxNum(134,110,adc_value,5,16,0);
    temp = (float)adc_value*(3.3/4096); // 计算电压值
    adc_value = temp;
    LCD_ShowxNum(134,130,adc_value,1,16,0); // 显示电压整数部分
    temp -= adc_value;
    temp *= 1000; // 计算电压小数部分
    LCD_ShowxNum(150,130,temp,3,16,0x80);
    LED_TOGGLE(LED0_GPIO_Pin);
    delay_ms(100);
  }
}
```

### 2.2 单通道ADC采集（DMA读取）

#### 2.2.1 ADC初始化

```c
#include "adc.h"
#include "delay.h"

ADC_HandleTypeDef adc_handle;
DMA_HandleTypeDef dma_handle;
uint8_t adc_dma_sta = 0; // 0:未完成，1:完成

void adc_init(void)
{
    adc_handle.Instance = ADC1;
    adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4; // 4分频
    adc_handle.Init.Resolution = ADC_RESOLUTION_12B; // 12位精度
    adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc_handle.Init.ScanConvMode = DISABLE; // 单通道扫描模式禁用
    adc_handle.Init.ContinuousConvMode = DISABLE; // 连续转换模式禁用 (在DMA模式下会通过CR2_CONT使能)
    adc_handle.Init.NbrOfConversion = 1; // 单次转换
    adc_handle.Init.DiscontinuousConvMode = DISABLE;
    adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adc_handle.Init.DMAContinuousRequests = ENABLE; // 使能DMA连续请求 (ADC生成DMA请求后，DMA会持续请求直到传输完成)
    if (HAL_ADC_Init(&adc_handle) != HAL_OK)
    {
        // 初始化失败处理，例如错误打印或死循环
        while(1);
    }
}
```

#### 2.2.2 ADC通道初始化

```c
// ADC通道配置
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    if(hadc->Instance == ADC1)
    {
        GPIO_InitTypeDef GPIO_Initure;
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_ADC1_CLK_ENABLE();
        /* PA5 作为ADC输入 */
        GPIO_Initure.Pin = GPIO_PIN_5;
        GPIO_Initure.Mode = GPIO_MODE_ANALOG; // 模拟输入模式
        GPIO_Initure.Pull = GPIO_NOPULL; // 无上拉/下拉
        HAL_GPIO_Init(GPIOA, &GPIO_Initure);
        // 关联ADC和DMA中断
        HAL_NVIC_SetPriority(DMA2_Stream4_IRQn, 2, 2); // DMA中断优先级
        HAL_NVIC_EnableIRQ(DMA2_Stream4_IRQn);
    }
}
```

#### 2.2.3 设置ADC通道采样时间

```c
/**
 * @brief       设置ADC通道采样时间
 * @param       adc_handle : adc句柄指针,ADC_HandleTypeDef
 * @param       ch   : 通道号, ADC_CHANNEL_0~ADC_CHANNEL_17
 * @param       rank: 多通道采集时需要设置的采集编号, 单通道DMA设置为 ADC_REGULAR_RANK_1
 * @param       stime: 采样时间
 * @retval      无
 */
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch, uint32_t rank, uint32_t stime)
{
    ADC_ChannelConfTypeDef adc_channel;
    adc_channel.Channel = ch;         // 设置ADCX对通道ch
    adc_channel.Rank = rank;          // 设置采样序列
    adc_channel.SamplingTime = stime; // 设置采样时间
    if (HAL_ADC_ConfigChannel(adc_handle, &adc_channel) != HAL_OK)
    {
        // 通道配置失败处理
        while(1);
    }
}
```

#### 2.2.4 DMA配置

```c
// DMA配置
void adc_dma_init(uint32_t address)
{
    // 确保ADC先初始化，因为DMA需要关联到ADC句柄
    adc_init();

    // 确定DMA时钟使能
    __HAL_RCC_DMA2_CLK_ENABLE(); // 对于DMA2_Stream4，直接使能DMA2时钟即可

    dma_handle.Instance = DMA2_Stream4;
    dma_handle.Init.Channel = DMA_CHANNEL_0; // 对应ADC1的DMA请求，通常是通道0
    dma_handle.Init.Direction = DMA_PERIPH_TO_MEMORY; // 外设到存储器
    dma_handle.Init.PeriphInc = DMA_PINC_DISABLE; // 外设地址不递增
    dma_handle.Init.MemInc = DMA_MINC_ENABLE; // 存储器地址递增
    dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // 外设数据宽度半字 (16位)
    dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD; // 存储器数据宽度半字 (16位)
    dma_handle.Init.Mode = DMA_CIRCULAR; 
    dma_handle.Init.Priority = DMA_PRIORITY_MEDIUM; // DMA优先级中等
    dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE; // 禁用FIFO
    dma_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL; // FIFO阈值
    dma_handle.Init.MemBurst = DMA_MBURST_SINGLE; // 存储器突发模式单次传输
    dma_handle.Init.PeriphBurst = DMA_PBURST_SINGLE; // 外设突发模式单次传输

    if (HAL_DMA_Init(&dma_handle) != HAL_OK)
    {
        // DMA初始化失败处理
        while(1);
    }

    // 关联DMA句柄到ADC句柄
    __HAL_LINKDMA(&adc_handle, DMA_Handle, dma_handle);

    // 设置ADC通道用于DMA传输 (这里我们只用一个通道，PA5/ADC_CHANNEL_5)
    adc_channel_set(&adc_handle, ADC_CHANNEL_5, 1, ADC_SAMPLETIME_56CYCLES); // 设置通道，序列和采样时间

    // 开启ADC连续转换模式（对于DMA通常需要，以便ADC自动开始下一次转换）
    SET_BIT(adc_handle.Instance->CR2, ADC_CR2_CONT); // 使能连续转换

    // 启动DMA ADC转换
    // 第三个参数是数据传输的次数。在DMA_CIRCULAR模式下，DMA会自动循环。
    if (HAL_ADC_Start_DMA(&adc_handle, (uint32_t*)address, ADC_DMA_BUFFSIZE) != HAL_OK)
    {
        // 启动DMA转换失败处理
        while(1);
    }

    // 开启DMA传输完成中断 (在DMA_CIRCULAR模式下，每次缓冲区填充满时会触发)
    __HAL_DMA_ENABLE_IT(&dma_handle, DMA_IT_TC);
}

// 在DMA_CIRCULAR模式下，此函数通常只用于首次启动DMA，或从停止状态重新启动。
// 一旦DMA以CIRCULAR模式启动，它会自行循环，无需重复调用。
void adc_dma_enable(uint16_t times)
{
    // 检查DMA是否已经处于活跃状态，如果已经活跃，则不执行任何操作。
    // 如果DMA处于HAL_DMA_STATE_READY或HAL_DMA_STATE_BUSY_TC，则不应再次启动。
    // 这里简单地禁用和重新使能，以确保状态被重置。
    // 在DMA_CIRCULAR模式下，HAL_ADC_Start_DMA 已经启动了，所以这个函数可能变得多余。
    // 如果您想使用它来停止和重新启动，可以保留逻辑。
    __HAL_ADC_DISABLE(&adc_handle); // 禁用ADC
    __HAL_DMA_DISABLE(&dma_handle); // 禁用DMA

    // 在DMA_CIRCULAR模式下，NDTR会被HAL_ADC_Start_DMA设置，并自动重置。
    // 重新设置NDTR在这里可能没有必要，甚至可能导致问题，因为HAL_ADC_Start_DMA会处理它。
    // dma_handle.Instance->NDTR = times; // 考虑注释掉或删除此行

    __HAL_DMA_ENABLE(&dma_handle); // 重新使能DMA
    __HAL_ADC_ENABLE(&adc_handle); // 重新使能ADC

    // 启动ADC规则组转换（软件触发），这将启动DMA传输
    SET_BIT(adc_handle.Instance->CR2, ADC_CR2_SWSTART); // 启动规则转换通道
}


// DMA中断函数
void DMA2_Stream4_IRQHandler(void)
{
    // HAL库中断处理函数会自动处理DMA中断标志和调用回调函数
    HAL_DMA_IRQHandler(&dma_handle);
}

// DMA传输完成回调函数
// 在DMA_CIRCULAR模式下，每次整个缓冲区被填充完毕后，此回调函数都会被调用。
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1)
    {
        adc_dma_sta = 1; // 完成标志位置1
        // 在DMA_CIRCULAR模式下，DMA会自动重新开始传输，无需在此处手动重新启动。
    }
}
```

#### 2.2.5 主函数测试

```c
#include "bsp_init.h" // 假设包含了LCD初始化和显示函数，以及LED控制
#include "stdio.h"
#include "adc.h"

uint16_t adc_dma_buf[ADC_DMA_BUFFSIZE]; // DMA缓冲区
extern uint8_t adc_dma_sta; // DMA传输完成标志

int main(void)
{
  uint16_t i;
  uint32_t sum;
  float voltage_val;

  bsp_init(); 

  // 初始化ADC DMA采集。
  // 在此函数内部，DMA已经被设置为CIRCULAR模式并启动。
  adc_dma_init((uint32_t)adc_dma_buf);

  LCD_ShowString(30, 50, 200, 16, 16, "STM32F4 ADC DMA Test");
  LCD_ShowString(30, 110, 200, 16, 16, "ADC1_CH5_VAL:");
  LCD_ShowString(30, 130, 200, 16, 16, "ADC1_CH5_VOL:0.000V");

  // 由于 adc_dma_init 已经通过 HAL_ADC_Start_DMA 启动了DMA和ADC
  // 并且DMA是CIRCULAR模式，这里不需要再调用 adc_dma_enable。
  // 如果您在 adc_dma_init 中没有调用 HAL_ADC_Start_DMA，那么在这里调用一次是必需的。
  // 目前的 adc_dma_init 已经包含了 HAL_ADC_Start_DMA，所以这行可以注释掉或移除。
  // adc_dma_enable(ADC_DMA_BUFFSIZE); // 此行在此方案中不再需要

  while(1)
  {
      // 检查DMA传输是否完成一个缓冲区的数据
      if(adc_dma_sta)
      {
          sum = 0;
          // 遍历DMA缓冲区，计算平均值
          for(i = 0; i < ADC_DMA_BUFFSIZE; i++)
          {
              sum += adc_dma_buf[i];
          }
          // 计算平均ADC值
          uint16_t avg_adc_value = sum / ADC_DMA_BUFFSIZE;
          LCD_ShowxNum(134, 110, avg_adc_value, 4, 16, 0); // 显示平均ADC值

          // 计算电压值 (3.3V 参考电压，12位ADC，最大值为4095)
          voltage_val = (float)avg_adc_value * (3.3f / 4095.0f); // 12位ADC最大值为4095

          // 显示电压整数部分
          uint16_t voltage_int = (uint16_t)voltage_val;
          LCD_ShowxNum(134, 130, voltage_int, 1, 16, 0);

          // 显示电压小数部分 (取小数点后三位)
          uint16_t voltage_frac = (uint16_t)((voltage_val - voltage_int) * 1000);
          LCD_ShowxNum(150, 130, voltage_frac, 3, 16, 0x80);
          adc_dma_sta = 0; // 清除完成标志，等待下一次DMA传输完成回调
      }
      LED_TOGGLE(LED0_GPIO_Pin);
      delay_ms(100);
  }
}
```

### 2.3 多通道ADC采集（DMA读取）

#### 2.3.1 ADC相关参数宏定义

```c
#ifndef __ADC_H
#define __ADC_H

#include "sys.h"

// 定义ADC相关的GPIO和ADC外设
#define SINGLE_ADC_GPIO_PORT              GPIOA
#define SINGLE_ADC_GPIO_PIN               GPIO_PIN_5
#define SINGLE_ADC_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define ADC_INSTANCE                      ADC1
#define ADC_CLK_ENABLE()                  __HAL_RCC_ADC1_CLK_ENABLE()

// 定义ADC DMA相关的宏
#define ADC_DMA_STREAM                    DMA2_Stream4
#define ADC_DMA_CHANNEL                   DMA_CHANNEL_0
#define ADC_DMA_IRQn                      DMA2_Stream4_IRQn

// 多通道ADC DMA采集通道数
#define MULTI_ADC_CHANNEL_NUM             6

// 外部声明
extern uint8_t g_adc_dma_transfer_complete_flag;
extern uint16_t g_adc_dma_buffer[]; // 声明外部缓冲区

void adc_multi_channel_dma_init(void);
void adc_multi_channel_dma_enable(uint16_t num_transfers);
void DMA2_Stream4_IRQHandler(void);

#endif /* __ADC_H */
```

#### 2.3.2 GPIO初始化

```c
#include "delay.h"
#include "adc.h"
#include "led.h"

ADC_HandleTypeDef g_adc_handle;
DMA_HandleTypeDef g_dma_adc_handle;
uint8_t g_adc_dma_transfer_complete_flag = 0;

// 外部声明缓冲区
extern uint16_t g_adc_dma_buffer[];

// 多通道 GPIO 初始化
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC_INSTANCE)
    {
        GPIO_InitTypeDef GPIO_InitStructure;
        ADC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | 
                              GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
        GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStructure.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
}
```

#### 2.3.3 设置ADC通道采样时间

```c
// 设置ADC通道采样时间
void adc_set_channel_config(ADC_HandleTypeDef *hadc, uint32_t channel, uint32_t rank, uint32_t sample_time)
{
    ADC_ChannelConfTypeDef adc_channel_config;
    adc_channel_config.Channel = channel; // 通道
    adc_channel_config.Rank = rank; // 通道序号
    adc_channel_config.SamplingTime = sample_time; // 采样时间
    HAL_ADC_ConfigChannel(hadc, &adc_channel_config);
}
```

#### 2.3.4 ADC多通道DMA初始化

```c
// ADC多通道DMA初始化函数
void adc_multi_channel_dma_init(void)
{
    // 使能ADC时钟和DMA时钟
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();

    // 初始化ADC
    g_adc_handle.Instance = ADC_INSTANCE; // ADC1
    g_adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4; // 4分频
    g_adc_handle.Init.Resolution = ADC_RESOLUTION_12B; // 12位精度
    g_adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT; // 数据右对齐
    g_adc_handle.Init.ScanConvMode = ENABLE; // 扫描模式
    g_adc_handle.Init.ContinuousConvMode = ENABLE; // 连续转换模式
    g_adc_handle.Init.DiscontinuousConvMode = DISABLE; // 不连续转换模式
    g_adc_handle.Init.NbrOfConversion = MULTI_ADC_CHANNEL_NUM; // 转换通道数-6
    g_adc_handle.Init.NbrOfDiscConversion = 0; // 不连续转换通道数
    g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START; // 软件触发
    g_adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; // 无外部触发信号
    g_adc_handle.Init.DMAContinuousRequests = ENABLE; // DMA连续请求模式
    HAL_ADC_Init(&g_adc_handle);

    // 初始化DMA
    g_dma_adc_handle.Instance = ADC_DMA_STREAM; // DMA2_Stream4
    g_dma_adc_handle.Init.Channel = ADC_DMA_CHANNEL; // DMA_CHANNEL_0
    g_dma_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY; // 从外设到内存
    g_dma_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE; // 外设地址不增
    g_dma_adc_handle.Init.MemInc = DMA_MINC_ENABLE; // 内存地址增
    g_dma_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    g_dma_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    g_dma_adc_handle.Init.Mode = DMA_CIRCULAR; // 循环模式
    g_dma_adc_handle.Init.Priority = DMA_PRIORITY_HIGH; // 高优先级
    g_dma_adc_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE; // 禁用FIFO
    HAL_DMA_Init(&g_dma_adc_handle);

    // 链接DMA到ADC
    __HAL_LINKDMA(&g_adc_handle, DMA_Handle, g_dma_adc_handle);

    // 设置多通道采样规则序列
    adc_set_channel_config(&g_adc_handle, ADC_CHANNEL_0, 1, ADC_SAMPLETIME_480CYCLES);
    adc_set_channel_config(&g_adc_handle, ADC_CHANNEL_1, 2, ADC_SAMPLETIME_480CYCLES);
    adc_set_channel_config(&g_adc_handle, ADC_CHANNEL_2, 3, ADC_SAMPLETIME_480CYCLES);
    adc_set_channel_config(&g_adc_handle, ADC_CHANNEL_3, 4, ADC_SAMPLETIME_480CYCLES);
    adc_set_channel_config(&g_adc_handle, ADC_CHANNEL_4, 5, ADC_SAMPLETIME_480CYCLES);
    adc_set_channel_config(&g_adc_handle, ADC_CHANNEL_5, 6, ADC_SAMPLETIME_480CYCLES);

    // 配置DMA中断
    HAL_NVIC_SetPriority(ADC_DMA_IRQn, 0, 0);  // 最高优先级
    HAL_NVIC_EnableIRQ(ADC_DMA_IRQn);
}

// 使能ADC DMA传输
void adc_multi_channel_dma_enable(uint16_t num_transfers)
{
    // 启动ADC DMA传输
    HAL_ADC_Start_DMA(&g_adc_handle, (uint32_t*)g_adc_dma_buffer, num_transfers);
    // 软件触发启动转换
    SET_BIT(g_adc_handle.Instance->CR2, ADC_CR2_SWSTART);
}
```

#### 2.3.5 中断函数

```c
// ADD DMA中断回调函数
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC_INSTANCE) // 确保是ADC1
    {
        static uint32_t callback_count = 0;
        callback_count++;
        g_adc_dma_transfer_complete_flag = 1; // 标记DMA传输完成
        if(callback_count == 100)
        {
            LED_TOGGLE(LED1_GPIO_Pin); // 测试用
            callback_count = 0;
        }
    }
}

// DMA中断函数
void DMA2_Stream4_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_dma_adc_handle);
}
```

#### 2.3.6 主函数测试

```c
#include "bsp_init.h"
#include "adc.h"
#include <stdio.h>

// 定义ADC DMA缓冲区
#define ADC_DMA_SAMPLES_PER_CHANNEL 50 // 单通道时DMA缓冲区大小
#define ADC_DMA_BUFFER_SIZE (ADC_DMA_SAMPLES_PER_CHANNEL * MULTI_ADC_CHANNEL_NUM) // 多通道时DMA缓冲区大小：单通道大小 * 通道数
uint16_t g_adc_dma_buffer[ADC_DMA_BUFFER_SIZE]; // DMA缓冲区

// LCD显示文本
const char *lcd_title[] = {
    "STM32F407",
    "ADC 6CH DMA TEST",
    "There are 6 channels"
};

const char *lcd_channel_info_val[] = {
    "ADC1_CH0_VAL:",
    "ADC1_CH1_VAL:",
    "ADC1_CH2_VAL:",
    "ADC1_CH3_VAL:",
    "ADC1_CH4_VAL:",
    "ADC1_CH5_VAL:",
};

const char *lcd_channel_info_vol[] = {
    "ADC1_CH0_VOL:0.000V",
    "ADC1_CH1_VOL:0.000V",
    "ADC1_CH2_VOL:0.000V",
    "ADC1_CH3_VOL:0.000V",
    "ADC1_CH4_VOL:0.000V",
    "ADC1_CH5_VOL:0.000V",
};

int main(void)
{
    uint32_t channel_sum;  // 通道数
    uint16_t adc_raw_value; // ADC原始值
    float voltage_value; // 计算得来电压值

    // 启动ADC DMA采集
    bsp_init();
    adc_multi_channel_dma_init();
    adc_multi_channel_dma_enable(ADC_DMA_BUFFER_SIZE);

    // 初始化LCD显示
    LCD_ShowString(30, 50, 200, 16, 16, (char*)lcd_title[0]);
    LCD_ShowString(30, 70, 200, 16, 16, (char*)lcd_title[1]);
    LCD_ShowString(30, 90, 200, 16, 16, (char*)lcd_title[2]);

    for (uint8_t i = 0; i < MULTI_ADC_CHANNEL_NUM; i++)
    {
        LCD_ShowString(30, 110 + (i * 30), 200, 12, 12, (char*)lcd_channel_info_val[i]);
        LCD_ShowString(30, 122 + (i * 30), 200, 12, 12, (char*)lcd_channel_info_vol[i]);
    }

    while (1)
    {
        if(g_adc_dma_transfer_complete_flag) // DMA采集完成
        {
            // 处理并显示每个通道的数据
            for (uint8_t channel_index = 0; channel_index < MULTI_ADC_CHANNEL_NUM; channel_index++)
            {
                channel_sum = 0; 
                // 计算每个通道的平均值
                for (uint16_t sample_index = 0; sample_index < ADC_DMA_SAMPLES_PER_CHANNEL; sample_index++)
                {
                    channel_sum += g_adc_dma_buffer[sample_index * MULTI_ADC_CHANNEL_NUM + channel_index];
                }
                adc_raw_value = channel_sum / ADC_DMA_SAMPLES_PER_CHANNEL;
                // 显示ADC原始值
                LCD_ShowxNum(108, 110 + (channel_index * 30), adc_raw_value, 4, 12, 0);
                // 计算并显示电压值
                voltage_value = (float)adc_raw_value * (3.3f / 4096.0f);
                // 显示电压值
                uint16_t integer_part = (uint16_t)voltage_value;
                uint16_t decimal_part = (uint16_t)((voltage_value - integer_part) * 1000);
                LCD_ShowxNum(108, 122 + (channel_index * 30), integer_part, 1, 12, 0);
                LCD_ShowxNum(120, 122 + (channel_index * 30), decimal_part, 3, 12, 0X80);
            }
            g_adc_dma_transfer_complete_flag = 0; // 开始下一轮
        }
        LED_TOGGLE(LED0_GPIO_Pin);
        HAL_Delay(100);
    }
}
```

### 2.4 单通道ADC过采样（16位）

#### 2.4.1 主函数

```c
#include "bsp_init.h"
#include "stdio.h"
#include "adc.h"

extern uint8_t adc_dma_sta; // DMA传输完成标志
extern ADC_HandleTypeDef adc_handle;
/* ADC过采样技术, 是利用ADC多次采集的方式, 来提高ADC精度, 采样速度每提高4倍
 * 采样精度提高 1bit, 同时, ADC采样速度降低4倍, 如提高4bit精度, 需要256次采集
 * 才能得出1次数据, 相当于ADC速度慢了256倍. 理论上只要ADC足够快, 我们可以无限
 * 提高ADC精度, 但实际上ADC并不是无限快的, 而且由于ADC性能限制, 并不是位数无限
 * 提高结果就越好, 需要根据自己的实际需求和ADC的实际性能来权衡.
 */
#define ADC_OVERSAMPLE_TIMES 256 // 过采样次数
#define ADC_DMA_BUF_SZIE ADC_OVERSAMPLE_TIMES*10 // DMA缓冲区大小
uint16_t ADC_DMA_BUF[ADC_DMA_BUF_SZIE];

int main(void)
{
  uint16_t i;
  uint32_t adc_value,sum;
  float temp;
  bsp_init(); 

  // 初始化ADC DMA采集。
  // 在此函数内部，DMA已经被设置为CIRCULAR模式并启动。
  adc_dma_init((uint32_t)&ADC_DMA_BUF);
  LCD_ShowString(30, 50, 200, 16, 16, "STM32F4 ADC DMA Test");
  LCD_ShowString(30, 110, 200, 16, 16, "ADC1_CH5_VAL:");
  LCD_ShowString(30, 130, 200, 16, 16, "ADC1_CH5_VOL:0.000V");

  // 由于 adc_dma_init 已经通过 HAL_ADC_Start_DMA 启动了DMA和ADC
  // 并且DMA是CIRCULAR模式，这里不需要再调用 adc_dma_enable。
  // 如果您在 adc_dma_init 中没有调用 HAL_ADC_Start_DMA，那么在这里调用一次是必需的。
  // 目前的 adc_dma_init 已经包含了 HAL_ADC_Start_DMA，所以这行可以注释掉或移除。
  adc_dma_enable(ADC_DMA_BUF_SZIE); // 此行在此方案中不再需要
  while(1)
  {
    if(adc_dma_sta)
    {
        sum=0;
        for(i=0;i<ADC_DMA_BUF_SZIE;i++)
        {
            sum += ADC_DMA_BUF[i];
        }
        adc_value = sum/(ADC_DMA_BUF_SZIE/ADC_OVERSAMPLE_TIMES); // 计算平均值
        adc_value >>= 4; // 除以2^4倍, 得到12+4位 ADC精度值, 注意: 提高 N bit精度, 需要 >> N
        LCD_ShowxNum(134,110,adc_value,5,16,0);
        temp = (float)adc_value*(3.3/65536);
        adc_value = temp;
        LCD_ShowxNum(134,130,adc_value,1,16,0);
        temp -= adc_value;
        temp *= 1000;
        LCD_ShowxNum(150,130,temp,3,16,0x80);
        adc_dma_sta = 0;
        adc_dma_enable(ADC_DMA_BUF_SZIE);
    }
    LED_TOGGLE(LED0_GPIO_Pin);
    delay_ms(100);
  }
}
```

## 3. ADC常见函数（HAL库）

### 3.1 ADC 初始化与配置

#### 3.1.1 `HAL_ADC_Init()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_ADC_Init(ADC_HandleTypeDef *hadc)
```

**参数**:

- `hadc`: ADC 句柄指针

**配置结构体**:

```c
typedef struct {
 uint32_t ClockPrescaler; // 时钟分频: 
// ADC_CLOCK_SYNC_PCLK_DIV2/4/6/8
 uint32_t Resolution; // 分辨率:
 // ADC_RESOLUTION_12B/10B/8B/6B
 uint32_t DataAlign; // 数据对齐: 
// ADC_DATAALIGN_RIGHT/LEFT
 uint32_t ScanConvMode; // 扫描模式: 
// ENABLE/DISABLE
 uint32_t EOCSelection; // EOC 标志选择:
 // ADC_EOC_SINGLE_CONV
 // ADC_EOC_SEQ_CONV
 uint32_t ContinuousConvMode; // 连续转换模式:
 // ENABLE/DISABLE
 uint32_t DMAContinuousRequests; // DMA 连续请求:
 // ENABLE/DISABLE
 uint32_t NbrOfConversion; // 转换序列长度 (1-16)
 uint32_t DiscontinuousConvMode; // 间断模式: ENABLE/DISABLE
 uint32_t NbrOfDiscConversion; // 间断序列长度 (1-8)
 uint32_t ExternalTrigConv; // 外部触发源:
 // ADC_SOFTWARE_START
 // ADC_EXTERNALTRIGCONV_T1_CC1-3
 // ADC_EXTERNALTRIGCONV_T2_CC2-4
 // ADC_EXTERNALTRIGCONV_T3_CC1-4
 // ADC_EXTERNALTRIGCONV_T4_CC4
 // ADC_EXTERNALTRIGCONV_T8_CC1-4
 // ADC_EXTERNALTRIGCONV_EXT_IT11
 uint32_t ExternalTrigConvEdge; // 外部触发边沿:
 // ADC_EXTERNALTRIGCONVEDGE_NONE
 // ADC_EXTERNALTRIGCONVEDGE_RISING
 // ADC_EXTERNALTRIGCONVEDGE_FALLING
 // ADC_EXTERNALTRIGCONVEDGE_RISINGFALLING
} ADC_InitTypeDef;
```

**功能**: 初始化 ADC 基础参数  
**示例配置**:

```c
ADC_HandleTypeDef hadc1;

void ADC_Init(void) {
    __HAL_RCC_ADC1_CLK_ENABLE();
    
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.ScanConvMode = ENABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DMAContinuousRequests = ENABLE;
    hadc1.Init.NbrOfConversion = 4;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_TRGO;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }
}
```

### 3.2 ADC 通道配置

#### 3.2.1 `HAL_ADC_ConfigChannel()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_ADC_ConfigChannel(
 ADC_HandleTypeDef *hadc,
 ADC_ChannelConfTypeDef *sConfig)
```

**通道配置结构体**:

```c
typedef struct {
 uint32_t Channel; // 通道号: ADC_CHANNEL_0 到 19
 uint32_t Rank; // 转换序列中的位置 (1-16)
 uint32_t SamplingTime; // 采样时间:
 // ADC_SAMPLETIME_3CYCLES
 // ADC_SAMPLETIME_15CYCLES
 // ADC_SAMPLETIME_28CYCLES
 // ADC_SAMPLETIME_56CYCLES
 // ADC_SAMPLETIME_84CYCLES
 // ADC_SAMPLETIME_112CYCLES
 // ADC_SAMPLETIME_144CYCLES
 // ADC_SAMPLETIME_480CYCLES
 uint32_t Offset; // 偏移值 (用于偏移校准)
} ADC_ChannelConfTypeDef;
```

**示例配置** (多通道扫描):

```c
ADC_ChannelConfTypeDef sConfig = {0};

// 通道0配置 (PA0)
sConfig.Channel = ADC_CHANNEL_0;
sConfig.Rank = 1;
sConfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;
HAL_ADC_ConfigChannel(&hadc1, &sConfig);

// 通道1配置 (PA1)
sConfig.Channel = ADC_CHANNEL_1;
sConfig.Rank = 2;
sConfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;
HAL_ADC_ConfigChannel(&hadc1, &sConfig);

// 通道2配置 (PA2)
sConfig.Channel = ADC_CHANNEL_2;
sConfig.Rank = 3;
sConfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;
HAL_ADC_ConfigChannel(&hadc1, &sConfig);
```

### 3.3 ADC 校准

#### 3.3.1 校准函数

```c
// 执行ADC校准
HAL_StatusTypeDef HAL_ADCEx_Calibration_Start(
 ADC_HandleTypeDef *hadc,
 uint32_t SingleDiff);

// 获取校准因子
uint32_t HAL_ADCEx_Calibration_GetValue(
 ADC_HandleTypeDef *hadc,
 uint32_t SingleDiff);
```

**示例**:

```c
// 执行校准
if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK) {
 Error_Handler();
}

// 获取校准因子
uint32_t calib_factor = HAL_ADCEx_Calibration_GetValue(&hadc1, ADC_SINGLE_ENDED);
```

### 3.4 ADC 转换控制

#### 3.4.1 启动转换

```c
// 启动ADC转换 (阻塞模式)
HAL_StatusTypeDef HAL_ADC_Start(ADC_HandleTypeDef *hadc);

// 启动ADC转换 (中断模式)
HAL_StatusTypeDef HAL_ADC_Start_IT(ADC_HandleTypeDef *hadc);

// 启动ADC转换 (DMA模式)
HAL_StatusTypeDef HAL_ADC_Start_DMA(
 ADC_HandleTypeDef *hadc,
 uint32_t *pData,
 uint32_t Length);


```

#### 3.4.2 停止转换

```c
// 停止ADC转换
HAL_StatusTypeDef HAL_ADC_Stop(ADC_HandleTypeDef *hadc);

// 停止ADC转换 (中断模式)
HAL_StatusTypeDef HAL_ADC_Stop_IT(ADC_HandleTypeDef *hadc);

// 停止ADC转换 (DMA模式)
HAL_StatusTypeDef HAL_ADC_Stop_DMA(ADC_HandleTypeDef *hadc);
```

### 3.5 ADC 数据获取

#### 3.5.1 获取转换结果

```c
// 获取ADC转换值 (阻塞模式)
uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef *hadc);

// 检查转换是否完成
HAL_StatusTypeDef HAL_ADC_PollForConversion(
 ADC_HandleTypeDef *hadc,
 uint32_t Timeout);
```

**示例** (单次转换):

```c
HAL_ADC_Start(&hadc1); // 启动转换

// 等待转换完成
if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
 // 获取转换结果
 uint32_t adc_value = HAL_ADC_GetValue(&hadc1);
 float voltage = (adc_value * 3.3f) / 4095.0f; // 转换为电压
}
```

### 3.6 ADC 中断处理

#### 3.6.1 中断服务函数

```c
// ADC全局中断服务函数
void ADC_IRQHandler(void) {
 HAL_ADC_IRQHandler(&hadc1);
}
```

#### 3.6.2 回调函数 (用户实现)

```c
// 转换完成回调
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
 if (hadc == &hadc1) {
 // 处理转换完成事件
 }
}

// 转换半完成回调 (DMA双缓冲)
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) {
 if (hadc == &hadc1) {
 // 处理半转换完成事件
 }
}

// 错误回调
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc) {
 // 处理ADC错误
 uint32_t error = HAL_ADC_GetError(hadc);
 if (error & HAL_ADC_ERROR_OVR) {
 // 溢出错误处理
 }
}
```

### 3.7 ADC 多模式配置 (双ADC)

#### 3.7.1 `HAL_ADCEx_MultiModeConfigChannel()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_ADCEx_MultiModeConfigChannel(
 ADC_HandleTypeDef *hadc,
 ADC_MultiModeTypeDef *multimode)
```

**多模式配置结构体**:

```c
typedef struct {
 uint32_t Mode; // 多ADC模式:
 // ADC_MODE_INDEPENDENT
 // ADC_DUALMODE_REGSIMULT
 // ADC_DUALMODE_INTERL
 // ADC_DUALMODE_INJECSIMULT
 // ADC_DUALMODE_ALTERTRIG
 // ADC_TRIPLEMODE_REGSIMULT
 // ADC_TRIPLEMODE_INTERL
 // ADC_TRIPLEMODE_INJECSIMULT
 // ADC_TRIPLEMODE_ALTERTRIG
 uint32_t DMAAccessMode; // DMA访问模式:
 // ADC_DMAACCESSMODE_DISABLED
 // ADC_DMAACCESSMODE_1
 // ADC_DMAACCESSMODE_2
 // ADC_DMAACCESSMODE_3
 uint32_t TwoSamplingDelay; // 采样延迟 (0-31个时钟周期)
} ADC_MultiModeTypeDef;
```

**示例** (双ADC同步规则模式):

```c
ADC_MultiModeTypeDef multimode;

multimode.Mode = ADC_DUALMODE_REGSIMULT;
multimode.DMAAccessMode = ADC_DMAACCESSMODE_1;
multimode.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_5CYCLES;

HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode);
```

### 3.8 ADC 注入通道

#### 3.8.1 注入通道配置

```c
// 配置注入通道
HAL_StatusTypeDef HAL_ADCEx_InjectedConfigChannel(
 ADC_HandleTypeDef *hadc,
 ADC_InjectionConfTypeDef *sConfigInjected);

// 启动注入转换
HAL_StatusTypeDef HAL_ADCEx_InjectedStart(ADC_HandleTypeDef *hadc);
```

**注入配置结构体**:

```c
typedef struct {
 uint32_t InjectedChannel; // 注入通道号
 uint32_t InjectedRank; // 注入序列位置 (1-4)
 uint32_t InjectedSamplingTime; // 注入采样时间
 uint32_t InjectedOffset; // 注入偏移值
 uint32_t InjectedNbrOfConversion; // 注入转换数量
 FunctionalState AutoInjectedConv; // 自动注入: ENABLE/DISABLE
 FunctionalState InjectedDiscontinuousConvMode; // 间断模式
} ADC_InjectionConfTypeDef;
```

### 3.9 ADC 看门狗

#### 3.9.1 模拟看门狗配置

```c
HAL_StatusTypeDef HAL_ADC_AnalogWDGConfig(
 ADC_HandleTypeDef *hadc,
 ADC_AnalogWDGConfTypeDef *AnalogWDGConfig);
```

**看门狗配置结构体**:

```c
typedef struct {
 uint32_t WatchdogMode; // 看门狗模式:
 // ADC_ANALOGWATCHDOG_NONE
 // ADC_ANALOGWATCHDOG_SINGLE_REG
 // ADC_ANALOGWATCHDOG_ALL_REG
 // ADC_ANALOGWATCHDOG_ALL_INJEC
 // ADC_ANALOGWATCHDOG_SINGLE_INJEC
 uint32_t Channel; // 监控通道
 uint32_t ITMode; // 中断模式: ENABLE/DISABLE
 uint32_t HighThreshold; // 高阈值 (12位)
 uint32_t LowThreshold; // 低阈值 (12位)
} ADC_AnalogWDGConfTypeDef;
```

**示例**:

```c
ADC_AnalogWDGConfTypeDef AnalogWDGConfig;

AnalogWDGConfig.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REG;
AnalogWDGConfig.Channel = ADC_CHANNEL_5;
AnalogWDGConfig.ITMode = ENABLE;
AnalogWDGConfig.HighThreshold = 3000; // 约2.4V (3.3V参考)
AnalogWDGConfig.LowThreshold = 1000; // 约0.8V

HAL_ADC_AnalogWDGConfig(&hadc1, &AnalogWDGConfig);
```

### 3.10 温度传感器与内部参考电压

#### 3.10.1 内部通道配置

```c
// 启用温度传感器
void HAL_ADCEx_EnableVREFINT(void);
void HAL_ADCEx_EnableVREFINTTemp(void);

// 配置内部通道
ADC_ChannelConfTypeDef sConfig = {0};
sConfig.Channel = ADC_CHANNEL_TEMPSENSOR; // 温度传感器
// 或 ADC_CHANNEL_VREFINT // 内部参考电压
sConfig.Rank = 1;
sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
HAL_ADC_ConfigChannel(&hadc1, &sConfig);
```

#### 3.10.2 温度计算

```c
// 获取温度值
float Get_Temperature(uint32_t adc_value) {
    // 使用工厂校准值
    uint32_t *ts_cal1 = (uint32_t *)0x1FFF7A2C; // 30°C校准值地址
    uint32_t *ts_cal2 = (uint32_t *)0x1FFF7A2E; // 110°C校准值地址
    
    float temp = ((float)adc_value - *ts_cal1) * (110.0 - 30.0);
    temp = temp / (*ts_cal2 - *ts_cal1) + 30.0;
    
    return temp;
}
```

### 3.11 ADC DMA 双缓冲模式

#### 3.11.1 DMA 双缓冲配置

```c
#define ADC_BUF_SIZE 256
uint16_t adc_buffer1[ADC_BUF_SIZE];
uint16_t adc_buffer2[ADC_BUF_SIZE];

// 启动ADC DMA双缓冲
HAL_ADC_Start_DMA(&hadc1, 
(uint32_t*)adc_buffer1, 
(uint32_t*)adc_buffer2, 
ADC_BUF_SIZE);
```

#### 3.11.2 双缓冲回调处理

```c
// 缓冲区1转换完成
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
 if (hadc == &hadc1) {
 // 处理adc_buffer1数据
 Process_ADC_Data(adc_buffer1, ADC_BUF_SIZE);
 }
}

// 缓冲区2转换完成
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) {
 if (hadc == &hadc1) {
 // 处理adc_buffer2数据
 Process_ADC_Data(adc_buffer2, ADC_BUF_SIZE);
 }
}
```

### 3.12 ADC 其他配置

#### 3.12.1 采样时间选择

```c
// 根据输入阻抗选择采样时间
#define INPUT_IMPEDANCE 10e3 // 10KΩ
void Set_Sampling_Time(ADC_HandleTypeDef *hadc, uint32_t channel) {
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = channel;
    sConfig.Rank = 1;
    
    // 采样时间计算: T_sampling = (SamplingTime + 12.5) / ADC_CLK
    if (INPUT_IMPEDANCE > 50e3) {
        sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    } else if (INPUT_IMPEDANCE > 10e3) {
        sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;
    } else {
        sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
    }
    
    HAL_ADC_ConfigChannel(hadc, &sConfig);
}
```

#### 3.12.2 软件过采样

```c
// 通过软件过采样提高分辨率
uint32_t Oversample_ADC(ADC_HandleTypeDef *hadc, uint8_t oversample_bits) {
    uint32_t sum = 0;
    uint16_t samples = 1 << (2 * oversample_bits); // 4^N 次采样
    
    for (int i = 0; i < samples; i++) {
        HAL_ADC_Start(hadc);
        HAL_ADC_PollForConversion(hadc, 10);
        sum += HAL_ADC_GetValue(hadc);
    }
    
    // 右移N位得到(12+N)位结果
    return sum >> oversample_bits;
}
```

#### 3.12.3 自动校准偏移

```c
// 自动校准ADC偏移
void Auto_Calibrate_Offset(ADC_HandleTypeDef *hadc) {
    // 短路ADC输入到地
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_VREFINT; // 使用内部通道
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    HAL_ADC_ConfigChannel(hadc, &sConfig);
    
    // 采集多个样本
    uint32_t sum = 0;
    for (int i = 0; i < 16; i++) {
        HAL_ADC_Start(hadc);
        HAL_ADC_PollForConversion(hadc, 10);
        sum += HAL_ADC_GetValue(hadc);
    }
    uint32_t avg = sum / 16;
    
    // 设置偏移校准
    hadc->Instance->OFR1 = (avg << ADC_OFR1_OFFSET1_Pos) | ADC_OFR1_OFFSET1_EN;
}
```

---
