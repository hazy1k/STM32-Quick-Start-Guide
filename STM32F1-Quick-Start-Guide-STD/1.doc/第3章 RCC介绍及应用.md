# 第三章 RCC时钟部分介绍及应用

## 1. RCC框图剖析-时钟部分

时钟树单纯讲理论的话会比较枯燥，如果选取一条主线，并辅以代码，先主后次讲解的话会很容易，而且记忆还更深刻。我们这里选取库函数时钟系统时钟函数： **SetSysClockTo72();** 以这个函数的编写流程来讲解时钟树，这个函数也是我们用库的时候默认的系统时钟设置函数。该函数的功能是利用 HSE 把时钟设置为： PCLK2 = HCLK = SYSCLK = 72M， PCLK1=HCLK/2 = 36M。下面我们就以这个代码的流程为主线，来分析时钟树，对应的是图中的黄色部分，代码流程在时钟树中以数字的大小顺序标识。

![屏幕截图 2025-05-28 125332.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/05/28-12-53-41-屏幕截图%202025-05-28%20125332.png)

## 2. 系统时钟

### 2.1 HSE高速外部时钟信号

HSE 是高速的外部时钟信号，可以由有源晶振或者无源晶振提供，频率从 4-16MHZ 不等。当使用有源晶振时，时钟从 OSC_IN 引脚进入， OSC_OUT 引脚悬空，当选用无源晶振时，时钟从OSC_IN 和 OSC_OUT 进入，并且要配谐振电容。

HSE 最常使用的就是 8M 的无源晶振。当确定 PLL 时钟来源的时候， HSE 可以不分频或者 2 分频，这个由时钟配置寄存器 CFGR 的位 17： PLLXTPRE 设置，我们设置为 HSE 不分频。

#### 2.1.1 PLL时钟源

PLL 时钟来源可以有两个，一个来自 HSE，另外一个是 HSI/2，具体用哪个由时钟配置寄存器 CFGR 的位 16： PLLSRC 设置。 HSI 是内部高速的时钟信号，频率为 8M，根据温度和环境的情况频率会有漂移，一般不作为 PLL 的时钟来源。这里我们选 HSE作为 PLL 的时钟来源。

#### 2.1.2 PLL时钟PLLCLK

通过设置 PLL 的倍频因子，可以对 PLL 的时钟来源进行倍频，倍频因子可以是:[2,3,4,5,6,7,8,9,10,11,12,13,14,15,16]，具体设置成多少，由时钟配置寄存器 CFGR的位 21-18： PLLMUL[3:0] 设置。我们这里设置为 9 倍频，因为上一步我们设置 PLL的时钟来源为 HSE=8M，所以经过 PLL 倍频之后的 PLL 时钟： PLLCLK = 8M *9 = 72M。 72M 是 ST 官方推荐的稳定运行时钟，如果你想超频的话，增大倍频因子即可，最高为 128M。我们这里设置 PLL 时钟： PLLCLK = 8M *9 = 72M。

#### 2.1.3 系统时钟SYSCLK

系统时钟来源可以是： HSI、 PLLCLK、 HSE，具体的时钟配置寄存器 CFGR 的位 1-0： SW[1:0] 设置。我们这里设置系统时钟： SYSCLK = PLLCLK = 72M

#### 2.1.4 AHB总线时钟HCLK

系统时钟 SYSCLK 经过 AHB 预分频器分频之后得到时钟叫 APB 总线时钟，即 HCLK，分频因子可以是:[1,2,4， 8， 16， 64， 128， 256， 512]，具体的由时钟配置寄存器 CFGR 的位7-4 ： HPRE[3:0] 设置。片上大部分外设的时钟都是经过 HCLK 分频得到，至于 AHB 总线上的外设的时钟设置为多少，得等到我们使用该外设的时候才设置，我们这里只需粗线条的设置好 APB 的时钟即可。我们这里设置为 1 分频，即 HCLK=SYSCLK=72M。

#### 2.1.5 APB2总线时钟PLCK2

APB2 总线时钟 PCLK2 由 HCLK 经过高速 APB2 预分频器得到，分频因子可以是:[1,2,4， 8， 16]，具体由时钟配置寄存器 CFGR 的位 13-11： PPRE2[2:0] 决定。 PCLK2属于高速的总线时钟，片上高速的外设就挂载到这条总线上，比如全部的 GPIO、 USART1、 SPI1 等。至于 APB2 总线上的外设的时钟设置为多少，得等到我们使用该外设的时候才设置，我们这里只需粗线条的设置好 APB2 的时钟即可。我们这里设置为1 分频，即 PCLK2 = HCLK = 72M。

#### 2.1.6 APB1总线时钟PCLK1

APB1 总线时钟 PCLK1 由 HCLK 经过低速 APB 预分频器得到，分频因子可以是:[1,2,4， 8， 16]，具体的由时钟配置寄存器 CFGR 的位 10-8： PRRE1[2:0] 决定。 PCLK1 属于低速的总线时钟，最高为 36M，片上低速的外设就挂载到这条总线上，比如 USART2/3/4/5、SPI2/3， I2C1/2 等。至于 APB1 总线上的外设的时钟设置为多少，得等到我们使用该外设的时候才设置，我们这里只需粗线条的设置好 APB1 的时钟即可。我们这里设置为 2 分频，即 PCLK1 = HCLK/2 = 36M。

### 2.2 其他时钟

#### 2.2.1 USB时钟

USB 时钟是由 PLLCLK 经过 USB 预分频器得到，分频因子可以是： [1,1.5]，具体的由时钟配置寄存器 CFGR 的位 22： USBPRE 配置。 USB 的时钟最高是 48M，根据分频因子反推过来算， PLLCLK 只能是 48M 或者是 72M。一般我们设置 PLLCLK=72M， USBCLK=48M。 USB 对时钟要求比较高，所以 PLLCLK 只能是由 HSE 倍频得到，不能使用 HSI 倍频。

#### 2.2.2 Cortex系统时钟

Cortex 系统时钟由 HCLK 8 分频得到，等于 9M， Cortex 系统时钟用来驱动内核的系统定时器 SysTick， SysTick 一般用于操作系统的时钟节拍，也可以用做普通的定时。

#### 2.2.3 ADC时钟

ADC 时钟由 PCLK2 经过 ADC 预分频器得到，分频因子可以是 [2,4,6,8]，具体的由时钟配置寄存器 CFGR 的位 15-14： ADCPRE[1:0] 决定。很奇怪的是怎么没有 1 分频。ADC 时钟最高只能是 14M，如果采样周期设置成最短的 1.5 个周期的话， ADC 的转换时间可以达到最短的 1us。如果真要达到最短的转换时间 1us 的话，那 ADC 的时钟就得是 14M，反推 PCLK2 的时钟只能是： 28M、 56M、 84M、 112M，鉴于 PCLK2 最高是 72M，所以只能取 28M 和 56M。

#### 2.2.4 RTC时钟、独立看门狗时钟

RTC 时钟可由 HSE/128 分频得到，也可由低速外部时钟信号 LSE 提供，频率为32.768KHZ，也可由低速内部时钟信号 LSI 提供，具体选用哪个时钟由备份域控制寄存器 BDCR 的位 9-8： RTCSEL[1:0] 配置。独立看门狗的时钟由 LSI 提供，且只能是由 LSI 提供， LSI 是低速的内部时钟信号，频率为 30~60KHZ 直接不等，一般取40KHZ。

#### 2.2.5 MCO时钟输出

MCO 是 microcontroller clock output 的缩写，是微控制器时钟输出引脚，在 STM32 F1系列中由 PA8 复用所得，主要作用是可以对外提供时钟，相当于一个有源晶振。 MCO的时钟来源可以是： PLLCLK/2、 HSI、 HSE、 SYSCLK，具体选哪个由时钟配置寄存器CFGR 的位 26-24： MCO[2:0] 决定。除了对外提供时钟这个作用之外，我们还可以通过示波器监控 MCO 引脚的时钟输出来验证我们的系统时钟配置是否正确。

## 3. RCC使用示例

一般情况下，我们都是使用 HSE，然后 HSE 经过 PLL 倍频之后作为系统时钟。通常的配置是： HSE=8M， PLL 的倍频因子为： 9，系统时钟就设置成:SYSCLK = 8M * 9 = 72M。使用 HSE，系统时钟 SYSCLK 最高是 128M。我们使用的库函数就是这么干的，当程序来到 main 函数之前，启动文件： statup_stm32f10x_hd.s 已经调用 SystemInit() 函数把系统时钟初始化成 72MHZ， SystemInit()在库文件： system_stm32f10x.c 中定义。如果我们想把系统时钟设置低一点或者超频的话，可以修改底层的库文件，但是为了维持库的完整性，我们可以根据时钟树的流程自行写一个。

当 HSE 故障的时候，如果 PLL 的时钟来源是 HSE，那么当 HSE 故障的时候，不仅 HSE 不能使用，连 PLL 也会被关闭，这个时候系统会自动切换 HSI 作为系统时钟，此时 SYSCLK=HSI=8M，如果没有开启 CSS 和 CSS 中断的话，那么整个系统就只能在低速率运行，这是系统跟瘫痪没什么两样。如果开启了 CSS 功能的话，那么可以当 HSE 故障时，在 CSS 中断里面采取补救措施，使用 HSI，并把系统时钟设置为更高的频率，最高是 64M， 64M 的频率足够一般的外设使用，如： ADC、 SPI、 I2C 等。但是这里就又有一个问题了，原来 SYSCLK=72M，现在因为故障改成 64M，那么那些外设的时钟肯定被改变了，那么外设工作就会被打乱，那我们是不是在设置 HSI 时钟的时候，也重新调整外设总线的分频因子，即 AHB， APB2 和 APB1 的分频因子，使外设的时钟达到跟 HSE 没有故障之前一样。但是这个也不是最保障的办法，毕竟不能一直使用 HSI，所以当HSE 故障时还是要采取报警措施。

### 3.1 配置HSE/HSI为系统时钟

```c
#include "rcc.h"

/* 设置系统时钟步骤
    1.开启HSE/HSI，等待稳定
    2.设置AHB、APB2、APB1时钟分频
    3.设置PLL时钟来源和PLL时钟分配
    4.开启PLL，等待稳定
    5.把PLLCLK输出作为系统时钟 
    6.读取时钟切换状态位，确保时钟切换成功
*/

void HSE_Clock_Init(uint32_t pllmul)
{
    __IO uint32_t StartUpCounter = 0, HSEStartUpStatus = 0;

    RCC_DeInit(); // 复位RCC
    RCC_HSEConfig(RCC_HSE_ON); // 开启HSE
    HSEStartUpStatus = RCC_WaitForHSEStartUp(); // 等待HSE稳定
    if(HSEStartUpStatus == SUCCESS)
    {
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
        FLASH_SetLatency(FLASH_Latency_2);
        RCC_HCLKConfig(RCC_SYSCLK_Div1); // AHB分频为1分频，HCLK = SYSCLK 
        RCC_PCLK2Config(RCC_HCLK_Div1); // APB2分频为1分频，PCLK2 = HCLK 
        RCC_PCLK1Config(RCC_HCLK_Div2); // APB1分频为2分频，PCLK1 = HCLK/2
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, pllmul); // 设置PLL时钟来源为HSE，设置PLL倍频
        RCC_PLLCmd(ENABLE); // 启动PLL
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); // 等待PLL启动完成
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); // 选择PLL作为系统时钟源
        while(RCC_GetSYSCLKSource() != 0x08); // 读取时钟切换状态位，确保系统时钟切换完成
    }
    else // 如果HSE开启失败
    {
        while(1);
    }
}

void HSI_Clock_Init(uint32_t pllmul)
{
    __IO uint32_t HSIStartUpStatus = 0;

    RCC_DeInit();
    RCC_HSICmd(ENABLE); // 启动HSI
    HSIStartUpStatus = RCC->CR & RCC_CR_HSIRDY; // 等待HSI就绪
    if(HSIStartUpStatus == RCC_CR_HSIRDY)
    {
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
        FLASH_SetLatency(FLASH_Latency_2);
        RCC_HCLKConfig(RCC_SYSCLK_Div1);
        RCC_PCLK2Config(RCC_HCLK_Div1);
        RCC_PCLK1Config(RCC_HCLK_Div2);
        RCC_PLLConfig(RCC_PLLSource_HSI_Div2, pllmul);
        RCC_PLLCmd(ENABLE);
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        while(RCC_GetSYSCLKSource() != 0x08);
    }
    else
    {
        while(1);
    }
}
```

### 3.2 MCO GPIO初始化

```c
// PA8复用为MCO输出
void MCO_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
```

### 3.3 主函数测试

```c
// 程序来到main函数之前，启动文件：statup_stm32f10x_hd.s已经调用SystemInit()函数把系统时钟初始化成72MHZ
// SystemInit()在system_stm32f10x.c中定义
// 如果用户想修改系统时钟，可自行编写程序修改
// 重新设置系统时钟，根据需要修改，一般设置最高为128MHz
// SYSCLK = 8M * RCC_PLLMul_x, x:[2,3,...16]
int main()
{
    // HSE_Clock_Init(RCC_PLLMul_9); // 8M * 9 = 72M 
    // 如果HSI要作为PLL时钟的来源的话，必须二分频之后才可以，即HSI/2，而PLL倍频因子最大只能是16
    HSI_Clock_Init(RCC_PLLMul_16); // 4M * 16 = 64M

    // 设置MCO引脚输出时钟，用示波器即可在PA8测量到输出的时钟信号
    // MCO引脚输出可以是HSE,HSI,PLLCLK/2,SYSCLK
    RCC_MCOConfig(RCC_MCO_SYSCLK); // 本工程设置为64M
}
```

## 4. RCC常见函数（STD库）

### 4.1 时钟源使能与切换

- **使能HSE（外部高速时钟）**

```c
RCC_HSEConfig(RCC_HSE_ON);  // 开启HSE（如外部晶振）
while (RCC_WaitForHSEStartUp() != SUCCESS); // 等待HSE稳定
```

- **使能HSI（内部高速时钟）**

```c
RCC_HSICmd(ENABLE);         // 开启HSI（内部8MHz RC振荡器）
while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET); // 等待HSI就绪
```

- **选择系统时钟源**

```c
RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);   // 选择HSE作为系统时钟源
RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);    // 选择HSI作为系统时钟源
RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); // 选择PLL输出作为系统时钟源
```

### 4.2 PLL配置

- **配置PLL倍频参数**

```c
RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); // HSE不分频，倍频9倍（8MHz * 9 = 72MHz）
RCC_PLLCmd(ENABLE);             // 启动PLL
while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); // 等待PLL就绪
```

### 4.3 总线时钟分频

- **配置AHB、APB1、APB2总线分频系数**

```c
RCC_HCLKConfig(RCC_SYSCLK_Div1);    // AHB时钟 = 系统时钟（72MHz）
RCC_PCLK1Config(RCC_HCLK_Div2);     // APB1时钟 = 36MHz（最大36MHz）
RCC_PCLK2Config(RCC_HCLK_Div1);     // APB2时钟 = 72MHz
```

### 4.4 外设时钟使能

- **APB2外设时钟使能（如GPIO、USART1、SPI1等）**

```c
RCC_APB2PeriphClockCmd(
  RCC_APB2Periph_GPIOA |    // 使能GPIOA时钟
  RCC_APB2Periph_USART1 |    // 使能USART1时钟
  RCC_APB2Periph_AFIO,      // 使能复用功能时钟
  ENABLE
);
```

- **APB1外设时钟使能（如USART2、SPI2、TIM2等）**

```c
RCC_APB1PeriphClockCmd(
  RCC_APB1Periph_USART2 |    // 使能USART2时钟
  RCC_APB1Periph_TIM2,       // 使能TIM2时钟
  ENABLE
);
```

### 4.5 时钟状态与标志位操作

- **获取当前系统时钟源**

```c
uint32_t sysclk_source = RCC_GetSYSCLKSource(); 
// 返回值：RCC_SYSCLKSource_HSI/HSE/PLL
```

- **检查时钟是否就绪**

```c
if (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == SET) {
  // HSE已就绪
}
```

- **清除时钟标志位**

```c
RCC_ClearFlag(); // 清除所有RCC标志位
```

### 4.6 其他功能

- **使能备份寄存器时钟**

```c
RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); // 先使能PWR时钟
RCC_BackupResetCmd(ENABLE);  // 复位备份域（可选）
```

- **时钟安全系统（CSS）**

```c
RCC_ClockSecuritySystemCmd(ENABLE); // 启用时钟安全监测（HSE失效时切换HSI）
```
