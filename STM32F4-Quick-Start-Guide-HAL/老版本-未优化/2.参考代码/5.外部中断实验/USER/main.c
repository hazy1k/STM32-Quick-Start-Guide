#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "exti.h"

int main(void)
{
	HAL_Init();
	Stm32_Clock_Init(336,8,2,7);
	delay_init(168);
	uart_init(115200);
	LED_Init();
	KEY_Init();
	EXTI_Init();
	while(1)
	{
		printf("串口正常工作中...\r\n");
		delay_ms(1000);
	}
}
