#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern RTC_HandleTypeDef hrtc;

void rtc_write_bkr(uint32_t bkrx, uint32_t data);
uint32_t rtc_read_bkr(uint32_t bkrx);
void rtc_set_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t ampm);
void rtc_set_date(uint8_t year, uint8_t month, uint8_t date, uint8_t week);
uint8_t MX_RTC_Init(void);
void rtc_get_time(uint8_t* hour, uint8_t* min, uint8_t* sec, uint8_t* ampm);
void rtc_get_date(uint8_t* year, uint8_t* month, uint8_t* date, uint8_t* week);
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day);
void rtc_set_alarma(uint8_t week, uint8_t hour, uint8_t min, uint8_t sec);
void rtc_set_wakeup(uint8_t wksel, uint16_t cnt);


#ifdef __cplusplus
}
#endif

#endif /* __RTC_H__ */

