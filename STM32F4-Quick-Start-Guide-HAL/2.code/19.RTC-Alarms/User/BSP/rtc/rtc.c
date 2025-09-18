#include "rtc.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include <stdio.h>

RTC_HandleTypeDef rtc_handle;

volatile uint8_t rtc_alarm_flag = 0;
volatile uint8_t rtc_wakeup_flag = 0;

// 月份补偿表
static const uint8_t table_week[12] = {0,3,3,6,1,4,6,2,5,0,3,5};

// ============================= 基础操作 ==========================

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

// ========================== RTC基本功能 =============================

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

// ==================== 闹钟和周期唤醒 ========================

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

// =================== 中断及回调实现 ===========================

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
