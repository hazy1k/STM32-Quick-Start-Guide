# 第五十九章 PWR-PVD电源监控实验

## 1. 硬件设计

本实验中使用PVD监控STM32芯片的VDD引脚，当监测到供电电压低于阈值时会产生PVD中断，系统进入中断服务函数进入紧急处理过程。 所以进行这个实验时需要使用一个可调的电压源给实验板供电，改变给STM32芯片的供电电压，为此我们需要先了解实验板的电源供电系统：

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/Power005.jpeg" title="" alt="" width="1090">

整个电源供电系统主要分为以下五部分：

1. 6-12V的DC电源供电系统，这部分使用DC电源接口引入6-12V的电源， 经过RT7272进行电压转换成5V电源，再与第二部分的“5V_USB”电源线连接在一起。

2. 第二部分使用USB接口，使用USB线从外部引入5V电源， 引入的电源经过电源开关及保险丝连接到“5V”电源线。

3. 第三部分的是电源开关及保险丝，即当我们的实验板使用DC电源或“5V_USB”线供电时， 可用电源开关控制通断，保险丝也会起保护作用。

4. “5V”电源线遍布整个板子，板子上各个位置引出的标有“5V”丝印的排针都与这个电源线直接相连。 5V电源线给板子上的某些工作电压为5V的芯片供电。5V电源还经过LDO稳压芯片，输出3.3V电源连接到“3.3V”电源线。

5. 同样地，“3.3V”电源线也遍布整个板子，各个引出的标有“3.3V”丝印的排针都与它直接相连， 3.3V电源给工作电压为3.3V的各种芯片供电。STM32芯片的VDD引脚就是直接与这个3.3V电源相连的，所以通过STM32的PVD监控的就是这个“3.3V”电源线的电压。

当我们进行这个PVD实验时，为方便改变“3.3V”电源线的电压，我们可以把可调电源通过实验板上引出的“5V”及“GND” 排针给实验板供电， 当可调电源电压降低时，LDO在“3.3V”电源线的供电电压会随之降低，即STM32的PVD监控的VDD引脚电压会降低， 这样我们就可以模拟VDD电压下降的实验条件，对PVD进行测试了。不过，由于这样供电不经过保险丝， 所以在调节电压的时候要小心，不要给它供电远高于5V，否则可能会烧坏实验板上的芯片。

## 2. 软件设计

### 2.1 编程大纲

1. 初始化PVD中断，设置PVD电压监控等级并使能PVD

2. PVD中断服务函数

3. 主函数测试

### 2.2 代码分析

#### 2.2.1 初始化PVD设置电压阈值

```c
#include "pvd.h"

void PVD_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  /* 中断配置 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  /* EXTI配置 */
  EXTI_ClearITPendingBit(EXTI_Line16);
  EXTI_InitStructure.EXTI_Line = EXTI_Line16;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  /* PVD配置 */
  PWR_PVDLevelConfig(PWR_PVDLevel_2V6); // 设置电压阈值
  PWR_PVDCmd(ENABLE);
}
```

在这段代码中，执行的流程如下：

1. 配置PVD的中断优先级。 由于电压下降是非常危急的状态，所以请尽量把它配置成最高优先级。

2. 配置了EXTI16线的中断源， 设置EXTI16是因为PVD中断是通过EXTI16产生中断的(GPIO的中断是EXTI0-EXTI15)。

3. 使用库函数PWR_PVDLevelConfig设置PVD监控的电压阈值等级， 各个阈值等级表示的电压值请查阅表

4. 最后使用库函数PWR_PVDCmd使能PVD功能。

#### 2.2.2 PVD中断服务函数

```c
void PVD_IRQHandler(void)
{
    if(PWR_GetFlagStatus(PWR_FLAG_PVDO) == SET) // 电源管理器检测到PVD电压低于设定值            
    {
        LED_RED();     
    }
  EXTI_ClearITPendingBit(EXTI_Line16);
}
```

#### 2.2.3 主函数

```c
#include "stm32f10x.h"
#include "led.h"
#include "key.h" 
#include "usart.h"
#include "pvd.h"

int main(void)
{    
    LED_Init();    
    LED_GREEN(); 
    // 配置PVD，当电压过低时，会进入中断服务函数，亮红灯
    PVD_Config();
    while(1)
    {                    
    }
}
```

本电源监控实验的main函数执行流程比较简单，仅调用了PVD_Config配置监控功能，当VDD供电电压正常时，板子亮绿灯， 当电压低于阈值时，会跳转到中断服务函数中， 板子亮红灯

## 3. 小结

### 3.1 实验目标

- 理解PVD（电源电压检测器）的工作原理。
- 实现电源电压监控功能，当电压低于设定值时触发警报。

### 3.2 实验步骤

#### 1. 硬件连接

- 将LED连接到一个GPIO引脚（例如PA5），用于指示电压状态。
- 将蜂鸣器连接到另一个GPIO引脚（例如PB6），用于报警。
- 可调电源的输出连接到STM32的VDD引脚。

#### 2. 软件配置

1. **创建新项目**：在STM32开发环境中创建一个新项目，选择对应的STM32芯片。
2. **配置GPIO**：
   - 将PA5设置为输出模式，用于控制LED。
   - 将PB6设置为输出模式，用于控制蜂鸣器。

#### 3. 编写代码

以下是基于STM32的PVD电源监控代码示例：

```c
#include "stm32f4xx.h"

// 定义引脚
#define LED_PIN GPIO_Pin_5
#define LED_PORT GPIOA
#define BUZZER_PIN GPIO_Pin_6
#define BUZZER_PORT GPIOB

void GPIO_Config(void);
void PVD_Config(void);
void System_Clock_Config(void);

int main(void)
{
    // 初始化系统时钟
    System_Clock_Config();
    // 配置GPIO
    GPIO_Config();
    // 配置PVD
    PVD_Config();
    while (1)
    {
        // 主循环，持续监控PVD状态
        if (PWR_GetFlagStatus(PWR_FLAG_PVDO) != RESET)
        {
            // 电压低于设定值
            GPIO_SetBits(LED_PORT, LED_PIN); // 点亮LED
            GPIO_SetBits(BUZZER_PORT, BUZZER_PIN); // 激活蜂鸣器
        }
        else
        {
            // 电压正常
            GPIO_ResetBits(LED_PORT, LED_PIN); // 熄灭LED
            GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN); // 关闭蜂鸣器
        }
    }
}

// PVD配置函数
void PVD_Config(void)
{
    // 使能PWR时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    // 配置PVD电压检测器
    PWR_PVDCmd(ENABLE);
    PWR_PVDLevelConfig(PWR_PVDLevel_2_9V); // 设置阈值为2.9V（可根据需要调整）
}

// GPIO配置函数
void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // 使能GPIO时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    // 配置LED引脚为输出模式
    GPIO_InitStruct.GPIO_Pin = LED_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(LED_PORT, &GPIO_InitStruct);

    // 配置蜂鸣器引脚为输出模式
    GPIO_InitStruct.GPIO_Pin = BUZZER_PIN;
    GPIO_Init(BUZZER_PORT, &GPIO_InitStruct);
}

// 系统时钟配置函数
void System_Clock_Config(void)
{
    // 配置系统时钟的代码（可使用STM32CubeMX生成）
}

// 简单延时函数
void Delay(uint32_t time)
{
    while (time--);
}
```

---

2024.9.15 第一次修订，后期不再维护

2025.1.30 优化内容
