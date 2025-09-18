# 第十七章 PWR介绍及应用

## 1. 可编程电压检测器PVD

STM32提供了可编程电压检测器 PVD，它也是实时检测 VDD 的电压，当检测到电压低于编程的 VPVD 阈值时，会向内核产生一个 PVD 中断 (EXTI16 线中断) 以使内核在复位前进行紧急处理。该电压阈值可通过电源控制寄存器 PWR_CSR 设置。

![屏幕截图 2025-07-10 165412.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/07/10-17-10-40-屏幕截图%202025-07-10%20165412.png)

## 2. STM32 的功耗模式

按功耗由高到低排列， STM32 具有运行、睡眠、停止和待机四种工作模式。上电复位后 STM32处于运行状态时，当内核不需要继续运行，就可以选择进入后面的三种低功耗模式降低功耗，这三种模式中，电源消耗不同、唤醒时间不同、唤醒源不同，用户需要根据应用需求，选择最佳的低功耗模式。

![屏幕截图 2025-07-10 171522.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/07/10-17-15-32-屏幕截图%202025-07-10%20171522.png)

## 3. PWR使用示例

### 3.1 睡眠模式

#### 3.1.1 外部中断配置

```c
#include "key.h"

void BSP_KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    // 配置PA0为浮空输入（外部已有上拉电阻）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
```

#### 3.1.2 睡眠模式测试

```c
#include "stm32f10x.h"
#include "systick.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include <stdio.h>

volatile uint8_t key_wakeup_flag = 0;  // 按键唤醒标志

int main(void)
{
    BSP_LED_Init();
    BSP_KEY_Init();
    SysTick_Init();
    USARTx_Init(115200);

    printf("System Started.\r\n");
    printf("Press KEY to enter sleep mode...\r\n");

    // 初始状态：不进入睡眠
    uint8_t sleep_enabled = 0;

    while(1)
    {
        if(key_wakeup_flag)
        {
            key_wakeup_flag = 0;

            if(!sleep_enabled)
            {
                // 第一次按键：准备进入睡眠
                LED_ON(GREEN_LED_Pin);
                printf("Preparing to sleep. Press KEY again to wake up.\r\n");
                Delay_ms(1000);
                LED_OFF(GREEN_LED_Pin);

                sleep_enabled = 1;  // 允许进入睡眠
            }
            else
            {
                // 唤醒后操作
                LED_ON(GREEN_LED_Pin);
                printf("Waked up by KEY!\r\n");
                printf("Press KEY to enter sleep mode...\r\n");
                Delay_ms(1000);
                LED_OFF(GREEN_LED_Pin);

                sleep_enabled = 0;  // 禁止再次立即进入睡眠
            }
        }

        if(sleep_enabled)
        {
            // 进入睡眠模式
            printf("Entering sleep mode...\r\n");

            // 确保串口发送完成
            while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

            // 清除唤醒标志
            PWR->CR |= PWR_CR_CWUF;

            // 进入睡眠模式
            __WFI();

            // 唤醒后继续执行
            printf("Woke up from sleep!\r\n");
            sleep_enabled = 0;
        }
    }
}

void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line0);
        key_wakeup_flag = 1;
    }
}
```

### 3.2 停止模式

#### 3.2.1 外部中断配置

```c
#include "key.h"
#include "led.h"

void Key_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = KEY1_EXTI_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = KEY2_EXTI_IRQn;
    NVIC_Init(&NVIC_InitStructure);
}

void Key_GPIOandEXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK, ENABLE);
    /* KEY1 引脚配置 */
    GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
    GPIO_EXTILineConfig(KEY1_EXTI_PORTSOURCE, KEY1_EXTI_PINSOURCE);
    EXTI_InitStructure.EXTI_Line = KEY1_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    /* KEY2 引脚配置 */
    GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);
    GPIO_EXTILineConfig(KEY2_EXTI_PORTSOURCE, KEY2_EXTI_PINSOURCE);
    EXTI_InitStructure.EXTI_Line = KEY2_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

void KEY_EXTI_Init(void)
{
    Key_NVIC_Init();
    Key_GPIOandEXTI_Init();
}

void KEY1_IRQHandler(void)
{
    if(EXTI_GetITStatus(KEY1_EXTI_LINE) != RESET)
    {
        LED_ON(BLUE_LED_Pin);
        EXTI_ClearITPendingBit(KEY1_EXTI_LINE);
    }
}

void KEY2_IRQHandler(void)
{
    if(EXTI_GetITStatus(KEY2_EXTI_LINE) != RESET)
    {
        LED_ON(RED_LED_Pin);
        EXTI_ClearITPendingBit(KEY2_EXTI_LINE);
    }
}
```

#### 3.2.2 重配时钟树

```c
// 停止模式唤醒后使用HSI时钟，我们需要切换至HSE
void STOP_SYSConfig(void)
{
    RCC_HSEConfig(RCC_HSE_ON);
    while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);
    RCC_PLLCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while(RCC_GetSYSCLKSource() != 0x08); // 系统时钟源切换到PLL
}
```

#### 3.2.3 主函数测试

```c
int main(void)
{
    RCC_ClocksTypeDef clock_status_wakeup;
    RCC_ClocksTypeDef clock_status_config;
    uint8_t clock_source_wakeup;
    uint8_t clock_source_config;

    BSP_LED_Init();
    KEY_EXTI_Init();
    SysTick_Init();
    USARTx_Init(115200);

    while(1)
    {
        printf("绿灯亮，系统正常工作中...\r\n");
        LED_ON(GREEN_LED_Pin);
        Delay_ms(500);
        LED_OFF(GREEN_LED_Pin);
        Delay_ms(500);
        printf("准备进入停止模式...\r\n");
        // 确保所有串口数据已发送
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);    
        PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

        // 1. 获取唤醒时的时钟状态 (使用正确的结构体)
        clock_source_wakeup = RCC_GetSYSCLKSource();
        RCC_GetClocksFreq(&clock_status_wakeup);

        // 2. 重新配置系统时钟
        STOP_SYSConfig();

        // 3. 重新初始化关键外设
        SysTick_Init();          // 系统时钟改变后需重配SysTick
        USARTx_Init(115200);     // 必须重新初始化USART

        // 4. 获取重新配置后的时钟状态
        clock_source_config = RCC_GetSYSCLKSource();
        RCC_GetClocksFreq(&clock_status_config);

        // 5. 打印时钟状态
        printf("\r\n--- 唤醒后时钟状态 ---\r\n");
        printf("时钟源: %s\r\n", (clock_source_wakeup == 0) ? "HSI" : "PLL");
        printf("SYSCLK: %d Hz\r\n", clock_status_wakeup.SYSCLK_Frequency);
        printf("HCLK  : %d Hz\r\n", clock_status_wakeup.HCLK_Frequency);
        printf("PCLK1 : %d Hz\r\n", clock_status_wakeup.PCLK1_Frequency);
        printf("PCLK2 : %d Hz\r\n", clock_status_wakeup.PCLK2_Frequency);

        printf("\r\n--- 重配后时钟状态 ---\r\n");
        printf("时钟源: %s\r\n", (clock_source_config == 8) ? "PLL" : "HSI");
        printf("SYSCLK: %d Hz\r\n", clock_status_config.SYSCLK_Frequency);
        printf("HCLK  : %d Hz\r\n", clock_status_config.HCLK_Frequency);
        printf("PCLK1 : %d Hz\r\n", clock_status_config.PCLK1_Frequency);
        printf("PCLK2 : %d Hz\r\n", clock_status_config.PCLK2_Frequency);

        printf("\r\n测试完成，等待下一次循环...\r\n\r\n");
        Delay_ms(2000);
    }
}
```

### 3.3 待机模式

#### 3.3.1 KEY2长按

```c
// KEY2长按
uint8_t KEY2_LongPress(void)
{
    uint8_t CNT_Down = 0;
    uint8_t CNT_Up = 0;
    while(1)
    {
        if(GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == SET)
        {
            CNT_Down++;
            CNT_Up = 0;
            if(CNT_Down >= 100)
            {
                return 1;
            }
        }
        else
        {
            CNT_Up++;
            if(CNT_Up >= 5)
            {
                return 0;
            }
        }
    }
}
```

#### 3.3.2 主函数测试

```c
#include "stm32f10x.h"
#include "systick.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include <stdio.h>

int main(void)
{    
    BSP_LED_Init();
    KEY_GPIO_Init();
    SysTick_Init();
    USARTx_Init(115200);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    if(PWR_GetFlagStatus(PWR_FLAG_WU) == SET)
    {
        LED_ON(BLUE_LED_Pin); // 待机模式唤醒
        printf("待机模式唤醒复位\r\n");
    }
    else
    {
        LED_ON(GREEN_LED_Pin);
        printf("正常启动\r\n");
    }
    while(1)
    {
        if(KEY2_LongPress())
        {
            printf("长按KEY2进入待机模式，按KEY1唤醒\r\n");
            LED_ON(RED_LED_Pin);
            Delay_ms(1000);
            PWR_ClearFlag(PWR_FLAG_WU);
            PWR_WakeUpPinCmd(ENABLE);
            PWR_EnterSTANDBYMode();
        }
    }   
}
```

### 3.4 PVD电源监控

使用 PVD 监控 STM32 芯片的 VDD 引脚，当监测到供电电压低于阈值时会产生 PVD 中断，系统进入中断服务函数进入紧急处理过程。

```c
#include "stm32f10x.h"
#include "systick.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include <stdio.h>

void PVD_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    EXTI_ClearITPendingBit(EXTI_Line16);
    EXTI_InitStructure.EXTI_Line = EXTI_Line16;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    PWR_PVDLevelConfig(PWR_PVDLevel_2V6); // VDD电压低于2.6V时触发中断
    PWR_PVDCmd(ENABLE);
}

void PVD_IRQHandler(void)
{
    if(PWR_GetFlagStatus(PWR_FLAG_PVDO) == SET)
    {
        LED_ON(RED_LED_Pin);
    } 
    EXTI_ClearITPendingBit(EXTI_Line16);
}

int main(void)
{   
    PVD_Config();
    BSP_LED_Init();
    SysTick_Init();
    while(1);
}
```

我们实测 PVD 阈值等级为“PWR_PVDLevel_2V6”时，当可调电源电压降至 4.1V 时，板子亮红灯，此时的“3.3V”电源引脚的实测电压为 2.55V

## 4. 电源管理常见函数（STD库）

### 4.1 核心配置函数

```c
// 进入睡眠模式
void PWR_EnterSleepMode(FunctionalState SleepOnExit);

// 进入停止模式
void PWR_EnterSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry);

// 进入待机模式
void PWR_EnterSTANDBYMode(void);
```

**参数说明**：

- `SleepOnExit`：`ENABLE`表示退出中断后返回睡眠

- `PWR_Regulator`：
  
  - `PWR_Regulator_ON`：稳压器正常模式
  
  - `PWR_Regulator_LowPower`：稳压器低功耗模式

- `PWR_STOPEntry`：
  
  - `PWR_STOPEntry_WFI`：等待中断唤醒
  
  - `PWR_STOPEntry_WFE`：等待事件唤醒

### 4.2 模式切换完整流程

#### 4.2.1 睡眠模式配置

```c
void Enter_SleepMode(void) {
    // 1. 配置唤醒源（如EXTI中断）
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&EXTI_InitStructure);

    // 2. 进入睡眠
    PWR_EnterSleepMode(ENABLE);  // 退出中断后保持睡眠

    // 唤醒后执行此处代码...
}
```

#### 4.2.2 停止模式配置

```c
void Enter_StopMode(void) {
    // 1. 使能PWR时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    // 2. 配置唤醒源（如PA0唤醒引脚）
    PWR_WakeUpPinCmd(PWR_WakeUpPin_1, ENABLE);

    // 3. 进入停止模式（低功耗稳压器+WFI唤醒）
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

    // 4. 唤醒后恢复时钟（停止模式下时钟切换为HSI）
    SystemInit();  // 重新初始化时钟

    // 5. 重新配置外设
    GPIO_Init();   // 重新初始化GPIO等
}
```

#### 4.2.3 待机模式配置

```c
void Enter_StandbyMode(void) {
    // 1. 使能PWR时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    // 2. 清除唤醒标志
    PWR_ClearFlag(PWR_FLAG_WU);

    // 3. 配置RTC唤醒（可选）
    RTC_SetWakeUpCounter(0xFFFF);  // 设置唤醒间隔

    // 4. 进入待机模式
    PWR_EnterSTANDBYMode();

    // 唤醒后系统复位，从main()重新开始执行
}
```

### 4.3 唤醒源配置

#### 4.3.1 唤醒引脚配置

```c
// 使能WKUP引脚（PA0）
PWR_WakeUpPinCmd(PWR_WakeUpPin_1, ENABLE);

// 配置引脚极性（上升沿唤醒）
PWR_WakeUpPinPolarityConfig(PWR_WakeUpPin_1, PWR_WakeUpPinPolarity_High);
```

#### 4.3.2 RTC唤醒配置

```c
// 使能RTC时钟
RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
PWR_BackupAccessCmd(ENABLE);

// 配置RTC唤醒间隔（单位：RTC时钟周期）
RTC_WakeUpCmd(ENABLE);
RTC_SetWakeUpCounter(3276);  // 约1秒（假设RTC=32.768kHz）
```

### 4.4 备份域操作

#### 4.4.1 备份寄存器访问

```c
// 使能备份访问
PWR_BackupAccessCmd(ENABLE);

// 写入备份寄存器
BKP_WriteBackupRegister(BKP_DR1, 0x1234);

// 读取备份寄存器
uint16_t data = BKP_ReadBackupRegister(BKP_DR1);
```

#### 4.4.2 RTC配置

```c
// 初始化RTC
RTC_InitTypeDef RTC_InitStruct;
RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;
RTC_InitStruct.RTC_AsynchPrediv = 127;   // 异步分频
RTC_InitStruct.RTC_SynchPrediv = 255;    // 同步分频
RTC_Init(&RTC_InitStruct);

// 设置时间
RTC_TimeTypeDef TimeStruct;
TimeStruct.RTC_Hours = 12;
TimeStruct.RTC_Minutes = 30;
RTC_SetTime(RTC_Format_BIN, &TimeStruct);
```

### 4.5 电压监测配置

```c
// 配置PVD阈值
PWR_PVDLevelConfig(PWR_PVDLevel_2V9);  // 2.9V阈值

// 使能PVD中断
PWR_PVDCmd(ENABLE);
PWR_ITConfig(PWR_IT_PVD, ENABLE);

// 中断服务函数
void PVD_IRQHandler(void) {
    if(PWR_GetFlagStatus(PWR_FLAG_PVDO)) {
        // 电压低于阈值，执行紧急保存
        PWR_ClearFlag(PWR_FLAG_PVDO);
    }
}
```

| **模式** | **功耗** | **唤醒方式**          | **保持状态**        |
| ------ | ------ | ----------------- | --------------- |
| 睡眠模式   | ~1.5mA | 任意中断/事件           | CPU暂停，外设保持      |
| 停止模式   | ~20μA  | 外部中断/RTC/唤醒引脚     | SRAM/寄存器保持      |
| 待机模式   | ~2μA   | 唤醒引脚/RTC/复位/NWK引脚 | 仅备份域保持（需VBAT供电） |
