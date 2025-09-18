# 第三十六章 TIM-单色呼吸灯

## 1. 硬件设计

使用定时器输出控制LED灯呈呼吸效果，无其他硬件设计

## 2. 软件设计

### 2.1 编程大纲

1. TIM相关参数宏定义

2. 初始化PWM输出通道，初始化PWM工作模式

3. 计算获取PWM数据表；

4. 编写中断服务函数，在中断服务函数根据PWM数据表切换比较寄存器的值

5. 主函数测试

### 2.2 代码分析

#### 2.2.1 定时器及通道相关参数宏定义

```c
/* TIM配置 */
#define RED_TIMx TIM3
#define RED_APBxClock RCC_APB1PeriphClockCmd
#define RED_TIM_CLK RCC_APB1Periph_TIM3
#define RED_TIM_GPIO_APBxClock RCC_APB2PeriphClockCmd
#define RED_TIM_GPIO_CLK (RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)
/* 红灯引脚重映射 */
#define RED_GPIO_REMAP_FUN() GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
#define RED_GPIO_PORT GPIOB
#define RED_GPIO_PIN GPIO_Pin_5
#define RED_TIM_OCxInit TIM_OC2Init
#define RED_TIM_OCxPreloadConfig TIM_OC2PreloadConfig
#define RED_CCRx CCR2
/* 中断配置 */
#define RED_TIM_IRQ TIM3_IRQn
#define RED_TIM_IRQHandler TIM3_IRQHandler
```

#### 2.2.2 PWM表

```c
__IO uint16_t period_class = 10; // 输出波形的频率

// PWM表
uint16_t indexWave[] = {
1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 4,
4, 5, 5, 6, 7, 8, 9, 10, 11, 13,
15, 17, 19, 22, 25, 28, 32, 36,
41, 47, 53, 61, 69, 79, 89, 102,
116, 131, 149, 170, 193, 219, 250,
284, 323, 367, 417, 474, 539, 613,
697, 792, 901, 1024, 1024, 901, 792,
697, 613, 539, 474, 417, 367, 323,
284, 250, 219, 193, 170, 149, 131, 
116, 102, 89, 79, 69, 61, 53, 47, 41,
36, 32, 28, 25, 22, 19, 17, 15, 13, 
11, 10, 9, 8, 7, 6, 5, 5, 4, 4, 3, 3,
2, 2, 2, 2, 1, 1, 1, 1
};

uint16_t PWM_Num = sizeof(indexWave)/sizeof(indexWave[0]); // PWM波形元素个数
```

#### 2.2.3 TIM输出PWM模式配置

```c
static void TIM_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = RED_TIM_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void TIM_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RED_TIM_GPIO_CLK, ENABLE);
    RED_TIM_GPIO_APBxClock(RED_TIM_GPIO_CLK, ENABLE);
    RED_GPIO_REMAP_FUN(); // 重新映射GPIO
    // 呼吸灯使用到的引脚配置
    GPIO_InitStructure.GPIO_Pin = RED_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(RED_GPIO_PORT, &GPIO_InitStructure);
}

static void TIM_Mode_Init(void)
{
    RED_APBxClock(RED_TIM_CLK, ENABLE);
    // 时基配置
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = (1024-1);
    TIM_TimeBaseStructure.TIM_Prescaler = (200-1);
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(RED_TIMx, &TIM_TimeBaseStructure);
    // PWM模式配置
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    RED_TIM_OCxInit(RED_TIMx, &TIM_OCInitStructure);
    RED_TIM_OCxPreloadConfig(RED_TIMx, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(RED_TIMx, ENABLE); // 预装载寄存器
    TIM_Cmd(RED_TIMx, ENABLE);
    TIM_ITConfig(RED_TIMx, TIM_IT_Update, ENABLE);
}

void breathing_init(void)
{
    TIM_NVIC_Init();
    TIM_GPIO_Init();
    TIM_Mode_Init();
}
```

本配置主体与全彩LED灯实验中的类似，代码中初始化了控制RGB灯用的定时器，它被配置为向上计数， PWM通道输出也被配置成当计数器CNT的值小于输出比较寄存器CCRx的值时，PWM通道输出低电平，点亮LED灯。 在函数的最后还使能了定时器中断，每当定时器的一个计数周期完成时，产生中断，配合中断服务函数， 即可切换CCRx比较寄存器的值。

代码中的TIM_Period和TIM_Prescaler是关键配置。

其中TIMPeriod被配置为(1024-1)，它控制控制定时器的定时周期，定时器的计数寄存器CNT从0开始，每个时钟会对计数器加1， 计数至1023时完成一次计数，产生中断，也就是说一共1024个计数周期，与PWM表元素中的最大值相同。若定时器的输出比较寄存器CCRx被赋值为PWM表中的元素， 即可改变输出对应占空比的PWM波，控制LED灯，如：

![屏幕截图 2024 08 23 105037](https://img.picgo.net/2024/08/23/-2024-08-23-10503727c809d7f7479a00.png)

根据本工程中的PWM表更新CCRx的值，即可输出占空比呈呼吸特性曲线变化的PWM波形，达到呼吸灯的效果。

最终，拟合曲线的周期由TIMPeriod、PWM表的点数、TIM_Prescaler以及下面中断服务函数的period_cnt比较值共同决定， 本工程需要调整这些参数使得拟合曲线的周期约为3秒，从而达到较平缓的呼吸效果。

#### 2.2.4 TIM更新中断服务函数

```c
extern uint16_t indexWave[];

/* 呼吸灯中断服务函数 */
void RED_TIM_IRQHandler(void)
{    
    static uint16_t pwm_index = 0;    // 用于PWM查表
    static uint16_t period_cnt = 0;    // 用于计算周期数
    if (TIM_GetITStatus(RED_TIMx, TIM_IT_Update) != RESET) // TIM_IT_Update
     {            
        period_cnt++;
        RED_TIMx->RED_CCRx = indexWave[pwm_index];    // 根据PWM表修改定时器的比较寄存器值
        if(period_cnt > period_class)                                 
        {                
            pwm_index++;
            //若PWM表已到达结尾，重新指向表头
            if(pwm_index >=  PWM_Num)            
            {
                pwm_index=0;                                
            }    
            period_cnt=0;    
        }    
        else
        {
        }        
    TIM_ClearITPendingBit(RED_TIMx, TIM_IT_Update);
    }
}
```

#### 2.2.5 主函数测试

```c
#include "stm32f10x.h"
#include "breathing.h"

int main(void)
{            
    breathing_init();
    while(1)
    {
    }        
}
```

## 3. 小结

本章实际上就是利用TIM控制高低电平时间来模拟一个呼吸灯，稍微总结一下：

### 3.1 实现步骤

1. **配置 TIM 定时器**：设置一个定时器产生 PWM 信号。
2. **配置 PWM 输出**：通过 PWM 信号调节 LED 的亮度。
3. **实现呼吸效果**：在主循环中调整 PWM 的占空比，使 LED 的亮度随时间变化。

### 3.2 示例代码

以下是如何用 TIM3 产生单色呼吸灯的示例代码。假设你使用的是 STM32F103 系列单片机。

```c
#include "stm32f10x.h"

// 定义 PWM 参数
#define PWM_FREQUENCY 1000  // PWM 频率 1 kHz
#define PWM_PERIOD (SystemCoreClock / PWM_FREQUENCY) // PWM 周期

void TIM3_Config(void)
{
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    // 启用 TIM3 时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 配置 GPIO 引脚（假设为 GPIOA 的 PA6）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置 TIM3 时间基数
    TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // 配置 TIM3 通道 1 为 PWM 模式 1
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = 0;  // 初始脉冲宽度为 0

    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

    // 使能 TIM3
    TIM_Cmd(TIM3, ENABLE);
}

void Delay(__IO uint32_t nTime)
{
    // 简单延时函数实现
    for (; nTime != 0; nTime--);
}

int main(void)
{
    // 系统初始化
    SystemInit();

    // 配置 TIM3
    TIM3_Config();

    uint16_t pwm_duty_cycle;
    int8_t direction = 1;  // 1: 增加亮度, -1: 减少亮度

    while (1)
    {
        // 更新 PWM 占空比，模拟呼吸灯效果
        for (pwm_duty_cycle = 0; pwm_duty_cycle < PWM_PERIOD; pwm_duty_cycle += 10)
        {
            TIM_SetCompare1(TIM3, pwm_duty_cycle);
            Delay(100); // 控制呼吸灯的速度
        }

        // 反向减少亮度
        for (pwm_duty_cycle = PWM_PERIOD; pwm_duty_cycle > 0; pwm_duty_cycle -= 10)
        {
            TIM_SetCompare1(TIM3, pwm_duty_cycle);
            Delay(100); // 控制呼吸灯的速度
        }
    }
}
```

### 3.3 代码说明

1. **TIM 配置**：配置 TIM3 为 PWM 模式，并设置 PWM 频率为 1 kHz。
2. **GPIO 配置**：将 GPIOA 的 PA6 引脚设置为 TIM3 的 PWM 输出引脚。
3. **PWM 占空比控制**：通过更新 TIM3 通道 1 的比较值来控制 LED 的亮度。
4. **呼吸效果**：在主循环中，通过逐渐增加和减少占空比实现 LED 亮度的渐变，模拟呼吸效果。

---

2024.9.17 第一次修订，后期不再维护

2025.1.28 优化代码，去除条件编译
