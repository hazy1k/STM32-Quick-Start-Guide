# 第九章 RCC-MCO输出

## 1. 使用HSE

    一般情况下，我们都是使用HSE，然后HSE经过PLL倍频之后作为系统时钟。通常的配置是：HSE=8M，PLL的倍频因子为：9， 系统时钟就设置成:SYSCLK = 8M * 9 = 72M。使用HSE，系统时钟SYSCLK最高是128M。我们使用的库函数就是这么干的， 当程序来到main函数之前，启动文件：statup_stm32f10x_hd.s已经调用SystemInit()函数把系统时钟初始化成72MHZ， SystemInit()在库文件：system_stm32f10x.c中定义。如果我们想把系统时钟设置低一点或者超频的话，可以修改底层的库文件， 但是为了维持库的完整性，我们可以根据时钟树的流程自行写一个。

    更多内容参考：[STM32入坑（六）使用HSE配置系统时钟_stm32使用hse时钟-CSDN博客](https://blog.csdn.net/m0_46195580/article/details/111402501)

## 2. 使用HSI

    当HSE故障的时候，如果PLL的时钟来源是HSE，那么当HSE故障的时候，不仅HSE不能使用，连PLL也会被关闭，这个时候系统会自动切换HSI作为系统时钟， 此时SYSCLK=HSI=8M，如果没有开启CSS和CSS中断的话，那么整个系统就只能在低速率运行，这是系统跟瘫痪没什么两样。如果开启了CSS功能的话， 那么可以当HSE故障时，在CSS中断里面采取补救措施，使用HSI，并把系统时钟设置为更高的频率，最高是64M，64M的频率足够一般的外设使用， 如：ADC、SPI、I2C等。但是这里就又有一个问题了，原来SYSCLK=72M，现在因为故障改成64M，那么那些外设的时钟肯定被改变了， 那么外设工作就会被打乱，那我们是不是在设置HSI时钟的时候，也重新调整外设总线的分频因子，即AHB，APB2和APB1的分频因子， 使外设的时钟达到跟HSE没有故障之前一样。但是这个也不是最保障的办法，毕竟不能一直使用HSI，所以当HSE故障时还是要采取报警措施。

    还有一种情况是，有些用户不想用HSE，想用HSI，但是又不知道怎么用HSI来设置系统时钟，因为调用库函数都是使用HSE， 下面我们给出个使用HSI配置系统时钟例子，起个抛砖引玉的作用。

    更多内容参考：[STM32系统时钟的配置方法——内部高速时钟HSI作为系统时钟源_stm32 hsi-CSDN博客](https://blog.csdn.net/qq_33066921/article/details/137297817)

## 3. 硬件设计

- RCC

       这个是内部资源，不用管

## 4. 软件设计

### 4.1 编程大纲

1. 开启HSE/HSI ，并等待 HSE/HSI 稳定

2. 设置 AHB、APB2、APB1的预分频因子

3. 设置PLL的时钟来源，和PLL的倍频因子，设置各种频率主要就是在这里设置

4. 开启PLL，并等待PLL稳定

5. 把PLLCK切换为系统时钟SYSCLK

6. 读取时钟切换状态位，确保PLLCLK被选为系统时钟

### 4.2 代码分析

#### 4.2.1 使用HSE/HSI配置系统时钟

```c
#include "clkconfig.h"
#include "stm32f10x_rcc.h"

// 前置知识
/*
    1.AHB总线时钟：HCLK
    2.APB1总线时钟：PCLK1
    3.APB2总线时钟：PCLK2
    PCLK2 = HCLK = SYSCLK
    PCLK1 = HCLK/2,最高只能是36M
*/

// 使用HSE作为系统时钟源
/*
    1.开启HSE，等待HSE稳定
    2.设置AHB、APB1、APB2的预分频系数
    3.设置PLL的时钟来源和PLL的倍频因子
    4.开启PLL，等待PLL稳定
    5.把PLLCK切换到系统时钟SYSCLK,设置SYSCLK的时钟源为PLLCK
*/

void HSE_SetSYSCLK(uint32_t plluml) // plluml: PLL的倍频因子
{
    __IO uint32_t StartUpCounter = 0;   // 等待HSE稳定计数器
    __IO uint32_t HSEStartUpStatus = 0; // HSE启动成功标志位
    RCC_DeInit(); // 复位RCC寄存器，复位HSE、PLL
    // 1.开启HSE，等待HSE稳定
    RCC_HSEConfig(RCC_HSE_ON);
    HSEStartUpStatus = RCC_WaitForHSEStartUp(); 
    if(HSEStartUpStatus == SUCCESS)
    {
        // 2.启动HSE成功后，设置AHB、APB1、APB2的预分频系数
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); // 使能FLASH预读
        FLASH_SetLatency(FLASH_Latency_2); // SYSCLK周期与闪存访问时间的比例设置，设置为2：48< SYSCLK <= 72M
        // 下面正式开始设置分频系数
        RCC_HCLKConfig(RCC_SYSCLK_Div1);// AHB预分频因子设置为1，即HCLK = SYSCLK
        RCC_PCLK1Config(RCC_HCLK_Div2); // APB1预分频因子设置为2，即PCLK1 = HCLK/2
        RCC_PCLK2Config(RCC_HCLK_Div1); // APB2预分频因子设置为1，即PCLK2 = HCLK
        // 3.设置PLL的时钟来源和PLL的倍频因子
        // PLLCLK计算公式：PLLCLK = 8MHz * PLLMUL
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, plluml);
        // 4.开启PLL，等待PLL稳定
        RCC_PLLCmd(ENABLE);
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {

        }
        // 5.把PLLCK切换到系统时钟SYSCLK
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        // 6.读取时钟切换状态以确保PLLCLK被选中为系统时钟（SYSCLK）
        while(RCC_GetSYSCLKSource() != 0x08)
        {

        }
    }
    else
    {
        // 启动HSE失败，可能是HSE不足，或者外部晶振不稳定
        // 如果HSE启动时钟，单片机会自动把HSE切换到HSI
        // HSI是单片机内部的高速时钟源，HSI的频率为8MHz
        while(1)
        {

        }
    }
}

// 使用HSI作为系统时钟源
// 和HSE的步骤一样，我们就不多做说明了
void HSI_SetSYSCLK(uint32_t plluml)
{
    __IO uint32_t HSIStartStatus = 0; // HSI启动成功标志位
    RCC_DeInit(); 
    RCC_HSICmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
    {

    }
    HSIStartStatus = RCC->CR & RCC_CR_HSIRDY; // 读取HSI的就绪状态
    if(HSIStartStatus == RCC_CR_HSIRDY)
    {
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
        FLASH_SetLatency(FLASH_Latency_2);
        RCC_HCLKConfig(RCC_SYSCLK_Div1);
        RCC_PCLK1Config(RCC_HCLK_Div2);
        RCC_PCLK2Config(RCC_HCLK_Div1);
        RCC_PLLConfig(RCC_PLLSource_HSI_Div2, plluml);
        RCC_PLLCmd(ENABLE);
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {

        }
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        while(RCC_GetSYSCLKSource() != 0x08)
        {

        }
    }
    else
    {
        while(1)
        {

        }
    }
}
```

按照我们的时钟配置流程来编写代码，分为HSE和HSI两个版本

#### 4.2.2 MCO配置初始化

```c
#include "mcooutput.h"
#include "stm32f10x_rcc.h"

// MCO GPIO 初始化
void MCO_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
```

#### 4.2.3 主函数

```c
#include "stm32f10x.h"
#include "clkconfig.h"
#include "mcooutput.h"

int main(void)
{
    // 使用HSE时：SYSCLK = 8M * RCC_PLLMul_x(x=2~16)，最高128M
    // 使用HSI时：SYSCLK = 4M * RCC_PLLMul_x(x=2~16)，最高64M
    HSI_SetSYSCLK(RCC_PLLMul_16); // 这里我们选择HSI作为系统时钟源，SYSCLK = 4M * 16 = 64M
    MCO_GPIO_Init();
    // 设置MCO引脚输出的时钟频率
    // MCO引脚输出的频率可以是HSE、HSI、SYSCLK、PLLCLK等，这里我们选择SYSCLK作为输出源，也就是HSI啦
    RCC_MCOConfig(RCC_MCO_SYSCLK);
    while(1)
    {
    }    
}
```

    在主函数中，可以调用HSE_SetSYSCLK()或者HSI_SetSYSCLK()这两个函数把系统时钟设置成各种常用的时钟，然后通过MCO引脚监控， 或者通过LED闪烁的快慢体验不同的系统时钟对同一个软件延时函数的影响。

## 5. 小结

本章主要就是一个配置问题，可以参考时钟树来理解，比如我们以配置HSE为例：

1. 开启HSE ，并等待 HSE 稳定

    如何开启？答：使用库函数就行了呀

```c
// 把RCC外设初始化成复位状态
RCC_DeInit(); // 库函数 RCC_DeInit()
//使能HSE，开启外部晶振，野火开发板用的是8M
RCC_HSEConfig(RCC_HSE_ON); // 库函数 RCC_HSEConfig()
// 等待 HSE 启动稳定
HSEStartUpStatus = RCC_WaitForHSEStartUp(); // 库函数 RCC_WaitForHSEStartUp()
```

照例我们还是解释一下新出现的库函数：

1.  **`RCC_DeInit()`**
- **功能**: `RCC_DeInit()` 函数用于将 RCC（时钟控制器）外设的寄存器恢复到其默认复位状态。它会将 RCC 外设的所有设置重置为默认值，从而确保系统的时钟配置处于已知的初始状态。
- **作用**: 在对 RCC 进行任何新的配置之前，调用此函数可以避免之前设置可能对新的配置造成干扰，确保初始化过程的一致性和可靠性。
2.  **`RCC_HSEConfig(RCC_HSE_ON)`**
- **功能**: `RCC_HSEConfig()` 函数用于配置外部高速晶振（HSE）。`RCC_HSE_ON` 参数表示使能 HSE，即启动外部晶振。
- **参数**:
  - `RCC_HSE_ON`: 启用 HSE，即外部晶振。
  - `RCC_HSE_OFF`: 禁用 HSE，即关闭外部晶振。
  - `RCC_HSE_BYPASS`: 使用外部晶振的旁路模式（如果有外部时钟源直接输入到 HSE 引脚）。
- **作用**: 在 STM32 微控制器中，外部晶振（HSE）提供了系统时钟源的一个选项，特别是在需要更高精度时钟源时使用。通过调用这个函数并传入 `RCC_HSE_ON` 参数，可以启动外部晶振，供系统时钟使用。
3.  **`RCC_WaitForHSEStartUp()`**
- **功能**: `RCC_WaitForHSEStartUp()` 函数用于等待外部高速晶振（HSE）稳定并准备好。此函数会检查 HSE 是否已经成功启动并稳定，返回启动状态。
- **返回值**:
  - `SUCCESS`: 表示 HSE 已经成功启动并稳定。
  - `ERROR`: 表示 HSE 启动失败或没有稳定。
- **作用**: 外部晶振需要一定的时间才能稳定输出正确的时钟信号。通过调用此函数，可以确保在继续进行其他依赖 HSE 的配置之前，HSE 已经稳定工作。这对于确保系统的时钟源可靠性非常重要。

---

2. 设置 AHB、APB2、APB1的预分频因子

    参考时钟树设置预分频因子：

```c
// AHB预分频因子设置为1分频，HCLK = SYSCLK 
RCC_HCLKConfig(RCC_SYSCLK_Div1); 
// APB2预分频因子设置为1分频，PCLK2 = HCLK
RCC_PCLK2Config(RCC_HCLK_Div1); 
// APB1预分频因子设置为1分频，PCLK1 = HCLK/2 
RCC_PCLK1Config(RCC_HCLK_Div2);
```

1.  **`RCC_HCLKConfig(RCC_SYSCLK_Div1)`**
- **功能**: 设置 AHB 总线（HCLK）的时钟预分频因子。
- **参数**:
  - `RCC_SYSCLK_Div1`: 设置 HCLK 为 SYSCLK 的 1 倍，即没有分频。
  - 其他可能的参数如 `RCC_SYSCLK_Div2`, `RCC_SYSCLK_Div4`, 等等，用于不同的分频设置。
- **作用**: 这个配置将 AHB 总线的时钟频率设置为系统时钟（SYSCLK）的频率。在这个例子中，`RCC_SYSCLK_Div1` 使得 HCLK 与 SYSCLK 相同，没有分频。
2.  **`RCC_PCLK2Config(RCC_HCLK_Div1)`**
- **功能**: 设置 APB2 总线（PCLK2）的时钟预分频因子。
- **参数**:
  - `RCC_HCLK_Div1`: 设置 PCLK2 为 HCLK 的 1 倍，即没有分频。
  - 其他可能的参数如 `RCC_HCLK_Div2`, `RCC_HCLK_Div4`, 等等，用于不同的分频设置。
- **作用**: 这个配置将 APB2 总线的时钟频率设置为 AHB 总线（HCLK）的频率。在这个例子中，`RCC_HCLK_Div1` 使得 PCLK2 与 HCLK 相同，没有分频。
3.  **`RCC_PCLK1Config(RCC_HCLK_Div2)`**
- **功能**: 设置 APB1 总线（PCLK1）的时钟预分频因子。
- **参数**:
  - `RCC_HCLK_Div2`: 设置 PCLK1 为 HCLK 的 1/2，即分频因子为 2。
  - 其他可能的参数如 `RCC_HCLK_Div1`, `RCC_HCLK_Div4`, 等等，用于不同的分频设置。
- **作用**: 这个配置将 APB1 总线的时钟频率设置为 AHB 总线（HCLK）的 1/2，即有一个分频因子。这是因为 STM32 微控制器中，APB1 外设的最大时钟频率通常受到限制，可能低于 AHB 总线的频率。

---

3. 设置PLL的时钟来源，和PLL的倍频因子，设置各种频率主要就是在这里设置

```c
// 设置PLL时钟来源为HSE，设置PLL倍频因子
// PLLCLK = 8MHz * pllmul
RCC_PLLConfig(RCC_PLLSource_HSE_Div1, pllmul);
```

- 功能：这段代码配置 PLL 的时钟源以及倍频因子，从而设置 PLL 输出时钟的频率。PLL 是用于生成高频时钟信号的一个重要模块，通常用于系统时钟（SYSCLK）的源。

- 参数：
1. **`RCC_PLLSource_HSE_Div1`**：
   
   - 这指定了 PLL 时钟源为外部高速时钟（HSE，High-Speed External）。HSE 是一个外部晶振或晶体振荡器的输出频率。
   - `Div1` 表示将 HSE 时钟源的频率不做分频（即 HSE 输入直接作为 PLL 的输入时钟源）。

2. **`pllmul`**：
   
   - 这是 PLL 的倍频因子，它决定了 PLL 输出时钟的频率。通常，这个倍频因子可以在一个预定义的范围内设置，例如 2 到 16。
   - PLL 输出频率（PLLCLK）由公式计算：`PLLCLK = HSE * pllmul`。
- 作用：
1. **设置 PLL 时钟源**：
   通过将 PLL 时钟源设置为 HSE，你选择了一个外部的高频晶振作为 PLL 的输入。这通常提供了一个稳定且高精度的时钟源。

2. **配置 PLL 倍频因子**：
   倍频因子 `pllmul` 决定了 PLL 输出频率相对于 HSE 输入频率的倍数。这使得你可以根据需要生成所需的高频时钟，以满足系统的需求。

---

4. 开启PLL，并等待PLL稳定

```c
// 开启PLL 
RCC_PLLCmd(ENABLE); // 使用库函数 RCC_PLLCmd()
// 等待 PLL稳定
while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
{

}
```

1.  `RCC_PLLCmd()`

函数原型如下：

```c
void RCC_PLLCmd(FunctionalState NewState);
```

- **功能**：启用或禁用 PLL（Phase-Locked Loop）。
- **参数**：
  - **`NewState`**：设置为 `ENABLE` 或 `DISABLE`。 `ENABLE` 启用 PLL，`DISABLE` 禁用 PLL。
- **描述**：这个函数通过设置 RCC（Reset and Clock Control）寄存器中的 PLL 使能位来启动或停止 PLL 时钟源。
2.  `RCC_GetFlagStatus()`

函数原型如下：

```c
FlagStatus RCC_GetFlagStatus(uint8_t RCC_FLAG);
```

- **功能**：获取指定 RCC 标志的状态。
- **参数**：
  - **`RCC_FLAG`**：需要检查的标志类型，比如 `RCC_FLAG_PLLRDY` 表示 PLL 是否准备好了。
- **返回值**：
  - 返回 `SET` 或 `RESET`，指示指定的标志是否已经设置。

---

5. 把PLLCK切换为系统时钟SYSCLK

```c
// 当PLL稳定之后，把PLL时钟切换为系统时钟SYSCLK
RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); // PLLCLK经过我们上面设置了，现在只需要把它作为系统时钟
```

`RCC_SYSCLKConfig()`

函数原型如下：

```c
void RCC_SYSCLKConfig(uint32_t RCC_SYSCLKSource);
```

- **功能**：设置系统时钟源（SYSCLK）。
- **参数**：
  - **`RCC_SYSCLKSource`**：指定系统时钟源的常量。可以是以下几个值之一：
    - `RCC_SYSCLKSource_HSI`: 高速内部振荡器 (HSI) 时钟。
    - `RCC_SYSCLKSource_HSE`: 高速外部振荡器 (HSE) 时钟。
    - `RCC_SYSCLKSource_PLLCLK`: PLL 时钟。

---

6. 读取时钟切换状态位，确保PLLCLK被选为系统时钟

```c
// 读取时钟切换状态位，确保PLLCLK被选为系统时钟
// 库函数 RCC_GetSYSCLKSource() 用于读取系统时钟源
// 参数代表系统时钟源，0x08代表PLLCLK
while (RCC_GetSYSCLKSource() != 0x08)
{
      // 等待时钟切换完成
}
    // 系统时钟设置完成
```

`RCC_GetSYSCLKSource()`

函数原型如下：

```c
uint8_t RCC_GetSYSCLKSource(void);
```

- **功能**：获取当前系统时钟源。
- **返回值**：
  - `0x00`: 表示系统时钟源为 HSI (高速内部振荡器)。
  - `0x04`: 表示系统时钟源为 HSE (高速外部振荡器)。
  - `0x08`: 表示系统时钟源为 PLL (相位锁定环)。
- **描述**：这个函数读取 RCC 寄存器中的 SYSCLK 配置位，返回当前系统时钟的源。

---

2024.8.20 第一次修订，后期不再维护

2024.12.18 修补内容，优化代码
