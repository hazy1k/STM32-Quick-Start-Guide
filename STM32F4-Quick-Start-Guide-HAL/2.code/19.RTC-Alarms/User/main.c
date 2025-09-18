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
