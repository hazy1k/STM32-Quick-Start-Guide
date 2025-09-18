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
