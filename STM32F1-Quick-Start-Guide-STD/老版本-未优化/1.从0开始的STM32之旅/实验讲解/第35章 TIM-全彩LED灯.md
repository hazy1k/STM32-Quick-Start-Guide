# 第三十五章 TIM-全彩LED灯

## 1. 导入

全彩LED灯，实质上是一种把红、绿、蓝单色发光体集成到小面积区域中的LED灯，控制时对这三种颜色的灯管输出不同的光照强度， 即可混合得到不同的颜色，其混色原理与光的三原色混合原理一致。

例如，若红绿蓝灯都能控制输出光照强度为[0:255]种等级，那么该灯可混合得到使用RGB888表示的所有颜色(包括以RGB三个灯管都全灭所表示的纯黑色)。

## 2. 全彩LED灯控制原理

本教程配套开发板中的RGB灯就是一种全彩LED灯，前面介绍LED基本控制原理的时候，只能控制RGB三色灯的亮灭，即RGB每盏灯有[0:1]两种等级，因此只能组合出8种颜色。

要使用STM32控制LED灯输出多种亮度等级，可以通过控制输出脉冲的占空比来实现。见图：

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/COLORT002.jpeg)

示例图中列出了周期相同而占空比分别为100%、80%、50和20%的脉冲波形，假如利用这样的脉冲控制LED灯，即可控制LED灯亮灭时间长度的比例。 若提高脉冲的频率，LED灯将会高频率进行开关切换，由于视觉暂留效应，人眼看不到LED灯的开关导致的闪烁现象， 而是感觉到使用不同占空比的脉冲控制LED灯时的亮度差别。即单个控制周期内，LED灯亮的平均时间越长，亮度就越高，反之越暗。

把脉冲信号占空比分成256个等级，即可用于控制LED灯输出256种亮度，使用三种这样的信号控制RGB灯即可得到256*256*256种颜色混合的效果。 而要控制占空比，直接使用STM32定时器的PWM功能即可。

## 3. 硬件设计

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/COLORT004.jpeg)

本实验中的RGB灯使用阴极分别连接到了PB5、PB0及PB1，它们分别是定时器TIM3的通道2、3、4，其中PB5用于定时器输出通道时，需要使用重定义功能。

## 4. 软件设计

### 4.1 编程大纲

1. TIM及三色灯相关参数宏定义

2. RGB使用的TIM初始化
2) 编写修改PWM脉冲占空比大小的函数；

3) 测试配置的定时器脉冲控制周期是否会导致LED灯明显闪烁；

### 4.2 代码分析

#### 4.2.1 定时器参数宏定义

```c
#ifndef __COLOR_LED_H
#define __COLOR_LED_H

#include "stm32f10x.h"

/* TIMx */
#define TIMx TIM3
#define TIM_APBxClock RCC_APB1PeriphClockCmd
#define TIM_CLK RCC_APB1Periph_TIM3
#define TIM_GPIO_CLK (RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)

// RED
#define GPIO_REMAP_FUN() GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); // TIM3重映射
#define RED_GPIO_PORT GPIOB
#define RED_GPIO_PIN GPIO_Pin_5
#define RED_OCxInit TIM_OC2Init  // 通道初始化函数
#define RED_OCxPreloadConfig TIM_OC2PreloadConfig  // 预装载配置函数
#define RED_CCRx CCR2
// GREEN
#define GREEN_GPIO_PORT GPIOB
#define GREEN_GPIO_PIN GPIO_Pin_0
#define GREEN_OCxInit TIM_OC3Init  
#define GREEN_OCxPreloadConfig TIM_OC3PreloadConfig 
#define GREEN_CCRx CCR3
// BLUE
#define BLUE_GPIO_PORT GPIOB
#define BLUE_GPIO_PIN GPIO_Pin_1
#define BLUE_OCxInit TIM_OC4Init  
#define BLUE_OCxPreloadConfig TIM_OC4PreloadConfig 
#define BLUE_CCRx CCR4

void TIMx_Init(void);
void SetRGBColor(uint32_t rgb);
void SetColorValue(uint8_t r, uint8_t g, uint8_t b);
void LED_Close(void);

#endif /* __COLOR_LED_H */

```

当然，其中出现的一个新的库函数值得我们介绍一下：

`GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE)` 是 STM32 标准外设库中的一个函数，用于配置 GPIO 引脚的复用功能。具体而言，它用于将 TIM3 定时器的部分功能从默认的引脚映射到其他引脚上。

函数参数说明

- **GPIO_PartialRemap_TIM3**：这是一个枚举值，指定了要进行引脚复用的具体功能。在这个例子中，它表示 TIM3 定时器的部分复用。
- **ENABLE**：这是一个布尔值，用于启用或禁用复用功能。在这里，`ENABLE` 启用引脚复用。

作用

1. **引脚复用**：STM32 微控制器允许某些引脚具有多个功能。`GPIO_PinRemapConfig` 函数可以改变引脚的功能映射。例如，TIM3 定时器的默认引脚可以被重新映射到其他引脚，以适应不同的硬件布局或设计需求。
2. **部分复用**：`GPIO_PartialRemap_TIM3` 表示 TIM3 的功能将被部分重新映射，而不是完全重新映射。

使用示例

如果你需要将 TIM3 的引脚从其默认位置重新映射到其他引脚，你可以使用这个函数来实现。例如，你可以将 TIM3 的 PWM 输出引脚从默认的 GPIO 端口重新映射到另一个端口，以避免与其他功能冲突。

#### 4.2.2 TIM输出PWM初始化

```c
static void TIM_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(TIM_GPIO_CLK, ENABLE);
	GPIO_REMAP_FUN(); // 重新映射GPIO
	// 红
	GPIO_InitStructure.GPIO_Pin = RED_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RED_GPIO_PORT, &GPIO_InitStructure);
	// 绿
	GPIO_InitStructure.GPIO_Pin = GREEN_GPIO_PIN;
	GPIO_Init(GREEN_GPIO_PORT, &GPIO_InitStructure);
	// 蓝
	GPIO_InitStructure.GPIO_Pin = BLUE_GPIO_PIN;
	GPIO_Init(BLUE_GPIO_PORT, &GPIO_InitStructure);
}

static void TIM_Mode_Init(void)
{
	TIM_APBxClock(TIM_CLK, ENABLE);
	// 时基结构初始化
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 255;
	TIM_TimeBaseStructure.TIM_Prescaler = 4000;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
	// PWM模式初始化
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	RED_OCxInit(TIMx, &TIM_OCInitStructure); // TIM3_CH2初始化
	RED_OCxPreloadConfig(TIMx, TIM_OCPreload_Enable); // TIM3_CH2预装载使能
	GREEN_OCxInit(TIMx, &TIM_OCInitStructure); // TIM3_CH3初始化
	GREEN_OCxPreloadConfig(TIMx, TIM_OCPreload_Enable); // TIM3_CH3预装载使能
	BLUE_OCxInit(TIMx, &TIM_OCInitStructure); // TIM3_CH4初始化
	BLUE_OCxPreloadConfig(TIMx, TIM_OCPreload_Enable); // TIM3_CH4预装载使能
	TIM_ARRPreloadConfig(TIMx, ENABLE); // 使能重装载寄存器
	TIM_Cmd(TIMx, ENABLE); 
}
```

与LED灯的基本控制不同，由于本实验直接使用定时器输出通道的脉冲信号控制LED灯，此处代码把GPIO相关的引脚都配置成了复用推挽输出模式。 其中由于红灯使用的引脚需要用到第二功能，本代码使用COLOR_GPIO_REMAP_FUN()进行了该引脚的功能重定义操作。    

本配置初始化了控制RGB灯用的定时器，它被配置为向上计数，TIM_Period被配置为255，即定时器每个时钟周期计数器加1，计数至255时溢出， 从0开始重新计数；而代码中的PWM通道配置，当计数器的值小于输出比较寄存器CCRx的值时，PWM通道输出低电平，点亮LED灯。 所以上述代码配置把输出脉冲的单个周期分成了256份（注意区分定时器的时钟周期和输出脉冲周期）， 而输出比较寄存器CCRx配置的值即该脉冲周期内LED灯点亮的时间份数，所以修改CCRx的值，即可控制输出[0:255]种亮度等级。

关于定时器中的TIM_Prescaler分频配置，只要让它设置使得PWM控制脉冲的频率足够高，让人看不出LED灯闪烁即可，可以修改使用其它参数测试。

#### 4.2.3 设置彩灯颜色

```c
// 设置RGB颜色值
void SetRGBColor(uint32_t rgb)
{
	TIMx->RED_CCRx   = (uint8_t)(rgb >> 16);// R
	TIMx->GREEN_CCRx = (uint8_t)(rgb >> 8); // G
	TIMx->BLUE_CCRx  = (uint8_t)(rgb);      // B
}
// 设置RGB颜色值
void SetColorValue(uint8_t r, uint8_t g, uint8_t b)
{
	TIMx->RED_CCRx   = r;
	TIMx->GREEN_CCRx = g;
	TIMx->BLUE_CCRx  = b;
}
```

#### 4.2.4 主函数测试

```c
#include "stm32f10x.h"
#include "color_led.h"

void Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

int main(void)
{			
	TIMx_Init();
	while(1)
	{	
		SetColorValue(0,0,0);
		Delay(0xFFFFFF);
		SetColorValue(255,255,255);
		Delay(0xFFFFFF);
		SetColorValue(255,0,0);
		Delay(0xFFFFFF);
		SetColorValue(0,255,0);
		Delay(0xFFFFFF);
		SetColorValue(0,0,255);
		Delay(0xFFFFFF);
		SetColorValue(255,255,0);
		Delay(0xFFFFFF);
		SetColorValue(0,255,255);
		Delay(0xFFFFFF);
		SetColorValue(255,0,255);
		Delay(0xFFFFFF);
		SetColorValue(192,192,192);
		Delay(0xFFFFFF);
		SetColorValue(128,128,128);
		Delay(0xFFFFFF);
		SetColorValue(128,0,0);
		Delay(0xFFFFFF);
	}
}

```

## 5. 小结

这个实验还算简单，就是控制TIM3的不同通道来实现PWM，从而控制不同颜色的亮度来形成RGB，我们小结一下流程：

### 5.1 实验步骤

1. **配置系统时钟**：确保系统时钟正确配置，以便定时器能在所需的频率下工作。

2. **启用 TIM3 时钟**：使能 TIM3 的时钟。

3. **配置 GPIO 引脚**：将 TIM3 的通道引脚配置为复用功能模式，连接到 RGB 彩灯的每个颜色通道（红色、绿色、蓝色）。

4. **配置 TIM3 的 PWM 模式**：设置 TIM3 的三个通道（通道 1、2、3）为 PWM 模式。

5. **设置 PWM 参数**：配置 PWM 的频率、占空比，并启动 TIM3。

6. **更新占空比**：通过调整占空比来实现不同颜色的亮度变化。

### 5.2 示例代码

以下是一个简单的代码示例，展示如何用 TIM3 的三个通道控制 RGB 彩灯：

```c
#include "stm32f0xx.h"

// 定义 PWM 参数
#define PWM_FREQUENCY 1000  // PWM 频率 1 kHz
#define PWM_PERIOD (SystemCoreClock / PWM_FREQUENCY) // PWM 周期

void TIM3_Config(void)
{
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    // 启用 TIM3 时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 配置 GPIO 引脚（假设引脚为 GPIOA 的 PA6, PA7, PA8）
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 选择复用功能
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_2); // TIM3_CH1
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_2); // TIM3_CH2
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_2); // TIM3_CH3

    // 配置 TIM3 时间基数
    TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // 配置 TIM3 通道 1、2、3 为 PWM 模式 1
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = 0;  // 初始脉冲宽度为 0

    // 配置 TIM3 通道 1
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    // 配置 TIM3 通道 2
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    // 配置 TIM3 通道 3
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);

    // 启用 TIM3 的预装载功能
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

    // 使能 TIM3
    TIM_Cmd(TIM3, ENABLE);
}

int main(void)
{
    // 系统初始化
    SystemInit();

    // 配置 TIM3
    TIM3_Config();

    // 设置初始 RGB 颜色（例如全红）
    TIM_SetCompare1(TIM3, PWM_PERIOD / 2);  // 红色
    TIM_SetCompare2(TIM3, 0);               // 绿色
    TIM_SetCompare3(TIM3, 0);               // 蓝色

    while (1)
    {
        // 主循环，调节 RGB 颜色
        // 例如，逐步改变红色亮度
        for (int i = 0; i <= PWM_PERIOD; i += 10)
        {
            TIM_SetCompare1(TIM3, i);
            Delay(10); // 延时函数，调整亮度变化速度
        }
        // 同样可以实现绿色和蓝色的调整
    }
}

void Delay(__IO uint32_t nTime)
{
    // 简单延时函数实现
    for (; nTime != 0; nTime--);
}
```

### 5.3 代码说明

1. **GPIO 配置**：将 PA6、PA7、PA8 配置为 TIM3 的 PWM 输出引脚。
2. **定时器配置**：设置 TIM3 为 PWM 模式，周期根据所需的 PWM 频率计算。
3. **PWM 配置**：初始化三个通道（通道 1、2、3）为 PWM 模式，并设置初始脉冲宽度为 0。
4. **颜色控制**：通过改变 PWM 占空比来调整 RGB 彩灯的颜色。

---

2024.9.17 第一次修订，后期不再维护

2025.1.28 简化内容
