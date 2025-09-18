# 第十四章 基本定时器介绍及应用

## 1. 定时器分类

    STM32F1 系列中，除了互联型的产品，共有 8 个定时器，分为基本定时器，通用定时器和高级定时器。基本定时器 TIM6 和 TIM7 是一个 16 位的只能向上计数的定时器，只能定时，没有外部IO。通用定时器 TIM2/3/4/5 是一个 16 位的可以向上/下计数的定时器，可以定时，可以输出比较，可以输入捕捉，每个定时器有四个外部 IO。高级定时器 TIM1/8 是一个 16 位的可以向上/下计数的定时器，可以定时，可以输出比较，可以输入捕捉，还可以有三相电机互补输出信号，每个定时器有 8 个外部 IO。

![屏幕截图 2025-07-05 000824.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/07/05-00-08-30-屏幕截图%202025-07-05%20000824.png)

## 2. 定时器初始化结构体

在标准库函数头文件 stm32f10x_tim.h 中对定时器外设建立了四个初始化结构体，基本定时器只用到其中一个即 TIM_TimeBaseInitTypeDe

```c
typedef struct {
    uint16_t TIM_Prescaler; // 预分频器
    uint16_t TIM_CounterMode; // 计数模式
    uint32_t TIM_Period; // 定时器周期
    uint16_t TIM_ClockDivision; // 时钟分频
    uint8_t TIM_RepetitionCounter; // 重复计算器
}TIM_TimeBaseInitTypeDef;
```

1. TIM_Prescaler：定时器预分频器设置，时钟源经该预分频器才是定时器时钟，它设定 TIMx_PSC寄存器的值。可设置范围为 0 至 65535，实现 1 至 65536 分频。

2. TIM_CounterMode：定时器计数方式，可是在为向上计数、向下计数以及三种中心对齐模式。基本定时器只能是向上计数，即 TIMx_CNT 只能从 0 开始递增，并且无需初始化。

3. TIM_Period：定时器周期，实际就是设定自动重载寄存器的值，在事件生成时更新到影子寄存器。可设置范围为 0 至 65535。

4. TIM_ClockDivision：时钟分频，设置定时器时钟 CK_INT 频率与数字滤波器采样时钟频率分频比，基本定时器没有此功能，不用设置。

5. TIM_RepetitionCounter：重复计数器，属于高级控制寄存器专用寄存器位，利用它可以非常容易控制输出 PWM 的个数。这里不用设置。

虽然定时器基本初始化结构体有 5 个成员，但对于基本定时器只需设置其中两个就可以

## 3. 基本定时器使用示例

### 3.1 TIM模式配置

```c
static void TIM_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM_Mode_Init(void)
{
    TIM_NVIC_Init();
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    TIM_TimeBaseStructure.TIM_Period = 999;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM6, TIM_FLAG_Update);
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE); // 计数器溢出中断使能
    TIM_Cmd(TIM6, ENABLE);
}
/*  
    具体计算过程
    定时器时钟源为APB1，APB1为72MHz
    定时器的频率为72/(71+1) = 1MHz
    定时器的计数周期为999+1，计数周期/定时器频率 = 1ms
*/
```

### 3.2 中断服务函数

```c
void TIM6_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
    {
        time++;
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }
}

```

### 3.3 主函数测试

```c
#include "stm32f10x.h"
#include "tim.h"
#include "usart.h"
#include "led.h"

extern volatile uint32_t time;

int main(void)
{    
    USARTx_Init();   
    BSP_LED_Init();
    TIM_Mode_Init();
    while(1)
    {
        if(time  == 1000) // 1000*1=1ms
        {
            time = 0;
            LED_TOGGLE(RED_LED_Pin);
            printf("LED TOGGLE\r\n");
        }
    }
}

```

## 4. 基本定时器常见函数（STD库）

### 4.1 初始化与启动

```c
// 使能定时器时钟
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); // TIM6
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); // TIM7

// 配置时基参数
void TIM_TimeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);
```

### 4.2 参数结构体（仅需设置两个成员）

```c
typedef struct {
  uint16_t TIM_Prescaler;         // 预分频值 (0~65535)
  uint16_t TIM_Period;            // 自动重装载值 (0~65535)
  // 以下成员对TIM6/7无效，可忽略：
  uint16_t TIM_CounterMode;       // 基本定时器仅支持向上计数
  uint16_t TIM_ClockDivision;     // 无此功能
  uint8_t TIM_RepetitionCounter;  // 高级定时器专用
} TIM_TimeBaseInitTypeDef;
```

**常见配置示例**：

| 预分频值 (PSC) | 重装载值 (ARR) | 定时周期   | 应用场景  |
| ---------- | ---------- | ------ | ----- |
| 7199       | 4999       | 500 ms | LED闪烁 |
| 71         | 999        | 1 ms   | 系统节拍  |
| 0          | 71         | 1 μs   | 高精度延时 |
