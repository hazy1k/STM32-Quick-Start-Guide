#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "led.h"

int main(void)
{
	HAL_Init();
	Stm32_Clock_Init(336,8,2,7);
	delay_init(168);
	uart_init(115200);
	LED_Init();
	// 定时器初始化，设置重装值(arr)为5000-1，分频系数(psc)为8400-1
	// 这个工程中我们的定时器的工作频率为84MHz/8400=10KHz
	// 周期计算Time = ((arr+1)*(psc+1))/fclk = 5000*(8400)/84000000 = 500ms
	TIM_Config(5000-1,8400-1);
	while(1)
	{
		LED0 =! LED0;
		delay_ms(200);	
	}	
}
// LED0每200ms翻转一次
// LED1通过定时器中断每隔500ms翻转一次
