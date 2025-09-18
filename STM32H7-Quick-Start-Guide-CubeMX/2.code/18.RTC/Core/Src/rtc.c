#include "rtc.h"
#include "led.h"
#include "usart.h"
#include <stdio.h>

RTC_HandleTypeDef hrtc;
/* 月修正数据表 */
uint8_t const table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};

// 写入后备域SRAM
void rtc_write_bkr(uint32_t bkrx, uint32_t data)
{
	HAL_PWR_EnableBkUpAccess(); // 使能后备寄存器访问
	HAL_RTCEx_BKUPWrite(&hrtc, bkrx, data); // 写入数据
}

// 读取后备域SRAM
uint32_t rtc_read_bkr(uint32_t bkrx)
{
	uint32_t data;
	data = HAL_RTCEx_BKUPRead(&hrtc, bkrx); // 读取数据
	return data;
}

// 设置时间
void rtc_set_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t ampm)
{
	RTC_TimeTypeDef sTime = {0};
	sTime.Hours = hour;
	sTime.Minutes = min;
	sTime.Seconds = sec;
	sTime.TimeFormat = ampm;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
	{
		Error_Handler();
	}
}

// 设置日期
void rtc_set_date(uint8_t year, uint8_t month, uint8_t date, uint8_t week)
{
	RTC_DateTypeDef sDate = {0};
	sDate.Year = year;
	sDate.Month = month;
	sDate.Date = date;
	sDate.WeekDay = week;
	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
	{
		Error_Handler();
	}
}


/* RTC init function */
uint8_t MX_RTC_Init(void)
{
  uint16_t bkp_flag = 0; // 检查是不是第一次配置时钟
  // RTC 初始化
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24; // 24小时制
  hrtc.Init.AsynchPrediv = 0x7F; // 异步分频系数
  hrtc.Init.SynchPrediv = 0xFF;  // 同步分频系数
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE; // 不输出信号
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH; // 输出极性
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN; // 输出类型
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  bkp_flag = rtc_read_bkr(0); // 读取后备寄存器0的值
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
    return 1;
  }
  if((bkp_flag != 0x5050) && (bkp_flag != 0x5051))
  {
	  // 第一次配置RTC，手动设置初值
	  rtc_set_time(12, 12, 12, RTC_HOURFORMAT12_AM); // 设置时间 上午12:12:12
	  rtc_set_date(23, 3, 1, 3); // 设置日期 2023年3月1日 星期三
  }
  return 0;
}

/**
 * @brief       RTC底层驱动，时钟配置
 * @param       hrtc:RTC句柄
 * @note        此函数会被HAL_RTC_Init()调用
 * @retval      无
 */
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
    uint16_t retry = 200;

    RCC_OscInitTypeDef rcc_osc_init;
    RCC_PeriphCLKInitTypeDef rcc_periphclk_init;

    __HAL_RCC_RTC_CLK_ENABLE();     /* 使能RTC时钟 */
    HAL_PWR_EnableBkUpAccess();     /* 取消备份区域写保护 */
    __HAL_RCC_RTC_ENABLE();         /* RTC时钟使能 */

    /* 使用寄存器的方式去检测LSE是否可以正常工作 */
    RCC->BDCR |= 1 << 0;    /* 开启外部低速振荡器LSE */

    while (retry && ((RCC->BDCR & 0X02) == 0))  /* 等待LSE准备好 */
    {
        retry--;
        HAL_Delay(5);
    }
    if (retry == 0)     /* LSE起振失败，使用LSI */
    {
        rcc_osc_init.OscillatorType = RCC_OSCILLATORTYPE_LSI;           /* 选择要配置的振荡器 */
        rcc_osc_init.LSEState = RCC_LSI_ON;                             /* LSI状态：开启 */
        rcc_osc_init.PLL.PLLState = RCC_PLL_NONE;                       /* PLL无配置 */
        HAL_RCC_OscConfig(&rcc_osc_init);                               /* 配置设置的rcc_oscinitstruct */

        rcc_periphclk_init.PeriphClockSelection = RCC_PERIPHCLK_RTC;    /* 选择要配置的外设 RTC */
        rcc_periphclk_init.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;    /* RTC时钟源选择 LSI */
        HAL_RCCEx_PeriphCLKConfig(&rcc_periphclk_init);                 /* 配置设置的rcc_periphClkInitStruct */
        rtc_write_bkr(0, 0X5051);
    }
    else
    {
        rcc_osc_init.OscillatorType = RCC_OSCILLATORTYPE_LSE;           /* 选择要配置的振荡器 */
        rcc_osc_init.PLL.PLLState = RCC_PLL_NONE;                       /* PLL不配置 */
        rcc_osc_init.LSEState = RCC_LSE_ON;                             /* LSE状态：开启 */
        HAL_RCC_OscConfig(&rcc_osc_init);                               /* 配置设置的rcc_oscinitstruct */

        rcc_periphclk_init.PeriphClockSelection = RCC_PERIPHCLK_RTC;    /* 选择要配置外设 RTC */
        rcc_periphclk_init.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;    /* RTC时钟源为LSE */
        HAL_RCCEx_PeriphCLKConfig(&rcc_periphclk_init);                 /* 配置设置的rcc_periphclkinitstruct */
        rtc_write_bkr(0, 0X5050);
    }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();

    /* RTC interrupt Deinit */
    HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
    HAL_NVIC_DisableIRQ(RTC_Alarm_IRQn);
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

// 获取RTC时间
void rtc_get_time(uint8_t* hour, uint8_t* min, uint8_t* sec, uint8_t* ampm)
{
	RTC_TimeTypeDef sTime = {0};
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	*hour = sTime.Hours;
	*min = sTime.Minutes;
	*sec = sTime.Seconds;
	*ampm = sTime.TimeFormat;
}

// 获取RTC日期
void rtc_get_date(uint8_t* year, uint8_t* month, uint8_t* date, uint8_t* week)
{
	RTC_DateTypeDef sDate = {0};
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	*year = sDate.Year;
	*month = sDate.Month;
	*date = sDate.Date;
	*week = sDate.WeekDay;
}

/**
 * @breif       获得现在是星期几, 输入公历日期得到星期(只允许1901-2099年)
 * @param       year,month,day：公历年月日
 * @retval      星期号(1~7,代表周1~周日)
 */
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day)
{
    uint16_t temp2;
    uint8_t yearH, yearL;
    yearH = year / 100;
    yearL = year % 100;
    /*  如果为21世纪,年份数加100 */
    if (yearH > 19)yearL += 100;
    /*  所过闰年数只算1900年之后的 */
    temp2 = yearL + yearL / 4;
    temp2 = temp2 % 7;
    temp2 = temp2 + day + table_week[month - 1];
    if (yearL % 4 == 0 && month < 3)temp2--;
    temp2 %= 7;
    if (temp2 == 0)temp2 = 7;
    return temp2;
}

/**
 * @breif       设置闹钟时间(按星期闹铃,24小时制)
 * @param       week        : 星期几(1~7)
 * @param       hour,min,sec: 小时,分钟,秒钟
 * @retval      无
 */
void rtc_set_alarma(uint8_t week, uint8_t hour, uint8_t min, uint8_t sec)
{
    RTC_AlarmTypeDef rtc_alarm;

    rtc_alarm.AlarmTime.Hours = hour;                                /* 小时 */
    rtc_alarm.AlarmTime.Minutes = min;                               /* 分钟 */
    rtc_alarm.AlarmTime.Seconds = sec;                               /* 秒 */
    rtc_alarm.AlarmTime.SubSeconds = 0;
    rtc_alarm.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;

    rtc_alarm.AlarmMask = RTC_ALARMMASK_NONE;                        /* 精确匹配星期，时分秒 */
    rtc_alarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
    rtc_alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY; /* 按星期 */
    rtc_alarm.AlarmDateWeekDay = week;                               /* 星期 */
    rtc_alarm.Alarm = RTC_ALARM_A;                                   /* 闹钟A */
    HAL_RTC_SetAlarm_IT(&hrtc, &rtc_alarm, RTC_FORMAT_BIN);
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 1, 2);   /* 抢占优先级1,子优先级2 */
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}

/**
 * @breif       周期性唤醒定时器设置
 * @param       wksel
 *   @arg       RTC_WAKEUPCLOCK_RTCCLK_DIV16        ((uint32_t)0x00000000)
 *   @arg       RTC_WAKEUPCLOCK_RTCCLK_DIV8         ((uint32_t)0x00000001)
 *   @arg       RTC_WAKEUPCLOCK_RTCCLK_DIV4         ((uint32_t)0x00000002)
 *   @arg       RTC_WAKEUPCLOCK_RTCCLK_DIV2         ((uint32_t)0x00000003)
 *   @arg       RTC_WAKEUPCLOCK_CK_SPRE_16BITS      ((uint32_t)0x00000004)
 *   @arg       RTC_WAKEUPCLOCK_CK_SPRE_17BITS      ((uint32_t)0x00000006)
 * @note        000,RTC/16;001,RTC/8;010,RTC/4;011,RTC/2;
 * @note        注意:RTC就是RTC的时钟频率,即RTCCLK!
 * @param       cnt: 自动重装载值.减到0,产生中断.
 * @retval      无
 */
void rtc_set_wakeup(uint8_t wksel, uint16_t cnt)
{
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);  /* 清除RTC WAKE UP的标志 */

    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, cnt, wksel);          /* 设置重装载值和时钟 */

    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 2, 2);                       /* 抢占优先级2,子优先级2 */
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
}

/**
  * @brief This function handles RTC alarms (A and B) interrupt through EXTI line 17.
  */
void RTC_Alarm_IRQHandler(void)
{
  HAL_RTC_AlarmIRQHandler(&hrtc);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	printf("AlarmA Wakeup\r\n");
}

/**
  * @brief This function handles RTC wake-up interrupt through EXTI line 19.
  */
void RTC_WKUP_IRQHandler(void)
{

  HAL_RTCEx_WakeUpTimerIRQHandler(&hrtc);
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	HAL_GPIO_TogglePin(LED_RED_Port, LED_RED_Pin);
	printf("Wakeup\r\n");
}
