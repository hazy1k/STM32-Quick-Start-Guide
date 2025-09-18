# 第十五章 ATIM介绍及应用

## 1. 高级定时器简介

高级控制定时器 (TIM1 和 TIM8) 和通用定时器在基本定时器的基础上引入了外部引脚，可以实现输入捕获和输出比较功能。高级控制定时器时基单元包含一个 16 位自动重装载寄存器 ARR，一个 16 位的计数器 CNT，可向上/下计数，一个 16 位可编程预分频器 PSC，预分频器时钟源有多种可选，有内部的时钟、外部时钟。还有一个 8 位的重复计数器 RCR，这样最高可实现 40 位的可编程定时。

![屏幕截图 2025-07-07 234122.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/07/07-23-41-29-屏幕截图%202025-07-07%20234122.png)

## 2. 定时器初始化结构体

### 2.1 TIM_TimeBaseInitTypeDef

时基结构体 TIM_TimeBaseInitTypeDef 用于定时器基础参数设置，与 TIM_TimeBaseInit 函数配合使用完成配置。

```c
typedef struct {
    uint16_t TIM_Prescaler; // 预分频器
    uint16_t TIM_CounterMode; // 计数模式
    uint32_t TIM_Period; // 定时器周期
    uint16_t TIM_ClockDivision; // 时钟分频
    uint8_t TIM_RepetitionCounter; // 重复计算器
}TIM_TimeBaseInitTypeDef;
```

1. TIM_Prescaler：定时器预分频器设置，时钟源经该预分频器才是定时器计数时钟 CK_CNT，它设定 PSC 寄存器的值。计算公式为：计数器时钟频率 (fCK_CNT) 等于 fCK_PSC / (PSC[15:0] + 1)，可实现 1 至 65536 分频。

2. TIM_CounterMode：定时器计数方式，可设置为向上计数、向下计数以及中心对齐。高级控制定时器允许选择任意一种。

3. TIM_Period：定时器周期，实际就是设定自动重载寄存器 ARR 的值， ARR 为要装载到实际自动重载寄存器（即影子寄存器）的值，可设置范围为 0 至 65535。

4. TIM_ClockDivision：时钟分频，设置定时器时钟 CK_INT 频率与死区发生器以及数字滤波器采样时钟频率分频比。可以选择 1、 2、 4 分频。

5. TIM_RepetitionCounter：重复计数器，只有 8 位，只存在于高级定时器。

### 2.2 TIM_OCInitTypeDef

输出比较结构体 TIM_OCInitTypeDef 用于输出比较模式，与 TIM_OCxInit 函数配合使用完成指定定时器输出通道初始化配置。高级控制定时器有四个定时器通道，使用时都必须单独设置。

```c
typedef struct {
    uint16_t TIM_OCMode; // 比较输出模式
    uint16_t TIM_OutputState; // 比较输出使能
    uint16_t TIM_OutputNState; // 比较互补输出使能
    uint32_t TIM_Pulse; // 脉冲宽度
    uint16_t TIM_OCPolarity; // 输出极性
    uint16_t TIM_OCNPolarity; // 互补输出极性
    uint16_t TIM_OCIdleState; // 空闲状态下比较输出状态
    uint16_t TIM_OCNIdleState; // 空闲状态下比较互补输出状态
} TIM_OCInitTypeDef;
```

1. TIM_OCMode：比较输出模式选择，总共有八种，常用的为 PWM1/PWM2。它设定 CCMRx 寄存器 OCxM[2:0] 位的值

2. TIM_OutputState：比较输出使能，决定最终的输出比较信号 OCx 是否通过外部引脚输出。它设定 TIMx_CCER 寄存器 CCxE/CCxNE 位的值。

3. TIM_OutputNState: 比较互补输出使能，决定 OCx 的互补信号 OCxN 是否通过外部引脚输出。它设定 CCER 寄存器 CCxNE 位的值。

4. TIM_Pulse：比较输出脉冲宽度，实际设定比较寄存器 CCR 的值，决定脉冲宽度。可设置范围为 0 至 65535。

5. TIM_OCPolarity：比较输出极性，可选 OCx 为高电平有效或低电平有效。它决定着定时器通道有效电平。它设定 CCER 寄存器的 CCxP 位的值。

6. TIM_OCNPolarity：比较互补输出极性，可选 OCxN 为高电平有效或低电平有效。它设定TIMx_CCER 寄存器的 CCxNP 位的值。

7. TIM_OCIdleState：空闲状态时通道输出电平设置，可选输出 1 或输出 0，即在空闲状态(BDTR_MOE 位为 0) 时，经过死区时间后定时器通道输出高电平或低电平。它设定 CR2 寄存器的 OISx 位的值。

8. TIM_OCNIdleState：空闲状态时互补通道输出电平设置，可选输出 1 或输出 0，即在空闲状态(BDTR_MOE 位为 0) 时，经过死区时间后定时器互补通道输出高电平或低电平，设定值必须与TIM_OCIdleState 相反。它设定是 CR2 寄存器的 OISxN 位的值。

### 2.3 TIM_ICInitTypeDef

输入捕获结构体 TIM_ICInitTypeDef 用于输入捕获模式，与 TIM_ICInit 函数配合使用完成定时器输入通道初始化配置。如果使用 PWM 输入模式需要与 TIM_PWMIConfig 函数配合使用完成定时器输入通道初始化配置。

```c
typedef struct {
    uint16_t TIM_Channel; // 输入通道选择
    uint16_t TIM_ICPolarity; // 输入捕获触发选择
    uint16_t TIM_ICSelection; // 输入捕获选择
    uint16_t TIM_ICPrescaler; // 输入捕获预分频器
    uint16_t TIM_ICFilter; // 输入捕获滤波器
}TIM_ICInitTypeDef;
```

1. TIM_Channel：捕获通道 ICx 选择，可选 TIM_Channel_1、 TIM_Channel_2、 TIM_Channel_3 或TIM_Channel_4 四个通道。它设定 CCMRx 寄存器 CCxS 位的值。

2. TIM_ICPolarity：输入捕获边沿触发选择，可选上升沿触发、下降沿触发或边沿跳变触发。它设定 CCER 寄存器 CCxP 位和 CCxNP 位的值。

3. TIM_ICSelection：输入通道选择，捕获通道 ICx 的信号可来自三个输入通道，分别为TIM_ICSelection_DirectTI、 TIM_ICSelection_IndirectTI 或 TIM_ICSelection_TRC，具体的区别见图输入通道与捕获通道 IC 的映射图 。如果是普通的输入捕获， 4 个通道都可以使用，如果是 PWM输入则只能使用通道 1 和通道 2。它设定 CCRMx 寄存器的 CCxS[1:0] 位的值。

![屏幕截图 2025-07-07 235406.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/07/07-23-54-13-屏幕截图%202025-07-07%20235406.png)

4. TIM_ICPrescaler：输入捕获通道预分频器，可设置 1、 2、 4、 8 分频，它设定 CCMRx 寄存器的 ICxPSC[1:0] 位的值。如果需要捕获输入信号的每个有效边沿，则设置 1 分频即可。

5. TIM_ICFilter：输入捕获滤波器设置，可选设置 0x0 至 0x0F。它设定 CCMRx 寄存器 ICxF[3:0]位的值。一般我们不使用滤波器，即设置为 0。

### 2.4 TIM_BDTRInitTypeDef

断路和死区结构体 TIM_BDTRInitTypeDef 用于断路和死区参数的设置，属于高级定时器专用，用于配置断路时通道输出状态，以及死区时间。它与 TIM_BDTRConfig 函数配置使用完成参数配置。

```c
typedef struct {
    uint16_t TIM_OSSRState; // 运行模式下的关闭状态选择
    uint16_t TIM_OSSIState; // 空闲模式下的关闭状态选择
    uint16_t TIM_LOCKLevel; // 锁定配置
    uint16_t TIM_DeadTime; // 死区时间
    uint16_t TIM_Break; // 断路输入使能控制
    uint16_t TIM_BreakPolarity; // 断路输入极性
    uint16_t TIM_AutomaticOutput; // 自动输出使能
}TIM_BDTRInitTypeDef;
```

1. TIM_OSSRState：运行模式下的关闭状态选择，它设定 BDTR 寄存器 OSSR 位的值。

2. TIM_OSSIState：空闲模式下的关闭状态选择，它设定 BDTR 寄存器 OSSI 位的值。

3. TIM_LOCKLevel：锁定级别配置， BDTR 寄存器 LOCK[1:0] 位的值。

4. TIM_DeadTime：配置死区发生器，定义死区持续时间，可选设置范围为 0x0 至 0xFF。它设定BDTR 寄存器 DTG[7:0] 位的值。

5. TIM_Break：断路输入功能选择，可选使能或禁止。它设定 BDTR 寄存器 BKE 位的值。

6. TIM_BreakPolarity：断路输入通道 BRK 极性选择，可选高电平有效或低电平有效。它设定BDTR 寄存器 BKP 位的值。

7. TIM_AutomaticOutput：自动输出使能，可选使能或禁止，它设定 BDTR 寄存器 AOE 位的值。

## 3. 高级定时器使用示例

### 3.1 PWM互补输出

输出比较模式比较多，这里我们以 PWM 输出为例讲解，并通过示波器来观察波形。实验中不仅在主输出通道输出波形，还在互补通道输出与主通道互补的的波形，并且添加了断路和死区功能。

使用高级定时器 TIM1 的通道 1及其互补通道作为本实验的波形输出通道，对应选择 PA8 和 PB13 引脚。将示波器的两个输入通道分别与 PA8 和 PB13 引脚连接，用于观察波形

为增加断路功能，需要用到 TIM1_BKIN 引脚，这里选择 PB12 引脚。程序我们设置该引脚为高电平有效，当 BKIN 引脚被置高电平的时候，两路互补的 PWM 输出就被停止，就好像是刹车一样。

#### 3.1.1 计算死区时间

```c
// 计算死区时间 (单位: ns)
static uint32_t calc_deadtime(uint8_t dt)
{
    // 系统时钟频率 (Hz)
    uint32_t system_clk = SystemCoreClock;
    // 定时器时钟频率 (Hz)
    uint32_t tim_clk = system_clk / (TIM1->PSC + 1);
    // 基本时间单位 (ns)
    float t_dts = 1000000000.0f / tim_clk;
    return (uint32_t)(dt * t_dts);
}
```

#### 3.1.2 TIM GPIO初始化

```c
static void ATIM_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // PA8: TIM1_CH1 (PWM主输出)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PB13: TIM1_CH1N (互补输出)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // PB12: 刹车输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  // 输入上拉模式
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
```

#### 3.1.3 TIM工作模式配置

```c
void ATIM_Mode_Init(void)
{
    ATIM_GPIO_Init();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    /* 时基单元配置 */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 7;          // ARR = 7
    TIM_TimeBaseStructure.TIM_Prescaler = 8;        // PSC = 8
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    /* 输出比较配置 */
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 4;  // 50%占空比
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

    /* 刹车和死区配置 */
    TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
    TIM_BDTRStructInit(&TIM_BDTRInitStructure);
    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
    TIM_BDTRInitStructure.TIM_DeadTime = 11;  // 死区时间值
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
    TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);

    // 使能定时器
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    // 清除中断标志
    TIM_ClearFlag(TIM1, TIM_FLAG_Update | TIM_FLAG_Break);
}
```

#### 3.1.4 TIM调试信息

```c
void TIM1_DebugInfo(void)
{
    // 计算实际输出频率和占空比
    uint32_t system_clk = SystemCoreClock;
    uint32_t tim_clk = system_clk / (TIM1->PSC + 1);
    float output_freq = (float)tim_clk / (TIM1->ARR + 1);
    float duty_cycle = (TIM1->CCR1 * 100.0f) / (TIM1->ARR + 1);
    uint32_t deadtime_ns = calc_deadtime(TIM1->BDTR & 0xFF);

    printf("\r\n===== TIM1 Configuration =====\r\n");
    printf("System Clock: %u Hz\r\n", system_clk);
    printf("TIM1 Clock: %u Hz\r\n", tim_clk);
    printf("PSC: %u\r\n", TIM1->PSC);
    printf("ARR: %u\r\n", TIM1->ARR);
    printf("CCR1: %u\r\n", TIM1->CCR1);
    printf("Output Freq: %.1f Hz\r\n", output_freq);
    printf("Duty Cycle: %.1f%%\r\n", duty_cycle);
    printf("Dead Time: %u ns\r\n", deadtime_ns);
    printf("Break Input (PB12): %s\r\n", 
           GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) ? "High" : "Low");
    printf("Break Flag: %s\r\n", 
           TIM_GetFlagStatus(TIM1, TIM_FLAG_Break) ? "Active" : "Inactive");
    printf("MOE Status: %s\r\n", 
           (TIM1->BDTR & TIM_BDTR_MOE) ? "Enabled" : "Disabled");
    printf("TIM1 Status: %s\r\n", 
           (TIM1->CR1 & TIM_CR1_CEN) ? "Enabled" : "Disabled");
    printf("BDTR Register: 0x%04X\r\n", TIM1->BDTR);
    printf("=============================\r\n");
}
```

#### 3.1.5 主函数测试

```c
#include "stm32f10x.h"
#include "tim.h"
#include "systick.h"
#include "usart.h"
#include <stdio.h>

int main(void)
{   
    SysTick_Init();
    USARTx_Init(115200);
    printf("\r\nSystem Booted\r\n");
    printf("TIM1 Advanced Timer Demo\r\n");
    printf("For break function test:\r\n");
    printf("  - Connect PB12 to 3.3V to trigger break\r\n");
    printf("  - Disconnect PB12 to clear break flag\r\n");
    ATIM_Mode_Init();
    TIM1_DebugInfo();
    while(1) {
        printf("\r\n===== Main Menu =====\r\n");
        printf("1. Read TIM1 Status\r\n");
        printf("2. Test Break Function (connect PB12 to 3.3V)\r\n");
        printf("3. Reset Break Flag (disconnect PB12 from 3.3V)\r\n");
        printf("4. Re-enable PWM Output\r\n");
        printf("Enter choice: ");
        char choice = USART_ReceiveChar();
        printf("\r\nSelected: %c\r\n", choice);

        switch(choice) {
            case '1':  // 读取状态
                TIM1_DebugInfo();
                break;

            case '2':  // 测试刹车功能
                printf("Testing Break Function...\r\n");
                printf("Current PB12 state: %s\r\n", 
                       GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) ? "HIGH" : "LOW");

                if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
                    printf("PB12 is already HIGH. Break should be active.\r\n");
                } else {
                    printf("Please connect PB12 to 3.3V to trigger break...\r\n");
                    // 等待用户操作
                    uint32_t start = SysTick_GetTick();
                    while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) && 
                          (SysTick_GetTick() - start < 5000)) {
                        // 等待PB12变高或超时(5秒)
                    }

                    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
                        printf("PB12 is now HIGH. Break should be active.\r\n");
                    } else {
                        printf("Timeout. PB12 still LOW.\r\n");
                    }
                }

                // 检查刹车状态
                if(TIM_GetFlagStatus(TIM1, TIM_FLAG_Break)) {
                    printf("Break Event Detected!\r\n");
                } else {
                    printf("No Break Event Detected! Check PB12 connection.\r\n");
                }

                TIM1_DebugInfo();
                break;

            case '3':  // 清除刹车标志
                printf("Clearing Break Flag...\r\n");
                printf("Current PB12 state: %s\r\n", 
                       GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) ? "HIGH" : "LOW");

                if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
                    printf("PB12 is still HIGH. Please disconnect PB12 from 3.3V...\r\n");
                    // 等待用户操作
                    uint32_t start = SysTick_GetTick();
                    while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) && 
                          (SysTick_GetTick() - start < 5000)) {
                        // 等待PB12变低或超时(5秒)
                    }
                }

                if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
                    TIM_ClearFlag(TIM1, TIM_FLAG_Break);
                    Delay_ms(10);  // 稍等片刻让标志位更新

                    if(TIM_GetFlagStatus(TIM1, TIM_FLAG_Break) == RESET) {
                        printf("Break flag cleared.\r\n");
                    } else {
                        printf("Failed to clear break flag!\r\n");
                    }
                } else {
                    printf("PB12 still HIGH. Cannot clear break flag.\r\n");
                }

                TIM1_DebugInfo();
                break;

            case '4':  // 重新使能PWM输出
                printf("Re-enabling PWM Output...\r\n");

                // 确保刹车标志已清除
                if(TIM_GetFlagStatus(TIM1, TIM_FLAG_Break)) {
                    printf("Break flag still active! Clear it first.\r\n");
                    TIM1_DebugInfo();
                    break;
                }

                // 重新配置BDTR（确保所有配置正确）
                TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
                TIM_BDTRStructInit(&TIM_BDTRInitStructure);
                TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
                TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
                TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
                TIM_BDTRInitStructure.TIM_DeadTime = 11;
                TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
                TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
                TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
                TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);

                // 使能主输出
                TIM1->BDTR |= TIM_BDTR_MOE;
                TIM_CtrlPWMOutputs(TIM1, ENABLE);

                printf("PWM output re-enabled.\r\n");
                TIM1_DebugInfo();
                break;

            default:
                printf("Invalid choice!\r\n");
        }
        Delay_ms(1000);
    }
}
```

### 3.2 脉宽测量输入

我们选用通用定时器 TIM5 的 CH1，就 PA0 这个 GPIO 来测量信号的脉宽。在开发板中 PA0 接的是一个按键，默认接 GND，当按键按下的时候 IO 口会被拉高，这个时候我们可以利用定时器的输入捕获功能来测量按键按下的这段高电平的时间

![屏幕截图 2025-07-08 105303.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/07/08-10-53-11-屏幕截图%202025-07-08%20105303.png)

#### 3.2.1 TIM GPIO初始化

```c
static void TIM_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
```

#### 3.2.2 TIM NVIC初始化

```c
static void TIM_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
```

#### 3.2.3 TIM工作模式初始化

```c
void TIM_Mode_Init(void)
{
    TIM_GPIO_Init();
    TIM_NVIC_Init();
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    /* 时基结构初始化 */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseStructure.TIM_Prescaler = (72 - 1);
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
    /* 输入捕获结构初始化 */
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; // 上升沿捕获
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0;
    TIM_ICInit(TIM5, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM5, TIM_FLAG_Update|TIM_IT_CC1);
    TIM_ITConfig(TIM5, TIM_IT_Update|TIM_IT_CC1, ENABLE);
    TIM_Cmd(TIM5, ENABLE);
}
```

#### 3.2.4 TIM中断函数

```c
void TIM5_IRQHandler(void) {
    // 处理定时器更新中断（溢出）
    if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) {
        TIM5_OverflowCount++;  // 溢出次数增加
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update); // 清除中断标志
    }
    // 处理通道1捕获中断
    if(TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET) {
        if(g_captureState == 0) {  // 第一次捕获：上升沿（按键按下）
            // 1. 清除计数器并重置溢出计数
            TIM5->CNT = 0;
            TIM5_OverflowCount = 0;      
            // 2. 切换为下降沿捕获
            TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Falling);
            // 3. 更新捕获状态
            g_captureState = 1;
        }
        else {  // 第二次捕获：下降沿（按键释放）
            // 1. 获取当前捕获值
            uint32_t captureEnd = TIM_GetCapture1(TIM5);
            // 2. 计算总时间（考虑溢出）
            g_capturePeriod = (TIM5_OverflowCount * 0x10000) + captureEnd;
            // 3. 设置新数据可用标志
            g_newCaptureAvailable = 1;
            // 4. 切换回上升沿捕获（准备下一次测量）
            TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Rising);
            // 5. 重置捕获状态
            g_captureState = 0;
        }
        TIM_ClearITPendingBit(TIM5, TIM_IT_CC1); // 清除捕获中断标志
    }
}
```

#### 3.2.5 获取捕获值

```c
// 获取按键持续时间（微秒）
uint32_t Get_KeyPressDuration(void) {
    g_newCaptureAvailable = 0; // 清除标志
    return g_capturePeriod;
}

// 检查是否有新的捕获数据
uint8_t Is_NewCaptureAvailable(void) {
    return g_newCaptureAvailable;
}
```

#### 3.2.6 主函数测试

```c
#include "stm32f10x.h"
#include "tim.h"
#include "systick.h"
#include "usart.h"
#include <stdio.h>

int main(void)
{   
    SysTick_Init();
    USARTx_Init(115200);
    TIM_Mode_Init();
    printf("TIM_Mode_Init...\r\n");
    while(1)
    {
        if(Is_NewCaptureAvailable())
        {
            uint32_t duration_us = Get_KeyPressDuration();
            float duration_ms = duration_us / 1000.0f;
            printf("duration_us: %d, duration_ms: %.2f\r\n", duration_us, duration_ms);
            Delay_ms(100);
        }
    }
}
```

### 3.3 PWM输入捕获

我们讲了用输入捕获测量了信号的脉宽，这一节我们讲输入捕获的一个特例—PWM 输入。普通的输入捕获可以使用定时器的四个通道，一路捕获占用一个捕获寄存器，而 PWM 输入则只能使用两个通道，即通道 1 和通道 2，且一路 PWM 输入要占用两个捕获寄存器，一个用于捕获周期，一个用于捕获占空比。在本节实验中，我们用通用定时器产生一路 PWM 信号，然后用高级定时器的通道 1 或者通道 2 来捕获。

#### 3.3.1 TIM2 PWM输出初始化

```c
// 全局变量存储捕获数据
static volatile PWM_CaptureData pwm_data;
static volatile uint32_t tim1_overflows = 0;

// TIM2 PWM输出初始化 (PA0)
void TIM2_PWM_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    // 1. 使能时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    // 2. 配置GPIOA.0为复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    // 3. 配置TIM2时基
    TIM_TimeBaseStructure.TIM_Period = 999;           // 自动重装载值 (1000-1)
    TIM_TimeBaseStructure.TIM_Prescaler = 71;         // 预分频器 (72MHz/72 = 1MHz)
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    // 4. 配置PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 300;  // 初始占空比 30%
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);
    // 5. 使能TIM2
    TIM_Cmd(TIM2, ENABLE);
    TIM_CtrlPWMOutputs(TIM2, ENABLE);
}

// 设置PWM参数
void TIM2_SetPWM(uint32_t period, uint32_t duty_cycle) {
    TIM2->ARR = period - 1;
    TIM2->CCR1 = duty_cycle;
}
```

#### 3.3.2 TIM1 PWM输入捕获初始化

```c
// TIM1 PWM输入捕获初始化 (PA8)
void TIM1_PWMCapture_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    // 1. 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA, ENABLE);
    // 2. 配置GPIOA.8为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    // 3. 配置TIM1时基
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;        // 最大计数器值
    TIM_TimeBaseStructure.TIM_Prescaler = 0;          // 不分频 (72MHz)
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    // 4. 配置PWM输入模式
    // 选择TI1作为输入源
    TIM_SelectInputTrigger(TIM1, TIM_TS_TI1FP1);
    // 从模式: 复位模式 (在上升沿复位计数器)
    TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Reset);
    // 使能主从模式
    TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);
    // 配置通道1 (直接模式): 上升沿捕获
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x0;
    TIM_ICInit(TIM1, &TIM_ICInitStructure);
    // 配置通道2 (间接模式): 下降沿捕获
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_IndirectTI;
    TIM_ICInit(TIM1, &TIM_ICInitStructure);
    // 5. 配置中断
    // 更新中断 (用于处理溢出)
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    // 捕获中断 (通道1和通道2)
    TIM_ITConfig(TIM1, TIM_IT_CC1 | TIM_IT_CC2, ENABLE);
    // 配置NVIC
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
    NVIC_Init(&NVIC_InitStructure);
    // 6. 使能TIM1
    TIM_Cmd(TIM1, ENABLE);
}
```

#### 3.3.3 中断服务函数

```c
// TIM1更新中断 (处理溢出)
void TIM1_UP_IRQHandler(void) {
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) {
        tim1_overflows++;
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}

// TIM1捕获中断
void TIM1_CC_IRQHandler(void) {
    // 通道1捕获 (上升沿 - 周期开始)
    if (TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET) {
        // 读取周期值 (计数器在上升沿复位，所以CCR1是周期)
        uint32_t capture1 = TIM_GetCapture1(TIM1);
        // 考虑溢出的情况
        if (capture1 == 0) {
            // 如果捕获值为0，则周期是溢出次数 * 0x10000
            pwm_data.period = tim1_overflows * 0x10000;
        } else {
            // 否则周期 = 溢出次数 * 0x10000 + 捕获值
            pwm_data.period = tim1_overflows * 0x10000 + capture1;
        }
        // 重置溢出计数器
        tim1_overflows = 0;
        TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
    }
    // 通道2捕获 (下降沿 - 占空比)
    if (TIM_GetITStatus(TIM1, TIM_IT_CC2) != RESET) {
        // 读取占空比值 (从上升沿到下降沿的时间)
        pwm_data.duty_cycle = TIM_GetCapture2(TIM1);   
        // 计算频率 (72MHz时钟)
        pwm_data.frequency = 72000000.0f / pwm_data.period;
        // 计算占空比百分比
        pwm_data.duty_ratio = (pwm_data.duty_cycle * 100.0f) / pwm_data.period;
        TIM_ClearITPendingBit(TIM1, TIM_IT_CC2);
    }
}
```

#### 3.3.4 获取捕获值

```c
// 获取PWM捕获数据
PWM_CaptureData TIM1_GetPWMCaptureData(void) {
    return pwm_data;
}
```

#### 3.3.5 主函数测试

```c
#include "stm32f10x.h"
#include "tim.h"
#include "systick.h"
#include "usart.h"
#include <stdio.h>

int main(void)
{   
    SysTick_Init();
    USARTx_Init(115200);
    printf("TIM2产生PWM信号 -> TIM1捕获PWM参数\r\n");
    printf("PA0 (PWM输出) 连接到 PA8 (PWM输入)\r\n");
    TIM2_PWM_Init();
    TIM1_PWMCapture_Init();
    /* PWM参数设置 */
    uint32_t periods[] = {1000, 2000, 5000};  // 不同周期值
    uint32_t duties[] = {200, 500, 800};      // 不同占空比值
    uint8_t p_index = 0, d_index = 0;
    printf("开始PWM捕获测试...\r\n");
    while(1)
    {
        TIM2_SetPWM(periods[p_index], duties[d_index]);
        // 打印设置的PWM参数
        printf("设置的PWM参数: 周期=%lu, 占空比=%lu (%.1f%%)\r\n", 
               periods[p_index], duties[d_index],
               (duties[d_index] * 100.0f) / periods[p_index]);
        Delay_ms(1000);
        // 获取捕获的PWM数据
        PWM_CaptureData cap_data = TIM1_GetPWMCaptureData();
        // 打印捕获结果
        printf("捕获的PWM参数:\r\n");
        printf("  周期: %lu 计数\r\n", cap_data.period);
        printf("  占空比: %lu 计数\r\n", cap_data.duty_cycle);
        printf("  频率: %.2f Hz\r\n", cap_data.frequency);
        printf("  占空比: %.2f%%\r\n", cap_data.duty_ratio);
        printf("------------------------------------\r\n");
        // 更新PWM参数索引
        d_index = (d_index + 1) % 3;
        if (d_index == 0) {
            p_index = (p_index + 1) % 3;
        }
        Delay_ms(2000);
    }
}
```

## 4. ATIM常见函数（STD库）

### 4.1 时基配置函数

#### 4.1.1 基础初始化

```c
TIM_TimeBaseInitTypeDef TIM_InitStruct;
TIM_InitStruct.TIM_Prescaler = 72 - 1;        // 预分频值（72MHz/72=1MHz）
TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up; // 递增计数
TIM_InitStruct.TIM_Period = 1000 - 1;         // 自动重装载值（1KHz周期）
TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分频
TIM_InitStruct.TIM_RepetitionCounter = 0;      // 重复计数器（高级功能）

TIM_TimeBaseInit(TIM1, &TIM_InitStruct);      // 初始化时基
```

#### 4.1.2 时钟源选择

```c
// 使用内部时钟（默认）
TIM_InternalClockConfig(TIM1);

// 使用外部时钟（ETR引脚）
TIM_ETRClockMode1Config(TIM1, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0);
```

### 4.2 PWM输出配置（通道1-4）

#### 4.2.1 通道初始化

```c
TIM_OCInitTypeDef PWM_InitStruct;
PWM_InitStruct.TIM_OCMode = TIM_OCMode_PWM1;        // PWM模式1
PWM_InitStruct.TIM_OutputState = TIM_OutputState_Enable; // 使能主输出
PWM_InitStruct.TIM_OutputNState = TIM_OutputNState_Enable; // 使能互补输出
PWM_InitStruct.TIM_Pulse = 500;                    // 初始占空比50%（1000周期）
PWM_InitStruct.TIM_OCPolarity = TIM_OCPolarity_High; // 输出极性
PWM_InitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High; // 互补输出极性
PWM_InitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset; // 空闲状态
PWM_InitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;

TIM_OC1Init(TIM1, &PWM_InitStruct);  // 通道1初始化
TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable); // 使能预装载
```

#### 4.2.2 动态调整占空比

```c
// 设置通道1占空比（0-ARR范围）
TIM_SetCompare1(TIM1, new_pulse_value);
```

### 4.3 死区时间配置

```c
TIM_BDTRInitTypeDef BDTR_InitStruct;
BDTR_InitStruct.TIM_OSSRState = TIM_OSSRState_Enable;
BDTR_InitStruct.TIM_OSSIState = TIM_OSSIState_Enable;
BDTR_InitStruct.TIM_LOCKLevel = TIM_LOCKLevel_1;
BDTR_InitStruct.TIM_DeadTime = 0x54;  // 典型值84ns（1.5MHz下）
BDTR_InitStruct.TIM_Break = TIM_Break_Enable; // 使能刹车功能
BDTR_InitStruct.TIM_BreakPolarity = TIM_BreakPolarity_Low; // 低电平触发刹车
BDTR_InitStruct.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;

TIM_BDTRConfig(TIM1, &BDTR_InitStruct);  // 应用配置
TIM_CtrlPWMOutputs(TIM1, ENABLE);        // 必须使能PWM输出
```

### 4.4 输入捕获配置

#### 4.4.1 通道初始化（通道1为例）

```c
TIM_ICInitTypeDef IC_InitStruct;
IC_InitStruct.TIM_Channel = TIM_Channel_1;
IC_InitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising; // 上升沿捕获
IC_InitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI; // 直接输入
IC_InitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;       // 不分频
IC_InitStruct.TIM_ICFilter = 0x0;                     // 无滤波

TIM_ICInit(TIM1, &IC_InitStruct);
```

#### 4.4.2 读取捕获值

```c
// 获取输入捕获值
uint32_t capture_value = TIM_GetCapture1(TIM1);

// 计算脉冲宽度（单位us）
float pulse_width = (capture_value * 1000000.0) / (SystemCoreClock / (TIM_InitStruct.TIM_Prescaler + 1));
```

### 4.5 编码器接口配置

```c
TIM_EncoderInterfaceConfig(
    TIM1, 
    TIM_EncoderMode_TI12,  // 双通道编码模式
    TIM_ICPolarity_Rising, // 通道1极性
    TIM_ICPolarity_Rising  // 通道2极性
);

// 设置计数器范围
TIM_SetAutoreload(TIM1, 65535);  // 最大计数值

// 使能定时器
TIM_Cmd(TIM1, ENABLE);

// 读取编码器位置
int16_t encoder_pos = TIM_GetCounter(TIM1);
```

### 4.6 中断与DMA配置

#### 4.6.1 中断控制

```c
// 使能更新中断
TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

// 使能捕获中断
TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);

// 配置NVIC
NVIC_InitTypeDef NVIC_InitStruct;
NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn;  // 更新中断
// ...其他参数
NVIC_Init(&NVIC_InitStruct);
```

#### 4.6.2 DMA配置

```c
// 配置PWM DMA传输（通道1）
TIM_DMACmd(TIM1, TIM_DMA_CC1, ENABLE);

// 初始化DMA（以DMA1通道2为例）
DMA_InitTypeDef DMA_InitStruct;
DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&TIM1->CCR1; // 捕获/比较寄存器
DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)pwm_buffer;     // 内存地址
// ...其他DMA参数
DMA_Init(DMA1_Channel2, &DMA_InitStruct);
```

### 4.7 刹车与保护功能

```c
// 紧急刹车（软件触发）
TIM_GenerateEvent(TIM1, TIM_EventSource_Break);

// 配置刹车引脚（PB12）
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
GPIO_Init(GPIOB, &GPIO_InitStruct);

// 配置刹车源（详见TIM_BDTRConfig）
BDTR_InitStruct.TIM_Break = TIM_Break_Enable;
BDTR_InitStruct.TIM_BreakPolarity = TIM_BreakPolarity_Low;
```

| **功能** | **配置函数**                       | **重要参数**                         |
| ------ | ------------------------------ | -------------------------------- |
| PWM输出  | `TIM_OCxInit()`                | `TIM_OCMode`, `TIM_Pulse`        |
| 死区控制   | `TIM_BDTRConfig()`             | `TIM_DeadTime` (0x00-0xFF)       |
| 编码器接口  | `TIM_EncoderInterfaceConfig()` | `TIM_EncoderMode_TIx`            |
| 刹车功能   | `TIM_BDTRConfig()`             | `TIM_Break`, `TIM_BreakPolarity` |
| 重复计数器  | `TIM_SetRepetitionCounter()`   | 0-255                            |
