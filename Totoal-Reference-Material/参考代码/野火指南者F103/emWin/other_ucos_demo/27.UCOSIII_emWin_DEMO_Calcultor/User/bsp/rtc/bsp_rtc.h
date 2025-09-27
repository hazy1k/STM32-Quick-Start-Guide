#ifndef __RTC_H
#define	__RTC_H

#include "stm32f10x.h"
#include "..\User\bsp\rtc\bsp_calendar.h"
#include "..\User\bsp\rtc\bsp_date.h"
#include "..\User\bsp\usart\bsp_usart1.h"

/*等待超时时间*/
#define RTC_FLAG_TIMEOUT         ((uint32_t)0x1000000)
#define RTC_LONG_TIMEOUT         ((uint32_t)(10 * RTC_FLAG_TIMEOUT))

enum
{
  RTC_OK=0,
  RTC_TIMEOUT,
};
/* 如果定义了下面这个宏的话,PC13就会输出频率为RTC Clock/64的时钟 */   
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */

void RTC_NVIC_Config(void);
int8_t RTC_Configuration(void);
void Time_Regulate(struct rtc_time *tm);
void Time_Adjust(struct rtc_time *tm);
void Time_Display(uint32_t TimeVar,struct rtc_time *tm);
void Time_Show(struct rtc_time *tm);
static uint8_t USART_Scanf(uint32_t value);
int8_t RTC_CheckAndConfig(struct rtc_time *tm);
void RTC_TimeCovr(struct rtc_time *tm);

int8_t Time_Adjust_LCD(struct rtc_time *DecTm,struct rtc_time *SrcTm);

#endif /* __XXX_H */

