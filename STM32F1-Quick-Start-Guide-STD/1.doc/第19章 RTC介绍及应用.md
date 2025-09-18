# 第十九章 RTC介绍及应用

## 1. RTC简介

STM32 的 RTC 外设（Real Time Clock），实质是一个掉电后还继续运行的定时器。从定时器的角度来说，相对于通用定时器 TIM 外设，它十分简单，只有很纯粹的计时和触发中断的功能；但从掉电还继续运行的角度来说，它却是 STM32 中唯一一个具有如此强大功能的外设。所以 RTC外设的复杂之处并不在于它的定时功能，而在于它掉电还继续运行的特性。

以上所说的掉电，是指主电源 VDD 断开的情况，为了 RTC 外设掉电继续运行，必须接上锂电池给 STM32 的 RTC、备份发卡通过 VBAT 引脚供电。当主电源 VDD 有效时，由 VDD 给 RTC 外设供电；而当 VDD 掉电后，由 VBAT 给 RTC 外设供电。但无论由什么电源供电， RTC 中的数据都保存在属于 RTC 的备份域中，若主电源 VDD 和 VBAT 都掉电，那么备份域中保存的所有数据将丢失。备份域除了 RTC 模块的寄存器，还有 42 个 16 位的寄存器可以在 VDD 掉电的情况下保存用户程序的数据，系统复位或电源复位时，这些数据也不会被复位。

从 RTC 的定时器特性来说，它是一个 32 位的计数器，只能向上计数。它使用的时钟源有三种，分别为高速外部时钟的 128 分频（HSE/128）、低速内部时钟 LSI 以及低速外部时钟 LSE；使 HSE分频时钟或 LSI 的话，在主电源 VDD 掉电的情况下，这两个时钟来源都会受到影响，因此没法保证 RTC 正常工作。因此 RTC 一般使用低速外部时钟 LSE，在设计中，频率通常为实时时钟模块中常用的 32.768KHz，这是因为 32768 = 215，分频容易实现，所以它被广泛应用到 RTC 模块。在主电源 VDD 有效的情况下 (待机)， RTC 还可以配置闹钟事件使 STM32 退出待机模式。

## 2. RTC使用示例

从上面的分析可知， RTC 外设是个连续计数的计数器，利用它提供的时间戳，可通过程序转换输出实时时钟和日历的功能，修改计数器的值则可以重新设置系统当前的时间和日期。由于它的时钟配置系统 (RCC_BDCR 寄存器) 是在备份域，在系统复位或从待机模式唤醒后 RTC 的设置维持不变，而且使用备份域电源可以使 RTC 计时器在主电源关掉的情况下仍然运行，保证时间的正确

### 2.1 RTC结构体自定义

```c
#ifndef __RTC_H__
#define __RTC_H__

#include "stm32f10x.h"

// 自定义日期时间结构体（STD库中没有RTC_DateTypeDef/RTC_TimeTypeDef）
typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t day;
} Custom_DateTypeDef;

typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
}Custom_TimeTypeDef;

void RTC_Config(void);
void ConvertTimestamp(uint32_t timestamp, Custom_DateTypeDef* date, Custom_TimeTypeDef* time);

#endif

```

### 2.2 RTC初始化

```c
// RTC初始化
void RTC_Config(void) {
    // 启用PWR和BKP时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    // 允许访问BKP区域
    PWR_BackupAccessCmd(ENABLE);
    // 复位备份区域
    BKP_DeInit();
    // 启用LSE时钟（32.768KHz）
    RCC_LSEConfig(RCC_LSE_ON);
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
    // 选择LSE作为RTC时钟源
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    RCC_RTCCLKCmd(ENABLE);
    
    // 等待RTC寄存器同步
    RTC_WaitForSynchro();
    
    // 等待上一次操作完成
    RTC_WaitForLastTask();
    
    // 设置RTC分频器：32768Hz/(32767+1) = 1Hz
    RTC_SetPrescaler(32767);
    RTC_WaitForLastTask();
    
    // 检查是否首次配置
    if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5) {
        RTC_SetCounter(0);  // 从1970-01-01 00:00:00开始计数
        RTC_WaitForLastTask();
        
        // 设置备份寄存器标志
        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
    }
}
```

### 2.3 时间戳转换

```c
// 将Unix时间戳转换为日期时间
void ConvertTimestamp(uint32_t timestamp, Custom_DateTypeDef* date, Custom_TimeTypeDef* time) {
    // 简化的转换逻辑（实际项目需使用完整算法）
    time->second = timestamp % 60;
    timestamp /= 60;
    time->minute = timestamp % 60;
    timestamp /= 60;
    time->hour = timestamp % 24;
    timestamp /= 24;
    
    // 简化日期计算（忽略闰年等细节）
    date->year = 1970 + timestamp / 365;
    timestamp %= 365;
    date->month = 1 + timestamp / 30;
    date->day = 1 + timestamp % 30;
}
```

### 2.4 主函数测试

```c
#include "stm32f10x.h"
#include "rtc.h"
#include "systick.h"
#include "usart.h"
#include <stdio.h>

int main(void) {
    Custom_DateTypeDef date;
    Custom_TimeTypeDef time;
    // 系统时钟设置（使用外部8MHz晶振）
    SystemInit();
		SysTick_Init();
    // 初始化外设
    USARTx_Init(115200);
    RTC_Config();
    printf("STM32F103 RTC Calendar Demo\r\n");
    while(1) {
        // 获取当前时间戳
        uint32_t timestamp = RTC_GetCounter();
        // 转换为日期时间
        ConvertTimestamp(timestamp, &date, &time);
        // 打印日期时间
        printf("Date: %04d-%02d-%02d  Time: %02d:%02d:%02d\r\n",
               date.year, date.month, date.day,
               time.hour, time.minute, time.second);
       
			Delay_ms(1000);
    }
}


```

## 3. RTC常见函数（STD库）

### 3.1 RTC 初始化流程

#### 3.1.1 备份域访问使能

```c
// 必须步骤：解除备份寄存器写保护
PWR_BackupAccessCmd(ENABLE);
RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
```

#### 3.1.2 复位备份域

```c
// 首次配置时复位备份域
BKP_DeInit();
```

#### 3.1.3 时钟源选择

```c
// 使用LSE（外部32.768kHz晶振）
RCC_LSEConfig(RCC_LSE_ON);
while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET); // 等待LSE就绪

// 或使用LSI（内部RC）
RCC_LSICmd(ENABLE);
while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

// 选择RTC时钟源
RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);  // 选择LSE
RCC_RTCCLKCmd(ENABLE);                   // 使能RTC时钟
```

#### 3.1.4 等待时钟同步

```c
RTC_WaitForSynchro();  // 等待RTC寄存器同步
RTC_WaitForLastTask(); // 等待上一次操作完成
```

### 3.2 时间日期设置

#### 3.2.1 配置时间格式

```c
// 设置24小时制
RTC_InitTypeDef RTC_InitStruct;
RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;
RTC_Init(&RTC_InitStruct);
RTC_WaitForLastTask();
```

#### 3.2.2 设置日期

```c
RTC_DateTypeDef DateStruct;
DateStruct.RTC_Year = 23;    // 2023年
DateStruct.RTC_Month = RTC_Month_October; // 10月
DateStruct.RTC_Date = 15;    // 15日
DateStruct.RTC_WeekDay = RTC_Weekday_Sunday; // 周日（自动计算）

RTC_SetDate(RTC_Format_BIN, &DateStruct);
RTC_WaitForLastTask();
```

#### 3.2.3 设置时间

```c
RTC_TimeTypeDef TimeStruct;
TimeStruct.RTC_Hours = 14;    // 14时
TimeStruct.RTC_Minutes = 30;  // 30分
TimeStruct.RTC_Seconds = 0;   // 0秒

RTC_SetTime(RTC_Format_BIN, &TimeStruct);
RTC_WaitForLastTask();
```

### 3.3 闹钟配置

#### 3.3.1 单次闹钟

```c
RTC_AlarmTypeDef AlarmStruct;
AlarmStruct.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay; // 忽略日期
AlarmStruct.RTC_AlarmTime.RTC_Hours = 7;
AlarmStruct.RTC_AlarmTime.RTC_Minutes = 0;
AlarmStruct.RTC_Alarm = RTC_Alarm_A;  // 闹钟A

RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &AlarmStruct);
RTC_WaitForLastTask();
```

#### 3.3.2 每日闹钟

```c
AlarmStruct.RTC_AlarmMask = RTC_AlarmMask_All; // 每日同一时间
```

#### 3.3.3 使能闹钟中断

```c
RTC_ITConfig(RTC_IT_ALRA, ENABLE);  // 使能闹钟A中断
RTC_WaitForLastTask();

// 配置NVIC
NVIC_InitTypeDef NVIC_InitStruct;
NVIC_InitStruct.NVIC_IRQChannel = RTC_Alarm_IRQn;
NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
NVIC_Init(&NVIC_InitStruct);
```

### 3.4 中断服务函数

#### 3.4.1 闹钟中断

```c
void RTC_Alarm_IRQHandler(void) {
    if(RTC_GetITStatus(RTC_IT_ALRA) != RESET) {
        // 处理闹钟事件（如唤醒系统）
        RTC_ClearITPendingBit(RTC_IT_ALRA);
        EXTI_ClearITPendingBit(EXTI_Line17); // 清除挂起位
    }
}
```

#### 3.4.2 秒中断

```c
// 使能秒中断
RTC_ITConfig(RTC_IT_SEC, ENABLE);

void RTC_IRQHandler(void) {
    if(RTC_GetITStatus(RTC_IT_SEC) != RESET) {
        // 每秒执行一次（如更新显示）
        RTC_ClearITPendingBit(RTC_IT_SEC);
    }
}
```

### 3.5 时间戳功能

#### 3.5.1 配置时间戳

```c
// 使能时间戳（上升沿触发）
RTC_TimeStampCmd(RTC_TimeStampEdge_Rising, ENABLE);
RTC_WaitForLastTask();

// 使能中断
RTC_ITConfig(RTC_IT_TS, ENABLE);
```

#### 3.5.2 读取时间戳

```c
void RTC_IRQHandler(void) {
    if(RTC_GetITStatus(RTC_IT_TS) != RESET) {
        // 获取时间戳
        RTC_TimeTypeDef ts_time;
        RTC_DateTypeDef ts_date;
        RTC_GetTimeStamp(RTC_Format_BIN, &ts_time, &ts_date);
        
        RTC_ClearITPendingBit(RTC_IT_TS);
    }
}
```

### 3.6 校准与备份

#### 3.6.1 时钟校准

```c
// 校准时钟（-487ppm ~ +488ppm）
RTC_CoarseCalibConfig(RTC_CalibSign_Positive, 100); // +100ppm

// 或使用精确校准
RTC_SmoothCalibConfig(RTC_SmoothCalibPeriod_32sec, 
                      RTC_SmoothCalibPlusPulses_Set, 
                      100); // 增加100个脉冲
```

#### 3.6.2 数据备份与恢复

```c
// 备份RTC计数器值
uint32_t rtc_counter = RTC_GetCounter();
BKP_WriteBackupRegister(BKP_DR1, rtc_counter);

// 系统复位后恢复
if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) {
    uint32_t backup = BKP_ReadBackupRegister(BKP_DR1);
    RTC_SetCounter(backup);
    RTC_WaitForLastTask();
}
```

### 3.7 数据读取函数

#### 3.7.1 读取当前时间

```c
void RTC_GetCurrentTime(char *buffer) {
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
    
    RTC_GetTime(RTC_Format_BIN, &time);
    RTC_GetDate(RTC_Format_BIN, &date);
    
    sprintf(buffer, "20%02d-%02d-%02d %02d:%02d:%02d", 
            date.RTC_Year, date.RTC_Month, date.RTC_Date,
            time.RTC_Hours, time.RTC_Minutes, time.RTC_Seconds);
}
```

#### 3.7.2 获取时间戳

```c
uint32_t RTC_GetTimestamp(void) {
    return RTC_GetCounter();
}
```

| **参数** | **LSE（外部晶振）**   | **LSI（内部RC）**     |
| ------ | --------------- | ----------------- |
| 精度     | ±5ppm（年误差2.6分钟） | ±500ppm（月误差1.3小时） |
| 功耗     | 0.6μA           | 0.4μA             |
| 成本     | 需要外部晶振          | 全集成               |
| 温度稳定性  | 好（±10ppm）       | 差（±2500ppm）       |
| 推荐应用   | 需要精确计时          | 低成本/时间不敏感         |
