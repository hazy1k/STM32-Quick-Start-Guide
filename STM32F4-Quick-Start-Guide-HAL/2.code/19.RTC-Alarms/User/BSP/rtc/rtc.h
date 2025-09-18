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
