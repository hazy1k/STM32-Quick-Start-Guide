#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "iwdg.h"

int main(void)
{
	HAL_Init();
	Stm32_Clock_Init(336,8,2,7);
	delay_init(168);
	uart_init(115200);
	LED_Init();
	KEY_Init();
	delay_ms(100);
	// 初始化IWGD，预分频64，重载值512
	// 计算公式：Time = (64 * 512) / LSI
	IWDG_Init(IWDG_PRESCALER_64, 512); // 设置超时时间为1S 
	LED0_ON();
	while(1)
	{
		if(KEY_Scan(0) == KEY_UP_PRES) // 如果按键被按下，进行喂狗
		{
			IWDG_Feed(); // 喂狗
		}
		delay_ms(10); // 延时10ms
	}
}
