# 第十二章 RTC介绍及应用

## 1. RTC简介

STM32F407 的实时时钟（RTC）是一个独立的定时器。 STM32 的 RTC 模块拥有一组连续计数的计数器，在相对应的软件配置下，可提供时钟日历的功能。修改计数器的值可以重新设置系统的当前时间和日期。

RTC 模块和时钟配置系统（RCC_BDCR 寄存器）是在后备区域，即在系统复位或从待机模式唤醒后 RTC 的设置和时间维持不变，只要后备区域供电正常，那么 RTC 将可以一直运行。但是在系统复位后，会自动禁止访问后备寄存器和 RTC，以防止对后备区域(BKP)的意外写操作。所以在要设置时间之前，先要取消备份区域（BKP）写保护。

## 2. RTC使用示例

本实验通过 LCD 显示 RTC 时间，并可以通过 usmart 设置 RTC 时间，从而调节时间，或设置 RTC 闹钟，还可以写入或者读取 RTC 后备区域 SRAM。 LED0 闪烁，提示程序运行。

### 2.1 相关参数宏定义

```c
#ifndef __RTC_H
#define __RTC_H

#include "sys.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

//================= 用户配置区 ===================
#define RTC_DEFAULT_YEAR       20
#define RTC_DEFAULT_MONTH      4
#define RTC_DEFAULT_DATE       22
#define RTC_DEFAULT_WEEKDAY    3
#define RTC_BACKUP_FLAG_ADDR   RTC_BKP_DR0   // 备份寄存器0地址
#define RTC_BACKUP_LSE_FLAG    0x5050
#define RTC_BACKUP_LSI_FLAG    0x5051
// =============================================

#ifdef __cplusplus
extern "C" {
#endif

void rtc_write_bkr(uint32_t bkrx, uint32_t data);
uint32_t rtc_read_bkr(uint32_t bkrx);
HAL_StatusTypeDef rtc_set_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t ampm);
HAL_StatusTypeDef rtc_set_date(uint8_t year, uint8_t month, uint8_t date, uint8_t week);
void rtc_get_time(uint8_t *hour, uint8_t *min, uint8_t *sec, uint8_t *ampm);
void rtc_get_date(uint8_t *year, uint8_t *month, uint8_t *date, uint8_t *week);
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day);
uint8_t rtc_init(void);

void rtc_set_alarm(uint8_t week, uint8_t hour, uint8_t min, uint8_t sec);
void rtc_set_wakeup(uint8_t wksel, uint16_t cnt);

extern volatile uint8_t rtc_alarm_flag;
extern volatile uint8_t rtc_wakeup_flag;

#ifdef __cplusplus
}
#endif

#endif /* __RTC_H */
```

### 2.2 RTX备份寄存器操作

```c
/**
 * @brief  写RTC备份寄存器
 */
void rtc_write_bkr(uint32_t bkrx, uint32_t data)
{
    HAL_PWR_EnableBkUpAccess();
    HAL_RTCEx_BKUPWrite(&rtc_handle, bkrx, data);
}

/**
 * @brief  读RTC备份寄存器
 */
uint32_t rtc_read_bkr(uint32_t bkrx)
{
    return HAL_RTCEx_BKUPRead(&rtc_handle, bkrx);
}
```

### 2.3 设置RTC参数

```c
/**
 * @brief  设置RTC时间
 * @note   ampm: RTC_HOURFORMAT12_AM / RTC_HOURFORMAT12_PM / RTC_HOURFORMAT24
 */
HAL_StatusTypeDef rtc_set_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t ampm)
{
    RTC_TimeTypeDef rtc_time_handle = {0};
    rtc_time_handle.Hours = hour;
    rtc_time_handle.Minutes = min;
    rtc_time_handle.Seconds = sec;
    rtc_time_handle.TimeFormat = ampm;
    rtc_time_handle.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    rtc_time_handle.StoreOperation = RTC_STOREOPERATION_RESET;
    return HAL_RTC_SetTime(&rtc_handle, &rtc_time_handle, RTC_FORMAT_BIN);
}

/**
 * @brief  设置RTC日期
 */
HAL_StatusTypeDef rtc_set_date(uint8_t year, uint8_t month, uint8_t date, uint8_t week)
{
    RTC_DateTypeDef rtc_date_handle = {0};
    rtc_date_handle.Date = date;
    rtc_date_handle.Month = month;
    rtc_date_handle.WeekDay = week;
    rtc_date_handle.Year = year;
    return HAL_RTC_SetDate(&rtc_handle, &rtc_date_handle, RTC_FORMAT_BIN);
}

/**
 * @brief  获取RTC时间
 */
void rtc_get_time(uint8_t *hour, uint8_t *min, uint8_t *sec, uint8_t *ampm)
{ 
    RTC_TimeTypeDef rtc_time_handle;
    HAL_RTC_GetTime(&rtc_handle, &rtc_time_handle, RTC_FORMAT_BIN);
    *hour = rtc_time_handle.Hours;
    *min = rtc_time_handle.Minutes;
    *sec = rtc_time_handle.Seconds;
    *ampm = rtc_time_handle.TimeFormat;
}
```

### 2.4 获取RTC参数

```c
/**
 * @brief  获取RTC时间
 */
void rtc_get_time(uint8_t *hour, uint8_t *min, uint8_t *sec, uint8_t *ampm)
{ 
    RTC_TimeTypeDef rtc_time_handle;
    HAL_RTC_GetTime(&rtc_handle, &rtc_time_handle, RTC_FORMAT_BIN);
    *hour = rtc_time_handle.Hours;
    *min = rtc_time_handle.Minutes;
    *sec = rtc_time_handle.Seconds;
    *ampm = rtc_time_handle.TimeFormat;
}

/**
 * @brief  获取RTC日期
 */
void rtc_get_date(uint8_t *year, uint8_t *month, uint8_t *date, uint8_t *week)
{
    RTC_DateTypeDef rtc_date_handle;
    HAL_RTC_GetDate(&rtc_handle, &rtc_date_handle, RTC_FORMAT_BIN);
    *year = rtc_date_handle.Year;
    *month = rtc_date_handle.Month;
    *date = rtc_date_handle.Date;
    *week = rtc_date_handle.WeekDay;
}

/**
 * @brief  计算星期几
 */
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day)
{
    uint16_t temp;
    uint8_t yearH, yearL;
    if(month < 1 || month >12) month = 1;
    yearH = year / 100;
    yearL = year % 100;
    if (yearH > 19) yearL += 100;
    temp = yearL + yearL / 4;
    temp = temp % 7;
    temp = temp + day + table_week[month - 1];
    if ((yearL % 4 == 0) && (month < 3)) temp--;
    temp %= 7;
    if (temp == 0) temp = 7;
    return temp;
}
```

### 2.5 RTC初始化

```c
/**
 * @brief  RTC模块初始化
 * @retval 0-正常 1-HAL失败
 */
uint8_t rtc_init(void)
{
    uint16_t bkpflag = 0;

    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_RTC_ENABLE();

    rtc_handle.Instance = RTC;
    rtc_handle.Init.HourFormat = RTC_HOURFORMAT_24;
    rtc_handle.Init.AsynchPrediv = 0x7F;
    rtc_handle.Init.SynchPrediv = 0xFF;
    rtc_handle.Init.OutPut = RTC_OUTPUT_DISABLE;
    rtc_handle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    rtc_handle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    bkpflag = rtc_read_bkr(0);

    if(HAL_RTC_Init(&rtc_handle) != HAL_OK)
    {
        printf("HAL_RTC_Init失败\r\n");
        return 1;
    }
    if((bkpflag != RTC_BACKUP_LSE_FLAG) && (bkpflag != RTC_BACKUP_LSI_FLAG))
    {
        printf("首次配置RTC时间\r\n");
        rtc_set_time(6,59,59,RTC_HOURFORMAT12_AM);
        rtc_set_date(RTC_DEFAULT_YEAR, RTC_DEFAULT_MONTH, RTC_DEFAULT_DATE, RTC_DEFAULT_WEEKDAY);
    }
    return 0;
}

// ===================== 时钟底层MSP ========================

void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
    uint16_t retry = 200;
    RCC_OscInitTypeDef rcc_osc_init_handle = {0};
    RCC_PeriphCLKInitTypeDef rcc_periphclk_init_handle = {0};
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_RTC_ENABLE();

    RCC->BDCR |= 1 << 0;        // 启动LSE
    while (retry && ((RCC->BDCR & 0x02) == 0))
    {
        retry--;
        delay_ms(5);
    }
    if (retry == 0)
    {
        // LSE失败，改用LSI
        rcc_osc_init_handle.OscillatorType = RCC_OSCILLATORTYPE_LSI;
        rcc_osc_init_handle.LSIState = RCC_LSI_ON;
        rcc_osc_init_handle.PLL.PLLState = RCC_PLL_NONE;
        HAL_RCC_OscConfig(&rcc_osc_init_handle);

        rcc_periphclk_init_handle.PeriphClockSelection = RCC_PERIPHCLK_RTC;
        rcc_periphclk_init_handle.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
        HAL_RCCEx_PeriphCLKConfig(&rcc_periphclk_init_handle);

        rtc_write_bkr(0, RTC_BACKUP_LSI_FLAG);
        printf("使用RTC LSI时钟\r\n");
    }
    else
    {
        rcc_osc_init_handle.OscillatorType = RCC_OSCILLATORTYPE_LSE;
        rcc_osc_init_handle.PLL.PLLState = RCC_PLL_NONE;
        rcc_osc_init_handle.LSEState = RCC_LSE_ON;
        HAL_RCC_OscConfig(&rcc_osc_init_handle);

        rcc_periphclk_init_handle.PeriphClockSelection = RCC_PERIPHCLK_RTC;
        rcc_periphclk_init_handle.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
        HAL_RCCEx_PeriphCLKConfig(&rcc_periphclk_init_handle);

        rtc_write_bkr(0, RTC_BACKUP_LSE_FLAG);
        printf("使用RTC LSE时钟\r\n");
    }
}
```

### 2.6 设置闹钟和唤醒

```c
void rtc_set_alarm(uint8_t week, uint8_t hour, uint8_t min, uint8_t sec)
{
    RTC_AlarmTypeDef rtc_alarm_handle = {0};
    rtc_alarm_handle.AlarmTime.Hours = hour;
    rtc_alarm_handle.AlarmTime.Minutes = min;
    rtc_alarm_handle.AlarmTime.Seconds = sec;
    rtc_alarm_handle.AlarmTime.SubSeconds = 0;
    rtc_alarm_handle.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
    rtc_alarm_handle.AlarmMask = RTC_ALARMMASK_NONE;
    rtc_alarm_handle.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
    rtc_alarm_handle.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
    rtc_alarm_handle.AlarmDateWeekDay = week;
    rtc_alarm_handle.Alarm = RTC_ALARM_A;

    HAL_RTC_SetAlarm_IT(&rtc_handle, &rtc_alarm_handle, RTC_FORMAT_BIN);

    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 1, 2);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}

void rtc_set_wakeup(uint8_t wksel, uint16_t cnt)
{
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&rtc_handle, RTC_FLAG_WUTF);
    HAL_RTCEx_SetWakeUpTimer_IT(&rtc_handle, cnt, wksel);
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 2, 2);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
}
```

### 2.7 中断处理

```c
void RTC_Alarm_IRQHandler(void)
{
    HAL_RTC_AlarmIRQHandler(&rtc_handle);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    rtc_alarm_flag = 1;
}

void RTC_WKUP_IRQHandler(void)
{
    HAL_RTCEx_WakeUpTimerIRQHandler(&rtc_handle); 
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    rtc_wakeup_flag = 1;
}
```

### 2.8 主函数测试

```c
#include "bsp_init.h"
#include "stdio.h"
#include "usmart.h"
#include "rtc.h"

int main(void)
{
    uint8_t hour, min, sec, ampm;
    uint8_t year, month, date, week;
    char tempbuf[40];
    uint32_t last_refresh = 0, last_led = 0;

    bsp_init();
    usmart_dev.init(84);

    uint8_t rtc_status = rtc_init();
    if (rtc_status != 0)
    {
        printf("RTC初始化失败，错误代码：%d\r\n", rtc_status);
    }
    rtc_set_wakeup(RTC_WAKEUPCLOCK_CK_SPRE_16BITS, 0); // 1s周期中断

    while(1)
    {
        uint32_t now = HAL_GetTick();

        // 周期刷新时间
        if(now - last_refresh > 200)
        {
            last_refresh = now;

            rtc_get_time(&hour, &min, &sec, &ampm);
            sprintf(tempbuf, "Time:%02d:%02d:%02d", hour, min, sec);
            LCD_ShowString(30,130,210,16,16,tempbuf);

            rtc_get_date(&year, &month, &date, &week);
            sprintf(tempbuf, "Date:20%02d-%02d-%02d", year, month, date);
            LCD_ShowString(30,150,210,16,16,tempbuf);

            sprintf(tempbuf, "Week:%d", week);
            LCD_ShowString(30,170,210,16,16,tempbuf);
        }

        // LED0翻转
        if(now - last_led > 400)
        {
            last_led = now;
            LED_TOGGLE(LED0_GPIO_Pin);
        }

        // 唤醒标志事件
        if(rtc_wakeup_flag)
        {
            rtc_wakeup_flag = 0;
            // printf("RTC Wakeup!\r\n");
            LED_TOGGLE(LED1_GPIO_Pin);
        }

        // 闹钟标志事件
        if(rtc_alarm_flag)
        {
            rtc_alarm_flag = 0;
            printf("RTC Alarm!\r\n");
        }

        delay_ms(10);
    }
}
```

## 3. RTC常见函数（HAL库）

### 3.1 RTC 初始化与配置

#### 3.1.1  `HAL_RTC_Init()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_RTC_Init(RTC_HandleTypeDef *hrtc)
```

**参数**:

- `hrtc`: RTC 句柄指针

**功能**:  
初始化 RTC 外设，配置时钟源、预分频器等基础参数

**配置结构体**:

```c
typedef struct {
  uint32_t HourFormat;      // 小时格式: RTC_HOURFORMAT_12 / RTC_HOURFORMAT_24
  uint32_t AsynchPrediv;    // 异步预分频值 (7位, 0-127)
  uint32_t SynchPrediv;     // 同步预分频值 (15位, 0-32767)
  uint32_t OutPut;          // 输出选择: RTC_OUTPUT_DISABLE / ALARMA / ALARMB / WAKEUP
  uint32_t OutPutPolarity;  // 输出极性: RTC_OUTPUT_POLARITY_HIGH / LOW
  uint32_t OutPutType;      // 输出类型: RTC_OUTPUT_TYPE_OPENDRAIN / PUSHPULL
} RTC_InitTypeDef;
```

**示例配置**:

```c
RTC_HandleTypeDef hrtc;

hrtc.Instance = RTC;
hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
hrtc.Init.AsynchPrediv = 127;    // 异步分频值
hrtc.Init.SynchPrediv = 255;     // 同步分频值
hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_PUSHPULL;

if (HAL_RTC_Init(&hrtc) != HAL_OK) {
  // 错误处理
}
```

### 3.2 时间设置与获取

#### 3.2.1 `HAL_RTC_SetTime()`

**函数原型**:

```cs
HAL_StatusTypeDef HAL_RTC_SetTime(
    RTC_HandleTypeDef *hrtc,
    RTC_TimeTypeDef *sTime,
    uint32_t Format)
```

**参数**:

- `sTime`: 时间结构体指针

- `Format`: 格式: RTC_FORMAT_BIN (二进制) / RTC_FORMAT_BCD (BCD码)

**时间结构体**:

```c
typedef struct {
  uint8_t Hours;            // 小时 (0-23 或 1-12)
  uint8_t Minutes;          // 分钟 (0-59)
  uint8_t Seconds;          // 秒 (0-59)
  uint8_t TimeFormat;       // 时间格式: RTC_HOURFORMAT12_AM / RTC_HOURFORMAT12_PM
  uint32_t SubSeconds;      // 亚秒值
  uint32_t SecondFraction;  // 亚秒分数值
  uint32_t DayLightSaving;  // 夏令时: RTC_DAYLIGHTSAVING_SUB1H / ADD1H / NONE
  uint32_t StoreOperation;  // 存储操作: RTC_STOREOPERATION_RESET / SET
} RTC_TimeTypeDef;
```

**示例**:

```c
RTC_TimeTypeDef sTime = {0};
sTime.Hours = 14;        // 14:30:00
sTime.Minutes = 30;
sTime.Seconds = 0;
sTime.TimeFormat = RTC_HOURFORMAT12_PM;  // 24小时制下忽略
sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
sTime.StoreOperation = RTC_STOREOPERATION_RESET;

HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
```

#### 3.2.2 `HAL_RTC_GetTime()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_RTC_GetTime(
    RTC_HandleTypeDef *hrtc,
    RTC_TimeTypeDef *sTime,
    uint32_t Format)
```

**功能**: 获取当前时间  
**注意**: 必须先调用 `HAL_RTC_GetTime` 再调用 `HAL_RTC_GetDate`

### 3.3 日期设置与获取

#### 3.3.1 `HAL_RTC_SetDate()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_RTC_SetDate(
    RTC_HandleTypeDef *hrtc,
    RTC_DateTypeDef *sDate,
    uint32_t Format)
```

**日期结构体**:

```c
typedef struct {
  uint8_t WeekDay;  // 星期: RTC_WEEKDAY_MONDAY 到 SUNDAY (1-7)
  uint8_t Month;    // 月份: RTC_MONTH_JANUARY 到 DECEMBER (1-12)
  uint8_t Date;     // 日期: 1-31
  uint8_t Year;     // 年份: 0-99 (代表2000-2099)
} RTC_DateTypeDef;
```

**示例**:

```c
RTC_DateTypeDef sDate = {0};
sDate.WeekDay = RTC_WEEKDAY_MONDAY;
sDate.Month = RTC_MONTH_OCTOBER;
sDate.Date = 15;
sDate.Year = 23;  // 2023年

HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
```

#### 3.3.2 `HAL_RTC_GetDate()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_RTC_GetDate(
    RTC_HandleTypeDef *hrtc,
    RTC_DateTypeDef *sDate,
    uint32_t Format)
```

### 3.4 闹钟功能

#### 3.4.1 `HAL_RTC_SetAlarm()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_RTC_SetAlarm(
    RTC_HandleTypeDef *hrtc,
    RTC_AlarmTypeDef *sAlarm,
    uint32_t Alarm,
    uint32_t Format)
```

**参数**:

- `Alarm`: 闹钟选择: RTC_ALARM_A / RTC_ALARM_B

**闹钟结构体**:

```c
typedef struct {
  RTC_AlarmTypeDef Alarm;                // 闹钟A或B
  uint32_t AlarmMask;                    // 闹钟掩码
  uint32_t AlarmSubSecondMask;           // 亚秒掩码
  uint32_t AlarmDateWeekDaySel;          // 日期/星期选择
  uint8_t AlarmDateWeekDay;              // 日期(1-31)或星期(1-7)
  RTC_TimeTypeDef AlarmTime;             // 闹钟时间
  uint32_t AlarmSubSecondValue;          // 亚秒值
  uint32_t AlarmSubSecondFractionValue;  // 亚秒分数值
} RTC_AlarmTypeDef;
```

示例 (设置闹钟A在每天14:35:00触发):

```c
RTC_AlarmTypeDef sAlarm = {0};
sAlarm.Alarm = RTC_ALARM_A;
sAlarm.AlarmTime.Hours = 14;
sAlarm.AlarmTime.Minutes = 35;
sAlarm.AlarmTime.Seconds = 0;
sAlarm.AlarmTime.TimeFormat = RTC_HOURFORMAT12_PM;
sAlarm.AlarmMask = RTC_ALARMMASK_NONE;  // 所有字段都匹配
sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
sAlarm.AlarmDateWeekDay = 1;  // 日期设为1 (当使用日期时)
sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;

HAL_RTC_SetAlarm(&hrtc, &sAlarm, RTC_ALARM_A, RTC_FORMAT_BIN);
```

#### 3.4.2 闹钟中断控制

```c
// 使能闹钟中断
HAL_RTC_EnableAlarm(&hrtc, RTC_ALARM_A);

// 禁用闹钟中断
HAL_RTC_DisableAlarm(&hrtc, RTC_ALARM_A);
```

#### 3.4.3 闹钟中断回调函数

```c
// 用户需重写此函数
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
  if (hrtc->Instance == RTC) {
    // 处理闹钟事件
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
  }
}
```

### 3.5 唤醒定时器

#### 3.5.1 `HAL_RTCEx_SetWakeUpTimer()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_RTCEx_SetWakeUpTimer(
    RTC_HandleTypeDef *hrtc,
    uint32_t WakeUpCounter,
    uint32_t WakeUpClock)
```

**参数**:

- `WakeUpCounter`: 唤醒计数器值 (0-0xFFFF)

- `WakeUpClock`: 时钟源选择:
  
  - RTC_WAKEUPCLOCK_RTCCLK_DIV16
  
  - RTC_WAKEUPCLOCK_RTCCLK_DIV8
  
  - RTC_WAKEUPCLOCK_RTCCLK_DIV4
  
  - RTC_WAKEUPCLOCK_RTCCLK_DIV2
  
  - RTC_WAKEUPCLOCK_CK_SPRE_16BITS
  
  - RTC_WAKEUPCLOCK_CK_SPRE_17BITS

**示例** (每1秒唤醒):

```c
// 使用1Hz时钟 (RTC时钟通常为32768Hz)
HAL_RTCEx_SetWakeUpTimer(&hrtc, 0, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
```

#### 3.5.2 唤醒定时器控制

```c
// 使能唤醒定时器中断
HAL_RTCEx_EnableWakeUpTimer(&hrtc);

// 禁用唤醒定时器中断
HAL_RTCEx_DisableWakeUpTimer(&hrtc);
```

#### 3.5.3 唤醒中断回调函数

```c
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
  // 唤醒事件处理
}
```

### 3.6 备份寄存器操作

#### 3.6.1 `HAL_RTCEx_BKUPWrite()`

**函数原型**:

```c
void HAL_RTCEx_BKUPWrite(
    RTC_HandleTypeDef *hrtc,
    uint32_t BackupRegister,
    uint32_t Data)
```

**参数**:

- `BackupRegister`: 备份寄存器 (RTC_BKP_DR0 到 RTC_BKP_DR19)

#### 3.6.2 `HAL_RTCEx_BKUPRead()`

**函数原型**:

```c
uint32_t HAL_RTCEx_BKUPRead(
    RTC_HandleTypeDef *hrtc,
    uint32_t BackupRegister)
```

**示例**:

```c
// 写入备份寄存器
HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0x1234);

// 读取备份寄存器
uint32_t data = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0);
```

### 3.7 时间戳功能

#### 3.7.1 `HAL_RTCEx_SetTimeStamp()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_RTCEx_SetTimeStamp(
    RTC_HandleTypeDef *hrtc,
    uint32_t TimeStampEdge,
    uint32_t RTC_TimeStampPin)
```

**参数**:

- `TimeStampEdge`: 时间戳边沿 RTC_TIMESTAMPEDGE_RISING / FALLING

- `RTC_TimeStampPin`: 时间戳引脚 RTC_TIMESTAMPPIN_DEFAULT

#### 3.7.2 时间戳回调函数

```c
void HAL_RTCEx_TimeStampEventCallback(RTC_HandleTypeDef *hrtc)
{
  RTC_TimeTypeDef sTimeStamp;
  RTC_DateTypeDef sDateStamp;

  // 获取时间戳
  HAL_RTC_GetTime(hrtc, &sTimeStamp, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(hrtc, &sDateStamp, RTC_FORMAT_BIN);

  // 处理时间戳
}
```

### 3.8 RTC校准

#### 3.8.1 `HAL_RTCEx_SetSynchroShift()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_RTCEx_SetSynchroShift(
    RTC_HandleTypeDef *hrtc,
    uint32_t ShiftAdd1S,
    uint32_t ShiftSubFS)
```

**功能**: 用于RTC时钟校准

### 3.9 低功耗处理

#### 3.9.1 进入低功耗模式前的处理

```c
void BeforeEnterStopMode(void)
{
  // 禁用RTC中断
  HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);
  HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

  // 清除RTC标志位
  __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);
  __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_ALRAF);
}
```

#### 3.9.2 退出低功耗模式后的处理

```c
void AfterExitStopMode(void)
{
  // 重新启用RTC中断
  HAL_RTC_ActivateAlarm(&hrtc, RTC_ALARM_A);
  HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, wakeup_counter, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
}
```

---


