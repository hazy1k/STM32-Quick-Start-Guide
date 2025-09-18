#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "key.h"
#include "led.h"
#include "wwdg.h"

int main(void)
{
	HAL_Init();
	Stm32_Clock_Init(336,8,2,7);
	delay_init(168);
	uart_init(115200);
	KEY_Init();
	LED_Init();

	LED0_ON();
	delay_ms(300); // 延时300ms再启动看门狗

	// 窗口看门狗计算公式
	/*
		Fwwdg = PCLK1/(4096*(2^fp)) ,其中PCLK1为时钟频率一般为42Mhz
		函数参数：计数值、窗口值、分频系数
	*/
	WWDG_Init(0x7F, 0x5F, WWDG_PRESCALER_8); // 42Mhz/(4096*(2^8))=40Hz, 窗口值=0x5F, 计数值=0x7F
	while(1)
	{
		LED0_OFF();
	}
}
